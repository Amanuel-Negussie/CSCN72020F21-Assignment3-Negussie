//cscn72020f21 - simple network client to attach to our simple time server
// steveh - nov 2021

#include "NetworkingFunctions.h"
#include <stdio.h>


#define MAXBUFFER	200

int main(void)
{
	// first set up Winsock  (same as server)
	InitializeWindowsSockets();

	printf("Config the remote addr...\n");
	struct addrinfo* peer_address = ConfigureRemoteAddress("127.0.0.1", "8080", TCP);

	printf("Creating socket and connect...\n");
	SOCKET peer_socket = CreateAndConnectRemoteSocket(peer_address);

	//send initial 'greeting'
	char* message = "get current time\n\0";
	int sent = send(peer_socket, message, strlen(message), 0);
	if (sent == 0)
	{
		fprintf(stderr, "send failed\n");
		exit(1);
	}

	//receive reply from server
	char buffer[MAXBUFFER];
	memset(buffer, '\0', MAXBUFFER);

	while (recv(peer_socket, buffer, MAXBUFFER, 0) != 0)
		;

	printf("%s", buffer);

	printf("Closing the connection...\n");
	CloseSocketConnection(peer_socket);
	ShutdownWindowsSockets();
}