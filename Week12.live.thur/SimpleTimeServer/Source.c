
#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x6000
#endif

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetworkingFunctions.h"

#pragma comment(lib, "ws2_32.lib")


int main(void)
{
    InitializeWindowsSockets();

    printf("Config the local addr...\n");
    struct addrinfo* bind_address = ConfigureLocalAddress();

    printf("Creating socket...\n");
    SOCKET socket_listen = CreateBindListeningSocket(bind_address);

    printf("Start listening...\n");
    StartListeningForConnections(socket_listen);
 
    printf("Waiting for connection...\n");
    SOCKET socket_client = WaitForAndAcceptConnection(socket_listen);

    printf("Client is connected...");
    RecvRequestAndSendResponse(socket_client);

    printf("Closing the connection...\n");
    CloseSocketConnection(socket_client);

    printf("Closing listening socket...\n");
    CloseSocketConnection(socket_listen);

    ShutdownWindowsSockets();
    return 0;
}