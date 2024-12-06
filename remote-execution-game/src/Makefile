CC=gcc
CFLAGS=-Wall -pthread
SERVER=game_server
CLIENT=game_client

all: $(SERVER) $(CLIENT)

$(SERVER): game_server.c
	$(CC) $(CFLAGS) -o $(SERVER) game_server.c

$(CLIENT): game_client.c
	$(CC) $(CFLAGS) -o $(CLIENT) game_client.c

clean:
	rm -f $(SERVER) $(CLIENT)
