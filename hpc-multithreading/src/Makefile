# Makefile for client-server system

CC = gcc
CFLAGS = -Wall -pthread
SHM_LIBS = -lrt
MATH_LIBS = -lm

# Targets
CLIENT = client
SERVER = server

# Object files
CLIENT_OBJ = client.o
SERVER_OBJ = server.o

# Build all
all: $(CLIENT) $(SERVER)

# Build client
$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ) $(SHM_LIBS)

# Build server
$(SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ) $(SHM_LIBS) $(MATH_LIBS)

# Compile client source file
client.o: client.c
	$(CC) $(CFLAGS) -c client.c

# Compile server source file
server.o: server.c
	$(CC) $(CFLAGS) -c server.c

# Clean up
clean:
	rm -f $(CLIENT) $(SERVER) *.o

.PHONY: all clean
