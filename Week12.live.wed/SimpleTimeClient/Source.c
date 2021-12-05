#include "NetworkingFunctions.h"
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main(void)
{
	InitializeWindowsSockets();

	printf("Config the local addr...\n");
	struct addrinfo* bind_address = ConfigureRemoteAddress("127.0.0.1", "8080");

	SOCKET peerSocket = CreateAndConnectRemoteSocket(bind_address);

	char* message = "Get current time\n";
	int bytes_sent = send(peerSocket, message, (int)strlen(message), 0);
	if (bytes_sent == 0)
		fprintf(stderr, "something is wrong\n");

	char buffer[1000];
	int bytes_recvd = recv(peerSocket, buffer, (int)strlen(buffer), 0);
	if (bytes_recvd == 0)
		fprintf(stderr, "something went wrong\n");

	CloseSocketConnection(peerSocket);

	ShutdownWindowsSockets();
	return 0;
}