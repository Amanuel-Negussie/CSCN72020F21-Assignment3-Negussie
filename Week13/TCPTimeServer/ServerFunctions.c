#include "ServerFunctions.h"


//FILE I/O 
bool saveNoteListToFileDAT(NOTE* listP, char* FileName)
{
	FILE* write_ptr;

	write_ptr = fopen(FileName, "wb");

	if (fwrite(listP, sizeof(struct NOTE), MAX_NOTES, write_ptr) != 0)
	{
		fclose(write_ptr);
		return true;
	}
	else {
		fclose(write_ptr);
		return false;
	}
}

bool readNoteListFromFileDAT(NOTE* listP, char* FileName)
{
	FILE* read_ptr;
	read_ptr = fopen(FileName, "rb");
	if (fread(listP, sizeof(struct NOTE), MAX_NOTES, read_ptr) != 0)
	{
		fclose(read_ptr);
		return true;
	}
	else
	{
		fclose(read_ptr);
		return false;
	}
}
//Synchronous Multiplexed Connections 

void WaitForAndAcceptAndHandleMultiplexedConnections(SOCKET socket_listen, NOTE* listP)
{
	fd_set master;			//create new FD_SET
	FD_ZERO(&master);		// initialize it all to 0
	FD_SET(socket_listen, &master); //add our (existing) listener socket to FD_SET
	SOCKET max_socket = socket_listen;	//and this will be largest socket value
	SOCKET min_socket = max_socket; //this is our min_socket

	while (1)
	{
		fd_set reads;
		reads = master;
		if (select((int)(max_socket + 1), &reads, 0, 0, 0) < 0)
		{
			fprintf(stderr, "select() failed exiting with an error of (%d)\n", WSAGetLastError());
			exit(1);
		}
		SOCKET i;
		for (i = min_socket; i <= max_socket; ++i)
		{
			if (FD_ISSET(i, &reads))
			{
				if (i == socket_listen) //handles a new socket request with an accept
				{
					struct sockaddr_storage client_address;
					socklen_t client_len = sizeof(client_address);
					SOCKET socket_client = accept(socket_listen, (struct sockaddr*)&client_address, &client_len);
					if ((socket_client) == INVALID_SOCKET)
					{
						fprintf(stderr, "accept() failed. (%d)\n", WSAGetLastError());
						exit(1);
					}

					FD_SET(socket_client, &master);
					if (socket_client > max_socket)  //update maximum range in set
						max_socket = socket_client;
					if (socket_client < min_socket)  //update minimum range in set
						min_socket = socket_client;

					//Displaying new connection
					char address_buffer[DISPLAYBUFFERSIZE];
					getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer,
						sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
					printf("new connection from %s\n", address_buffer);
				}
				else
				{
					char read[SENDBUFFERSIZE];
					memset(&read, '\0', SENDBUFFERSIZE);
					int bytes_received = recv(i, read, SENDBUFFERSIZE, 0);
					if (bytes_received < 1)  //in the event the client disappears during this time
					{
						FD_CLR(i, &master);
						CloseSocketConnection(i);
						continue;
					}

					printf("received message (size: %d bytes) from %I64d\n", bytes_received, i);
					char buffer[SENDBUFFERSIZE];
					memset(&buffer, '\0', SENDBUFFERSIZE);

					handleReadAPI(&read, &buffer, listP);

					//createPayload(buffer);

					int bytes_sent = send(i, &buffer, (int)strlen(buffer), 0);
					memset(&buffer, '\0', SENDBUFFERSIZE);
					printf("sent message (size: %d bytes) to %I64d\n", bytes_sent, i);
				}
			}
		}
	}
}




//handleReadAPI 

