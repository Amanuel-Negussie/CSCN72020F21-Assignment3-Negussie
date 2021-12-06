// cscn72020f21 - a new and improved (using library v2) tcp server.
// steveh - nov 2021

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetworkingFunctions.h"

#define LISTENINGPORT   "8080"      //needs to be a string

int main(void)
{
    
    InitializeWindowsSockets();
    NOTE listOfNotes[MAX_NOTES],* listP;
    listP = &listOfNotes;
    IntializeData(listP);
    memset(&listOfNotes, NULL, sizeof(NOTE) * MAX_NOTES);
    
    printf("Config the local addr...\n");
    struct addrinfo* bind_address = ConfigureLocalAddress(LISTENINGPORT, TCP);

    printf("Creating socket...\n");
    SOCKET socket_listen = CreateBindListeningSocket(bind_address);

    printf("Start listening...\n");
    StartListeningForConnections(socket_listen);
 
    printf("Waiting for connection...\n");
    //SOCKET socket_client = WaitForAndAcceptConnection(socket_listen);
    WaitForAndAcceptAndHandleMultiplexedConnections(socket_listen, listP); //Added in for multiplexed connections
  

    printf("Closing listening socket...\n");
    CloseSocketConnection(socket_listen);

    ShutdownWindowsSockets();
    return 0;
}