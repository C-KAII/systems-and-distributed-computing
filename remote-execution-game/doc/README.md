1.	User Instructions
Compilation and running server and client:

1.	Compile the server and client using the provided Makefile.
>	make
2.	Run the Game Server with the following command:
>	./game_server <port_number> numbers [<number_of_players>]

•	Replace <port_number> with the desired port number (e.g., 5555).
•	Optional - Replace <number_of_players> with the desired number of players for the game. Else, defaults to MAX_PLAYERS (5)

3.	Run the Game Client with the following command:
>	./game_client numbers <server_ip> <port_number>

•	Replace <server_ip> with the server's IP address or name (e.g., 192.168.0.20 (localhost) if running locally).
•	Replace <port_number> with the same port number as the server.

Playing the Game:

•	Once connected, follow the on-screen instructions to enter a number between 1 and 9.
•	Type "quit" to exit the game when it is your turn to move.
•	The game ends when a player reduces the total to 0 or less, or when all players quit.
