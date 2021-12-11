//cscn72020f21 - simple network client to attach to our simple time server
// steveh - nov 2021

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