void handleReadAPI(char* info, char* buffer, NOTE* listOfNotes)
{
	//read read and choose which function to take

	//the request type, the document path, and the protocol version
	REQUEST_TYPE requestType;
	char documentPath[BUFSIZ];
	char* dP = documentPath;
	memset(documentPath, '\0', BUFSIZ * (sizeof(char)));
	PROTOCOL_TYPE protocolVersion;
	int index = 0;
	char query[BUFSIZ], q; //extra bonus: query
	q = query;
	NOTE newNote, * n;
	n = &newNote;
	InitializeNote(n);
	// requestLineParser: error checking here for request Type as well as protocolVersion
	if (!requestLineParser(info, &requestType, dP, &protocolVersion, &index, q, n))
	{
		produce400Error(buffer);
		return; //communicate to client that there is an incorrect -
	}

	//parse message

	if (protocolVersion == HTTP_ONE_POINT_ONE)
	{
		if (memcmp(documentPath, "/note", sizeof("/note")) == 0 || memcmp(documentPath, "/note/", sizeof("/note/")) == 0)
		{
			//NOTE* theNote = (NOTE*) malloc(sizeof(NOTE));
			NOTE theNote;
			InitializeNote(&theNote);

			switch (requestType)
			{
			case GET:
				if (index > 0)  //GET ONE
				{
					//accessing the data by index value

					if (!getNote(index, listOfNotes, &theNote))
					{
						//AError
						produce404Error(buffer);   //Page Not Available

						//free(theNote);
						return; //404 error
					}
					else
					{
						produce200OKHeader(buffer);
						produceNoteMessage(&theNote, buffer);
						produceNoteMessageJSON(&theNote, buffer, index);
						//free(theNote);
						return;
					}
				}
				else   //GET ALL
				{
					produce404Error(buffer);  //Page Not Available
					return;
					//produce404Error(buffer)
					//return;
				}
				break;
			case POST:
				if (index > 0)  //GET ONE
				{
					//accessing the data by index value

					if (!isNoteAvailable(listOfNotes + index - 1)) //if there is No Note you can Post.
					{
						produce200OKHeader(buffer);
						copyNotetoNote(listOfNotes + index - 1, &newNote);
						saveNoteListToFileDAT(listOfNotes, NOTES_DAT_FILE);
						produceNoteMessage(&newNote, buffer);
						return;
					}
					else
					{
						produce405Error(buffer);   //Method Not Available
						return;
					}
				}
				else   //GET ALL
				{
					produce405Error(buffer);  //Page Not Available
					return;
				}
				break;
			case PUT:
				if (index > 0)  //GET ONE
				{
					//accessing the data by index value

					if (isNoteAvailable(listOfNotes + index - 1)) //if there is No Note you can Post.
					{
						produce200OKHeader(buffer);
						copyNotetoNote(listOfNotes + index - 1, &newNote);
						saveNoteListToFileDAT(listOfNotes, NOTES_DAT_FILE);

						produceNoteMessage(&newNote, buffer);

						return; //404 error
					}
					else
					{
						produce405Error(buffer);   //Method Not Available
						return;
					}
				}
				else   //GET ALL
				{
					produce405Error(buffer);  //Page Not Available

					return;
					//produce404Error(buffer)
					//return;
				}
				break;
			case DELETE_IT:
				if (index > 0)  //GET ONE
				{
					//accessing the data by index value

					if (isNoteAvailable(listOfNotes + index - 1)) //if there is No Note you can Post.
					{
						produce204NoContent(buffer);
						InitializeNote(listOfNotes + index - 1);
						saveNoteListToFileDAT(listOfNotes, NOTES_DAT_FILE);
						return; //404 error
					}
					else
					{
						produce405Error(buffer);   //Method Not Available
						return;
					}
				}
				else   //GET ALL
				{
					produce405Error(buffer);  //Page Not Available

					return;
				}
				break;
			}
		}
		else if (memcmp(documentPath, "/notes", strlen(documentPath)) == 0 || memcmp(documentPath, "/notes/", strlen(documentPath)) == 0)
		{
			NOTE theNote;
			switch (requestType)
			{
			case GET:
				if (index > 0)  //GET ONE
				{
					produce405Error(buffer);
					return;
				}
				else   //GET ALL
				{
					produce200OKHeader(buffer);
					produceAllNoteMessage(listOfNotes, buffer);
					produceAllNoteMessageJSON(listOfNotes, buffer);
					return;

					//code for get ALL
				}
				break;
			default:
				produce405Error(buffer);
				return;
				break;
			}
		}
		else
		{
			produce405Error(buffer);
			return;
		}
	}
	else if (protocolVersion == HTTP_TWO)
	{
		produce405Error(buffer); //coming soon
		return;
	}
	else
	{
		produce404Error(buffer);
		return;
	}
}


