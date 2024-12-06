#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void send_message(int server_socket, const char *message) {
  send(server_socket, message, strlen(message), 0);
}

void process_server_message(int server_socket, const char *message) {
  char buffer[BUFFER_SIZE];
  strncpy(buffer, message, BUFFER_SIZE);

  // Tokenise by newline sep
  char *line = strtok(buffer, "\n");

  while (line != NULL) {
    if (strncmp(line, "TEXT", 4) == 0) {
      printf("%s\n", line + 5);
    } else if (strcmp(line, "GO") == 0) {
      int valid_move = 0;
      while (!valid_move) {
        printf("Enter number: ");
        char move[BUFFER_SIZE];
        fgets(move, BUFFER_SIZE, stdin);
        move[strcspn(move, "\n")] = 0;  // Remove newline character

        if (strncmp(move, "quit", 4) == 0) {
          send_message(server_socket, "QUIT\n");
          break;
        }

        int move_val = atoi(move);
        if (move_val >= 1 && move_val <= 9) {
          snprintf(buffer, BUFFER_SIZE, "MOVE %.1018s", move);
          send_message(server_socket, buffer);
          valid_move = 1;
        } else {
          printf("Invalid move. Please enter a number between 1 and 9.\n");
        }
      }
    } else if (strcmp(line, "END") == 0) {
      printf("Game over! Terminating...\n");
      exit(0);
    }

    // Move to the next part of the message
    line = strtok(NULL, "\n");
  }
}

int main(int argc, char *argv[]) {
  int server_socket;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <Game Type> <Server Name> <Port Number>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  // Create the socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("ERROR Could not create socket");
    exit(EXIT_FAILURE);
  }

  // Configure the server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[3]));
  server_addr.sin_addr.s_addr =
      inet_addr(argv[2]);  // Use IP address instead of name for simplicity

  // Connect to the server
  if (connect(server_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    perror("ERROR Could not connect to server");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  printf("Connected to the server\n");

  while (1) {
    // Receive message from the server
    int bytes_received = recv(server_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      break;  // Server closed connection
    }
    buffer[bytes_received] = '\0';

    process_server_message(server_socket, buffer);
  }

  close(server_socket);
  return 0;
}
