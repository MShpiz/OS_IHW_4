all: client server log
client: client.c DieWithError.c
	gcc client.c DieWithError.c -o client
server: server.c DieWithError.c
	gcc TCPEchoServer.h server.c DieWithError.c \
	HandleTCPClient.c CreateTCPServerSocket.c \
	-o server
log: log_server.c DieWithError.c
	gcc log_server.h log_server.c DieWithError.c \
	handleLogClient.c CreateTCPServerSocket.c \
	-o log
