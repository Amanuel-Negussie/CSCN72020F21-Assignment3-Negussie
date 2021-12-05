//cscn72020 - week13 - a simple UDP based timeserver
// steveh - nov 2021

#include <stdio.h>
#include <string.h>
#include "NetworkingFunctions.h"

#define LISTENINGPORT   "2056"  //as a string!

int main(void)
{
    InitializeWindowsSockets();
 
    printf("Config the local addr...\n");
    struct addrinfo* bind_address = ConfigureLocalAddress(LISTENINGPORT, UDP);

    printf("Creating socket...\n");
    SOCKET socket_listen = CreateBindListeningSocket(bind_address);

    printf("Waiting for client...");
    RecvUDPRequestAndSendResponse(socket_listen);

    printf("Closing listening socket...\n");
    CloseSocketConnection(socket_listen);

    ShutdownWindowsSockets();
    //comment
    return 0;
}

