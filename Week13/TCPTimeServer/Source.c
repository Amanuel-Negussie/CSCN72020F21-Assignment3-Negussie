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
	NOTE listOfNotes[MAX_NOTES], * listP;
	listP = &listOfNotes;
	memset(&listOfNotes, '\0', sizeof(NOTE) * MAX_NOTES);

	InitializeData(listP);
	strcpy_s(&listOfNotes[0].Author, sizeof(listOfNotes->Author), "Author");
	strcpy_s(&listOfNotes[0].topic, sizeof(listOfNotes->Author), "Mysetery Novels");
	strcpy_s(&listOfNotes[0].theNote, sizeof(listOfNotes->theNote), "This is a note about mystery");

	strcpy_s(&listOfNotes[2].Author, sizeof(listOfNotes->Author), "J.K. Rowling");
	strcpy_s(&listOfNotes[2].topic, sizeof(listOfNotes->Author), "Fantasy");
	strcpy_s(&listOfNotes[2].theNote, sizeof(listOfNotes->theNote), "This is Harry Potter and wow!");

	strcpy_s(&listOfNotes[5].Author, sizeof(listOfNotes->Author), "APPLE & PAIR");
	strcpy_s(&listOfNotes[5].topic, sizeof(listOfNotes->Author), "FRUITS");
	strcpy_s(&listOfNotes[5].theNote, sizeof(listOfNotes->theNote), "THE TYPES OF FRUITS ARE APPLE & PEAR");

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