//requestLineParser
bool requestLineParser(char* response, REQUEST_TYPE* rT, char* dP, PROTOCOL_TYPE* pV, int* index, char* query, NOTE* newNote)
{
	char requestTypeString[BUFSIZ], * rP;
	rP = requestTypeString;
	char protocolTypeString[BUFSIZ], * pP;
	pP = protocolTypeString;
	char dpBuffer[BUFSIZ], * Buffer;
	memset(dpBuffer, '\0', BUFSIZ * sizeof(char));
	Buffer = dpBuffer;

	while (*response != ' ')
		*rP++ = *response++;
	*rP = '\0';
	response++;
	while (*response != ' ')
	{
		*Buffer = *response;
		if (*Buffer == '/')
		{
			for (int i = 0; i < strlen(dpBuffer); i++)
			{
				*dP++ = dpBuffer[i];  //go through Buffer and iterate through pointer
			}
			//Buffer = &dpBuffer ;
			memset(dpBuffer, '\0', BUFSIZ * sizeof(char));
			Buffer = &dpBuffer;
		}
		else
		{
			Buffer++;  //increase buffer
		}
		response++;
	}
	//check if buffer if buffer is a query or if its an index ? If not then add buffer to document path
	Buffer = &dpBuffer;
	char* endPointer = NULL;
	int num = 0;
	if ((*Buffer) != NULL)
		num = strtol(Buffer, &endPointer, BASE_TEN);
	if (*Buffer == '?') //query
	{
		for (int i = 0; i < strlen(Buffer); i++)
		{
			*query++ = *(Buffer + i);
		}
		*query = '\0';
	}
	else if (num > 0 && endPointer == Buffer + strlen(Buffer)) //index
	{
		*index = num;
	}
	else
	{
		for (int i = 0; i < strlen(Buffer); i++)
		{
			*dP++ = *(Buffer + i);
		}
	}

	//end dP correctly
	*dP = '\0';
	response++;
	while (*response != '{' && *response != '\0')
		*pP++ = *response++;
	*pP = '\0';

	//parsing through REQUEST TYPES
	if (memcmp(requestTypeString, "GET", sizeof("GET")) == 0)
		*rT = GET;
	else if (memcmp(requestTypeString, "POST", sizeof("POST")) == 0)
		*rT = POST;
	else if (memcmp(requestTypeString, "PUT", sizeof("PUT")) == 0)
		*rT = PUT;
	else if (memcmp(requestTypeString, "DELETE", sizeof("DELETE")) == 0)
		*rT = DELETE_IT;
	else
		return false;
	pP = protocolTypeString;
	if (memcmp(pP, "HTTP/1.1", sizeof(pP)) == 0)
		*pV = HTTP_ONE_POINT_ONE;
	else if (memcmp(pP, "HTTP/2\0", sizeof(pP)) == 0)
		*pV = HTTP_TWO;
	else
		return false;

	if (*rT == POST || *rT == PUT)
	{
		if (!convertJSONtoNote(newNote, response))
			return false;
	}

	return true;
}

//Server Response 

//Complete Response Code
// Method not Available
bool produce405Error(char* buffer)
{
	const char* response =
		"HTTP/1.1 405 Method Not Allowed\r\n"
		"Connection: open\r\n\r\n";
	return (sprintf(buffer + strlen(buffer), "%s\n\0\0", response) >= 0);
}

//Page Request
bool produce404Error(char* buffer)
{
	const char* response =
		"HTTP/1.1 404 Page Not Found\r\n"
		"Connection: open\r\n\r\n";
	return (sprintf(buffer + strlen(buffer), "%s\n\0\0", response) >= 0);
}

//Bad Request
bool produce400Error(char* buffer)
{
	const char* response =
		"HTTP/1.1 400 Bad Request\r\n"
		"Connection: open\r\n\r\n";
	return (sprintf(buffer + strlen(buffer), "%s\n\0\0", response) >= 0);
}


