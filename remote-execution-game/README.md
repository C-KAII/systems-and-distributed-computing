# Systems and Distributed Computing – Remote Execution Game

## Objective

Develop a simple remote execution system comprising two programs:  
- **Game Server**: Manages the game and communicates with clients.  
- **Game Client**: Used by players to communicate with the server.  

The game, called **"Numbers"**, involves multi-player interactions through a network using a predefined protocol.

## System Overview

### Programs

1. **Game Server** (`game_server`):  
  Listens on a specified port for client connections and manages the game.  
  Command format:  
```bash
game_server <Port Number> <Game Type> <Game Arguments>
```
  Example:  
```bash
game_server 5555 numbers 3
```
2. **Game Client** (`game_client`):  
  Connects to the game server and allows players to participate.  
  Command format:  
```bash
game_client <Game Type> <Server Name> <Port Number>
```
  Example:
```bash
game_client numbers mypc 5555
```

## Game Protocol

### Stages

1. **Initiate**: Server starts with port, game type, and arguments from the command line.
2. **Join**: Server accepts players, sending "welcome to the game" upon connection.
3. **Play**: Players take turns; players can leave, and the game ends if participants drop below the minimum.
4. **Game Over**: Server announces results and closes all resources.

### Message Types

#### **Server to Client Messages**

1. **`TEXT <text>`**: Displays `<text>` on the client screen.  
  Example:  
    TEXT It is your turn
2. **`GO`**: Signals the client to input a move.
3. **`END`**: Indicates the game is over; clients should close connections.
4. **`ERROR`**: Indicates a protocol error or invalid move.

#### **Client to Server Messages**

1. **`MOVE <move>`**: Sends the player’s move to the server.  
  Example:  
    MOVE 4
2. **`QUIT`**: Informs the server the player is quitting. Sent only in response to a `GO` message.

## Game Rules: Numbers

- The game starts with a total of 25.
- Players take turns subtracting a number between 1 and 9.
- The player who reduces the total to 0 or less wins.

### Example Game Flow (2 Players)

1. **Server to Client 1**:
  TEXT Total is 25. Enter number:
  GO
  **Client 1 to Server**:
  MOVE 8
2. **Server to Client 2**:
  TEXT Total is 17. Enter number:
  GO
  **Client 2 to Server**:
  MOVE 15
  **Server to Client 2**:
  TEXT ERROR Bad input. Try again:
  GO

## Error Handling

- **Protocol Errors**:  
  Invalid messages result in immediate disconnection (`END` message).
- **Game Errors**:  
  Errors result in `TEXT ERROR` messages. Players can retry, but after 5 consecutive errors, they are disconnected.

## Winning Conditions

- A player wins by reducing the total to 0 or less.
- If only one player remains, they win by default.

## Program Termination

- The server terminates if no players remain or due to critical errors.
- The client terminates after receiving an `END` message or upon a `QUIT` request.

## Example Commands

### Server
```bash
game_server 5555 numbers 3
```

### Client
```bash
game_client numbers mypc 5555
```