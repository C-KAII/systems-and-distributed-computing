#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_KEY 1234  // Shared memory key
#define SHM_SIZE sizeof(struct shared_memory)
#define MAX_QUERIES 12
#define NUM_THREADS 32
#define INT_BITS 32

struct shared_memory {
  // Using unsigned int as it's 32 bits on my device
  unsigned int clientslot;         // Shared 32-bit variable for client requests
  unsigned int slot[MAX_QUERIES];  // Shared array for server responses
  char clientflag;                 // Flag for client to indicate a request
  char serverflag[MAX_QUERIES];    // Flags for server to indicate responses
  char progress[MAX_QUERIES];      // Progress array for each query
};

struct factorise_args {
  // int f_tid; // Factor thread id
  int s_tid;           // Slot thread id
  unsigned int f_num;  // Number to factor
};

struct shared_memory *shm;
sem_t *sem_slot_write, *sem_slot_read;
char slotflag[MAX_QUERIES];  // Flag to indicate available thread slots

unsigned int right_rotate(unsigned int num, unsigned int tid) {
  return (num >> tid % INT_BITS) | (num << (INT_BITS - tid) % INT_BITS);
}

void *factorise(void *args) {
  struct factorise_args *ptr = (struct factorise_args *)args;
  // int f_tid = ptr->f_tid;
  int s_tid = ptr->s_tid;
  unsigned int num = ptr->f_num;

  // unsigned int original_num = num;
  unsigned int largest_factor = 1;

  while (num % 2 == 0) {
    largest_factor = 2;
    num /= 2;
  }
  for (unsigned int i = 3; i <= sqrt(num); i += 2) {
    while (num % i == 0) {
      largest_factor = i;
      num /= i;
    }
  }

  if (num > 2) {
    largest_factor = num;
  }

  sem_wait(sem_slot_write);  // Wait for permission to write

  // Critical section
  shm->slot[s_tid] = largest_factor;
  shm->serverflag[s_tid] = 1;

  // printf("largest_factor of num %u for slot #%d == %u - on thread #%d\n",
  // original_num, s_tid, largest_factor, f_tid);
  shm->progress[s_tid] += 1;  // Update progress
  sem_post(sem_slot_read);    // Signal client to read

  pthread_exit(NULL);
}

void *test_thread(void *arg) {
  int tid = *(int *)arg;
  for (int i = tid * 10; i < (tid + 1) * 10; i++) {
    int delay = (rand() % 91) + 10;  // Random delay
    usleep(delay * 1000);            // Convert to microseconds
    printf("Test thread #%d returns number: %d\n", tid, i);
  }
  pthread_exit(NULL);
}

void *test_mode(void *arg) {
  pthread_t test_threads[10];

  for (int set = 0; set < 3; set++) {
    printf("Starting test set #%d\n", set + 1);
    for (int i = 0; i < 10; i++) {
      int *tid = malloc(sizeof(int));
      *tid = i;
      pthread_create(&test_threads[i], NULL, test_thread, tid);
    }

    for (int i = 0; i < 10; i++) {
      pthread_join(test_threads[i], NULL);
    }
  }
  printf("Test mode complete.\n");
  pthread_exit(NULL);
}

void *process_request(void *arg) {
  int slot_id = *(int *)arg;
  unsigned int query = shm->slot[slot_id];

  printf("Server received query: %u - Allocated to slot #%d\n", query, slot_id);

  // Start up 32 threads and factorise
  pthread_t f_threads[NUM_THREADS];
  struct factorise_args f_args[NUM_THREADS];  // 12 args for thread safety

  int smallest_num = query;
  for (int i = 0; i < NUM_THREADS; i++) {
    unsigned int rotated_num = right_rotate(query, i);
    // Ensure we don't wrap to numbers higher than query
    if (rotated_num > smallest_num) {
      rotated_num = smallest_num;
    } else {
      smallest_num = rotated_num;
    }

    f_args[i].f_num = rotated_num;
    f_args[i].s_tid = slot_id;
    // f_args[i].f_tid = i;
    pthread_create(&f_threads[i], NULL, factorise, (void *)&f_args[i]);
    usleep(100);  // Separate threads slightly
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(f_threads[i], NULL);
  }

  // printf("Server finished processing request in slot #%d\n", slot_id);

  // Indicate to client query is complete
  shm->progress[slot_id] = 0;  // Reset progress
  shm->slot[slot_id] = 0;
  slotflag[slot_id] = 0;  // Make thread slot available

  pthread_exit(NULL);
}

