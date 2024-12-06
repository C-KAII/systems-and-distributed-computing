#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_PLAYERS 5  // Maximum number of players allowed
#define PORT 5555
#define TIMEOUT 20000  // 20 seconds timeout for players

int total = 25;          // The game's total number (starting value)
int current_player = 0;  // Keeps track of whose turn it is

struct Player {
  int socket;
  int errors;  // Track consecutive errors for each player
};

struct Player players[MAX_PLAYERS];
int num_players = 0;

void send_message(int client_socket, const char *message) {
  send(client_socket, message, strlen(message), 0);
}

void broadcast_message(const char *message) {
  for (int i = 0; i < num_players; i++) {
    send_message(players[i].socket, message);
  }
}

void remove_player(int index) {
  printf("Player %d disconnected\n", index + 1);
  close(players[index].socket);

  for (int i = index; i < num_players - 1; i++) {
    players[i] = players[i + 1];
  }

  num_players--;

  // Adjust the current_player index to ensure it's valid
  if (current_player >= num_players) {
    current_player = 0;  // Reset to the first player if out of bounds
  }
}

void process_client_message(int client_index, const char *message) {
  char buffer[BUFFER_SIZE];
  strncpy(buffer, message, BUFFER_SIZE);

  // If the message starts with "MOVE", process the move
  if (strncmp(buffer, "MOVE", 4) == 0) {
    int move = atoi(buffer + 5);  // Extract the number after "MOVE"
    if (move < 1 || move > 9) {
      send_message(players[client_index].socket,
                   "TEXT ERROR Invalid move. Try again.\nGO\n");
      players[client_index].errors++;

      if (players[client_index].errors >= 5) {
        send_message(players[client_index].socket,
                     "TEXT Too many errors, you're out.\nEND\n");
        remove_player(client_index);
      }
    } else {
      // players[client_index].errors = 0; // Only if we want to reset errors
      // each round
      total -= move;
      if (total <= 0) {
        send_message(players[client_index].socket, "TEXT You won!\nEND\n");

        snprintf(buffer, BUFFER_SIZE,
                 "TEXT You lost! Player %d won this round.\nEND\n",
                 client_index + 1);
        broadcast_message(buffer);
      } else {
        // Update the total and proceed with the game
        snprintf(buffer, BUFFER_SIZE, "TEXT Total is %d.\n", total);
        broadcast_message(buffer);
      }
    }
  } else if (strcmp(buffer, "QUIT") == 0) {
    send_message(players[client_index].socket, "END\n");
    remove_player(client_index);
  } else {
    // Invalid message, send END and remove player
    send_message(players[client_index].socket,
                 "TEXT ERROR Invalid message. Disconnecting...\nEND\n");
    remove_player(client_index);
  }
}

void handle_game() {
  char buffer[BUFFER_SIZE];
  struct pollfd fds[MAX_PLAYERS];

  // Set up polling for each player socket
  for (int i = 0; i < num_players; i++) {
    fds[i].fd = players[i].socket;
    fds[i].events = POLLIN;
  }

  // Broadcast starting total to players
  snprintf(buffer, BUFFER_SIZE, "TEXT Starting total is %d.\n", total);
  broadcast_message(buffer);

  while (total > 0 && num_players > 1) {
    struct Player *player = &players[current_player];

    // Inform the current player of their turn
    snprintf(buffer, BUFFER_SIZE, "GO\n");
    send_message(player->socket, buffer);

    // Wait for the current player's move (with timeout)
    int ret = poll(&fds[current_player], 1, TIMEOUT);  // 20 seconds timeout

    if (ret > 0 && (fds[current_player].revents & POLLIN)) {
      // Receive the move from the player
      int bytes_received = recv(player->socket, buffer, BUFFER_SIZE, 0);
      if (bytes_received <= 0) {
        remove_player(current_player);  // Player disconnection
        continue;
      }
      buffer[bytes_received] = '\0';

      process_client_message(current_player, buffer);
    } else if (ret == 0) {
      // Timeout occurred
      send_message(player->socket,
                   "TEXT You took too long. You're out!\nEND\n");
      remove_player(current_player);
      continue;  // Move to the next player after timeout
    }

    // Move to the next player
    if (num_players > 0) {
      current_player = (current_player + 1) % num_players;
    }
  }

  // If there's one player left, they win by default
  if (num_players == 1) {
    send_message(players[0].socket, "TEXT You won by default!\nEND\n");
  }
}

int main(int argc, char *argv[]) {
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;

  // Create the server socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("ERROR Could not create socket");
    exit(EXIT_FAILURE);
  }

  // Configure the server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket
  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    perror("ERROR Could not bind to port");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  // Listen for clients
  if (listen(server_socket, MAX_PLAYERS) < 0) {
    perror("ERROR Could not listen for clients");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  printf("Server is running, waiting for players...\n");

  int min_start_players = MAX_PLAYERS;
  if (argc > 3) {
    min_start_players = atoi(argv[3]);
    if (min_start_players > MAX_PLAYERS) {
      // Ensure hard limit on max players for memory safety
      min_start_players = MAX_PLAYERS;
    }
  }

  // Accept clients until the game starts
  addr_size = sizeof(client_addr);
  while (num_players < MAX_PLAYERS) {
    client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket < 0) {
      perror("ERROR Could not accept client");
      close(server_socket);
      exit(EXIT_FAILURE);
    }

    players[num_players].socket = client_socket;
    players[num_players].errors = 0;
    num_players++;

    printf("Player %d connected\n", num_players);
    send_message(client_socket, "TEXT Welcome to the game\n");

    // Start the game if the required number of players have joined
    if (num_players >= min_start_players) {
      printf("Starting the game...\n");
      handle_game();
      break;
    }
  }

  // Clean up
  close(server_socket);
  return 0;
}