bool produce204NoContent(char* buffer)
{
	const char* response =
		"HTTP/1.1 204 No Content\r\n"
		"Connection: open\r\n"
		"Content-Type: text/plain and JSON\r\n\r\n";
	return (sprintf(buffer + strlen(buffer), "%s\n\0\0", response) >= 0);
}







//Note Methods --> isNoteAvailable, copyNotetoNote, getNote and producing Note Messages both in plain text and JSON 

bool isNoteAvailable(NOTE* theNote)
{
	//NOTE* emptyNote = (NOTE*)malloc(sizeof(NOTE));
	NOTE emptyNote;
	InitializeNote(&emptyNote);
	if (memcmp(theNote, &emptyNote, sizeof(NOTE)) == 0)
		return false;
	else
		return true;
}

void copyNotetoNote(NOTE* a, NOTE* b)
{
	if (memcmp(a, b, sizeof(NOTE)) == 0)
		return;
	strcpy_s(a->Author, sizeof(a->Author), b->Author);
	strcpy_s(a->topic, sizeof(a->topic), b->topic);
	strcpy_s(a->theNote, sizeof(a->theNote), b->theNote);
}

bool getNote(int index, NOTE* theListOfNotes, NOTE* theNote)//note
{
	if (index > MAX_NOTES)
		return false;
	copyNotetoNote(theNote, theListOfNotes + index - 1);

	return isNoteAvailable(theNote);  //if note is null then I cannot get Note //if note is note null then I can
}

bool produceNoteMessage(NOTE* theNote, char* theMessage)  //format
{
	time_t currentTime;
	currentTime = time(NULL);
	return (sprintf(theMessage + strlen(theMessage), "Author:\t%s\r\nTopic:\t%s\r\nMessage:%5s\r\nTime:%8s\r\n\0\0", theNote->Author, &theNote->topic, &theNote->theNote, ctime(&currentTime)) >= 0);
}

bool produceNoteMessageJSON(NOTE* theNote, char* theMessage, int i)  //format
{
	return (sprintf(theMessage + strlen(theMessage), "{\r\n\id:\t%d,\r\nAuthor:\t\"%s\",\r\nTopic:\t\"%s\",\r\nNote:\t\"%s\"\r\n},\r\n", i,
		theNote->Author, theNote->topic, theNote->theNote) < 0);
}

bool produceAllNoteMessage(NOTE* theListOfNotes, char* theMessage) //format
{
	time_t currentTime;

	for (int i = 0; i < MAX_NOTES; i++)
	{
		if (isNoteAvailable(theListOfNotes + i))
			if (sprintf(theMessage + strlen(theMessage), "Note #%i\nAuthor:\t%s\r\nTopic:\t%s\r\nMessage:%15s\n\n", i + 1, (theListOfNotes + i)->Author, (theListOfNotes + i)->topic, (theListOfNotes + i)->theNote) < 0)
			{
				fprintf(stderr, "Unable to add to the buffer.");
				exit(1);
			}
	}

	currentTime = time(NULL);
	return (sprintf(theMessage + strlen(theMessage), "Time:%9s\r\n\r\n\0\0", ctime(&currentTime)) >= 0);
}

bool produceAllNoteMessageJSON(NOTE* theListOfNotes, char* theMessage)
{
	time_t currentTime;
	if (sprintf(theMessage + strlen(theMessage), "{\r\n\Notes\: [\r\n") < 0)
		return false;
	for (int i = 0; i < MAX_NOTES; i++)
	{
		if (isNoteAvailable(theListOfNotes + i))
			if (sprintf(theMessage + strlen(theMessage), "{\r\n\id:\t%d,\r\nAuthor:\t\"%s\",\r\nTopic:\t\"%s\",\r\nNote:\t\"%s\"\r\n},\r\n", i + 1, (theListOfNotes + i)->Author, (theListOfNotes + i)->topic, (theListOfNotes + i)->theNote) < 0)
				return false;
	}
	if (sprintf(theMessage + strlen(theMessage) - 1, "\r\n]\r\n}\r\n") < 0)
		return false;
}