int main() {
  srand(time(NULL));  // Seed random number generator for test mode

  setbuf(stdout, NULL);  // Disable server output buffering

  int shmid;
  // Create shared memory
  shmid = shmget(SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
  if (shmid == -1) {
    perror("Shared memory creation failed");
    exit(1);
  }

  // Attach to shared memory
  shm = (struct shared_memory *)shmat(shmid, NULL, 0);
  if (shm == (void *)-1) {
    perror("Shared memory attach failed");
    exit(1);
  }

  // Init semaphores
  // Binary semaphore for writing
  sem_slot_write = sem_open("/sem_slot_write", O_CREAT, 0644, 1);
  // Semaphore for reading
  sem_slot_read = sem_open("/sem_slot_read", O_CREAT, 0644, 0);
  if (sem_slot_write == SEM_FAILED || sem_slot_read == SEM_FAILED) {
    perror("sem_open failed");
    exit(1);
  }

  printf("Server connected to shared memory and initialised semaphores.\n");

  // Init flags, slots and mutexes
  shm->clientflag = 0;
  shm->clientslot = 0;
  for (int i = 0; i < MAX_QUERIES; i++) {
    shm->slot[i] = 0;
    shm->serverflag[i] = 0;
    shm->progress[i] = 0;
    slotflag[i] = 0;
  }

  // Init threads
  pthread_t slot_threads[MAX_QUERIES];
  int thread_args[MAX_QUERIES];
  int main_thread_index = 0;

  // Client request slot allocation loop
  while (1) {
    if (shm->clientflag == 1) {
      // Check for test mode
      if (shm->clientslot == 0) {
        printf("Test mode initiated.\n");
        pthread_t test_mode_thread;
        pthread_create(&test_mode_thread, NULL, test_mode, NULL);
        pthread_join(test_mode_thread, NULL);
        shm->clientflag = 0;  // Reset clientflag after test mode
      } else {
        // Find an available slot
        int slot_id = -1;
        for (int i = 0; i < MAX_QUERIES; i++) {
          if (slotflag[i] == 0) {
            slot_id = i;
            slotflag[slot_id] = 1;

            // Store query in available slot
            shm->slot[slot_id] = shm->clientslot;

            // Start up slot thread for processing
            thread_args[main_thread_index] = slot_id;
            pthread_create(&slot_threads[main_thread_index], NULL,
                           process_request, &thread_args[main_thread_index]);
            main_thread_index++;

            // Indicate to client which slot is processing request and request
            // is handled
            shm->clientslot = slot_id;
            shm->clientflag = 0;  // Reset the clientflag as request is handled
            break;
          }
        }
      }

      if (slot_id == -1) {
        usleep(200000);
      }
    } else {
      usleep(500000);  // 500 ms update delay

      for (int i = 0; i < MAX_QUERIES; i++) {
        if (shm->progress[i] > 0) {
          int progress = (shm->progress[i] * 100) / 32;
          progress = (progress / 5) * 5;  // Round to nearest 5%
          printf("Query %d: %d%% ", i, progress);
        }
      }
    }
  }

  for (int i = 0; i < main_thread_index; i++) {
    pthread_join(slot_threads[i], NULL);
  }

  // Clean up semaphores
  sem_close(sem_slot_write);
  sem_close(sem_slot_read);
  sem_unlink("/sem_slot_write");
  sem_unlink("/sem_slot_read");

  // Detach from shared memory
  shmdt((void *)shm);

  return 0;
}
