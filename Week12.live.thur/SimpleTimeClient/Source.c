#include "NetworkingFunctions.h"
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define MAXSIZE		1000
int main(void)
{
	InitializeWindowsSockets();

	struct addrinfo* peerAddress = ConfigureRemoteAddress("127.0.0.1", "8080");

	SOCKET peer = CreateAndConnectRemoteSocket(peerAddress);

	char* message = "get current time\0";
	send(peer, message, strlen(message), 0);

	char buffer[MAXSIZE];
	int bytes_recv = recv(peer, buffer, MAXSIZE, 0);
	if (bytes_recv <= 0)
		fprintf(stderr, "problem receiving data\n");

	ShutdownWindowsSockets(peer);
	ShutdownWindowsSockets();
	return 0;
}