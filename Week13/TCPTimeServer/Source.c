/*
CSCN72020-Assignment3-Amanuel Negussie Due Date: December 10th, 2021
Utilized Professor Steve Hendrikse's existing library as a starting point https://github.com/ProfessorSteveH/CSCN72020F21
Incorporated Multiplexing through utilizing video from class
Utilized the Textbook Hands On Network Programming with C' by Lewis Van Winkle
The goal of the Client:
Was to be able to have a console mode program that was menu driven allowing the user to exercise all the methods on the server
GET, GET COLLECTION, PUT, POST, AND DELETE
The goal of the Server:
Have two URIs which I called /note/? for GET, PUT, POST, AND DELETE and second URI I called /notes for GET COLLECTION
Added Additional attributes on the NOTE -->Author, Topic, And Note for readable text as well
Server is loaded from disk on startup, constantly updated through any put, post and delete successful methods
so that in the event of server shutting down unexpectedly all data will be saved


Advanced features:
- Added synchronous multiplexing on server
- Completed Response code and response parsing as well as 'friendly printing'

Areas of Improvement:
- Work on GET method filtering
- Add Authentication
- Work on GUI implementation

client can work on WSL directly connecting to netcat 127.0.0.1 port 8080 and using request headers with request body in the form of JSON
client can also work through console in a friendly manner
*/

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetworkingFunctions.h"
#include "ServerFunctions.h"

#define LISTENINGPORT   "8080"      //needs to be a string

int main(void)
{
	InitializeWindowsSockets();
	NOTE listOfNotes[MAX_NOTES], * listP;
	listP = listOfNotes;
	for (int i =0; i<MAX_NOTES; i++)
	InitializeNote(listP++);

	listP = &listOfNotes;
	readNoteListFromFileDAT(listP, NOTES_DAT_FILE);
	

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