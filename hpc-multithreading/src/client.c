#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define SHM_KEY 1234  // Shared memory key
#define SHM_SIZE sizeof(struct shared_memory)
#define MAX_QUERIES 12

struct shared_memory {
  // Using unsigned int as it's 32 bits on my device
  unsigned int clientslot;         // Shared 32-bit variable for client requests
  unsigned int slot[MAX_QUERIES];  // Shared array for server responses
  char clientflag;                 // Flag for client to indicate a request
  char serverflag[MAX_QUERIES];    // Flags for server to indicate responses
  char progress[MAX_QUERIES];      // Progress array for each query
};

struct shared_memory *shm;
sem_t *sem_slot_write, *sem_slot_read;

struct timeval start_time[MAX_QUERIES];  // To track time of requests
struct timeval end_time[MAX_QUERIES];

char test_mode_flag;

void *progress_thread(void *arg) {
  while (1) {
    usleep(500000);  // 500ms delay between progress updates

    for (int i = 0; i < MAX_QUERIES; i++) {
      if (shm->progress[i] > 0) {
        int progress = (shm->progress[i] * 100) / 32;
        progress = (progress / 5) * 5;  // Round to nearest 5%
        printf("Query %d: %d%% ", i, progress);
      }
    }

    while (test_mode_flag == 1) {
      sleep(2);
    }
  }
  pthread_exit(NULL);
}

void *print_thread(void *arg) {
  while (1) {
    sem_wait(sem_slot_read);
    for (int i = 0; i < MAX_QUERIES; i++) {
      if (shm->serverflag[i] == 1) {
        // Check if slot == 0 as that indicates full query complete
        if (shm->slot[i] == 0) {
          // Record the time when the response was received
          gettimeofday(&end_time[i], NULL);

          // Calculate the time difference
          int seconds = end_time[i].tv_sec - start_time[i].tv_sec;
          int microseconds = end_time[i].tv_usec - start_time[i].tv_usec;
          double elapsed = seconds + microseconds * 1e-6;

          // Print the response and time taken
          printf("All factors found for slot #%d (Time taken: %.3f seconds)\n",
                 i, elapsed);
        } else {
          printf("Slot %d: %u\n", i, shm->slot[i]);
        }

        shm->serverflag[i] = 0;
        sem_post(sem_slot_write);
      }
    }
    usleep(50000);  // 50ms sleep to avoid high CPU usage
  }
  pthread_exit(NULL);
}

int main() {
  setbuf(stdout, NULL);

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

  sem_slot_write = sem_open("/sem_slot_write", 0);
  sem_slot_read = sem_open("/sem_slot_read", 0);
  if (sem_slot_write == SEM_FAILED || sem_slot_read == SEM_FAILED) {
    perror("sem_open failed");
    exit(1);
  }

  printf("Client connected to shared memory and initialised semaphores.\n");

  // Create thread to monitor and print server responses
  pthread_t print_tid;
  pthread_create(&print_tid, NULL, print_thread, NULL);

  // Create thread to display progress
  pthread_t progress_tid;
  pthread_create(&progress_tid, NULL, progress_thread, NULL);

  test_mode_flag = 0;
  printf("Enter an unsigned 32-bit integer to factor (or type 'quit'):\n");
  while (1) {
    char input[BUF_SIZE];
    unsigned long temp_query;
    char *endptr;

    if (!fgets(input, sizeof(input), stdin)) {
      // Error reading input
      perror("Error reading input");
      continue;
    }

    // Remove newline char
    input[strcspn(input, "\n")] = 0;

    // Special case: user exits with 'quit' query
    if (strcmp(input, "quit") == 0) {
      printf("Exiting...\n");
      exit(0);
    }

    // Attempt conversion to unsigned int
    errno = 0;
    temp_query = strtoul(input, &endptr, 10);

    // Invalid input handling
    if (errno == ERANGE || temp_query > UINT_MAX) {
      printf("Invalid input: value out of range for 32-bit unsigned integer\n");
      continue;
    } else if (endptr == input || *endptr != '\0') {
      printf("Invalid input: please enter a valid number\n");
      continue;
    }

    if (temp_query == 0) {
      char processing_flag = 0;
      for (int i = 0; i < MAX_QUERIES; i++) {
        if (shm->serverflag[i] != 0) {
          printf("Server cannot run test mode while processing queries.\n");
          processing_flag = 1;
          break;
        }
      }
      if (processing_flag == 0) {
        printf("Entering test mode...\n");
        test_mode_flag = 1;
      }
    }

    unsigned int query = (unsigned int)temp_query;  // Convert to 32 bit format

    // Check if we can write to clientslot
    if (shm->clientflag == 0) {
      // Write request to clientslot and send the request to server
      shm->clientslot = query;
      shm->clientflag = 1;  // Indicate that a request is available

      // Wait until request is accepted
      while (shm->clientflag == 1) {
        usleep(1000);
      }
      // Record the time the request was accepted
      gettimeofday(&start_time[shm->clientslot], NULL);
    } else {
      printf("Server is busy...\n");
      sleep(1);
      continue;
    }
  }

  pthread_join(print_tid, NULL);
  pthread_join(progress_tid, NULL);

  // Clean up semaphores
  sem_close(sem_slot_write);
  sem_close(sem_slot_read);

  // Detach from shared memory
  shmdt((void *)shm);

  return 0;
}
