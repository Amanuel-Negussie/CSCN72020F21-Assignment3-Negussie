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

#include "NetworkingFunctions.h"
#include "ClientFunctions.h"
#include <stdio.h>


#define MAXBUFFER	2000

int main(void)
{
	// first set up Winsock  (same as server)
	InitializeWindowsSockets();

	printf("Config the remote addr...\n");
	struct addrinfo* peer_address = ConfigureRemoteAddress("127.0.0.1", "8080", TCP);

	printf("Creating socket and connect...\n");
	SOCKET peer_socket = CreateAndConnectRemoteSocket(peer_address);

	//provide menu 
	/*
	1. View A Note 
	2. View All Notes 
	3. Update An Existing Note 
	4. Post A New Note 
	5. Delete An Existing Note 
	q: q is for quit
	*/

	/*
	Ask User what they want to choose. 
	Receive input. 
	If 1) 
	ask them what note number 
	send request
	If 2) 
	send request
	If 3)
	ask them what note number
	ask them Author, Topic, and Note
	If 4) 
	ask them what note number, 
	ask them what Author, Topic and Note
	send request 
	If 5) 
	ask them what note number
	send request
	*/

	int response;

	int index;
	char message[MAXBUFFER] = { '\0' };
	while (1)
	{

		while (!ShowMenuAndReceiveResponse(&response));
		NOTE newNote;
		InitializeNote(&newNote);
		int index;
		switch (response)
		{
		case 1:  //View Note
			while (!AskForNoteIndex(&index));

			createGETSingleRequestMessage(&message, index);
			break;
		case 2:  //View All Notes
			createGETCollectionRequestMessage(&message);

			break;
		case 3: //Update An Existing Note
			while (!AskForNoteIndex(&index));
			if (!ReceiveNoteResponse(&newNote))  //take a valid Note if false means client wants to go back to Menu
				continue;
			createPUTRequestMessage(&message, &newNote, index);
			break;
		case 4:  //Post a New Note
			while (!AskForNoteIndex(&index));

			if (!ReceiveNoteResponse(&newNote))
				continue;
			createPOSTRequestMessage(&message, &newNote, index);
			break;
		case 5: //Delete an Existing Note
			while (!AskForNoteIndex(&index));
			createDELETERequestMessage(&message, index);
			break;
		case 6:
			printf("Thank you for using NOTELY API.COM");
			exit(0);
			break;
		}




		//send seply to server
		if (send(peer_socket, message, (int)strlen(message), 0) == 0)
		{
			fprintf(stderr, "send failed\n");
		}

		//receive reply from server
		char buffer[MAXBUFFER] = { '\0' };


		if (recv(peer_socket, buffer, MAXBUFFER, 0) == 0)
		{
			fprintf(stderr, "unable to receive from server");
		}

		printf("%s", buffer);
		memset(&message, '\0', MAXBUFFER);
	}

	printf("Closing the connection...\n");
	CloseSocketConnection(peer_socket);
	ShutdownWindowsSockets();
}