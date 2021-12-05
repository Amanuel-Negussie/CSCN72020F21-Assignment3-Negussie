//cscn72020 - week12 - a simple example of a networked service.
//steveh - nov 2021

// to create a library for use in multiple projects, follow:
//
//1. Create new empty project
//-- > Properties
//------ > General
//---------- > change Configuration Type to static library
//
//2. copy/create source files in new project
//
//3. Main Project(existing) that needs to use library :
//----->References
//--------->add new reference.select Library project
//
//4. Server project properties
//-- > C / C++
//------->General
//----------->Additional Include Directories, add 
//							$(SolutionDir)NetworkCommonStaticLibrary
//								* no \ after macro!

#include "NetworkingFunctions.h"
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

void InitializeWindowsSockets()
{
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "failed to initialize network.  Exiting\n");
		exit(1);
	}
}

struct addrinfo* configureAddress(char* host, char* port, PROTOCOL protocol)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;                  // gonna be v4
	if (protocol == TCP)
	{
		hints.ai_socktype = SOCK_STREAM;    // gonna be tcp
		hints.ai_protocol = IPPROTO_TCP;
	}
	else if (protocol == UDP)
	{
		hints.ai_socktype = SOCK_DGRAM;		// gonna be udp
		hints.ai_protocol = IPPROTO_UDP;
	}
	else  //unknown   exit!
	{
		fprintf(stderr, "Unknown protocol selected. Exiting\n");
		exit(EXIT_FAILURE);
	}
	hints.ai_flags = AI_PASSIVE;                    //bind to * address

	struct addrinfo* bind_address;  //no malloc required here.  see note below
	if(!strncmp(host, "*", 1))	// 0 if same!
		getaddrinfo(0, port, &hints, &bind_address);
	else
		getaddrinfo(host, port, &hints, &bind_address);
	//
	// from: https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
	// All information returned by the getaddrinfo function pointed to by the ppResult 
	// parameter is dynamically allocated, including all addrinfo structures, socket address 
	// structures, and canonical host name strings pointed to by addrinfo structures. Memory
	// allocated by a successful call to this function must be released with a subsequent call
	// to freeaddrinfo.
	return bind_address;
}


struct addrinfo* ConfigureLocalAddress(char* port, PROTOCOL protocol)
{
	return configureAddress("*", port, protocol);
}

struct addrinfo* ConfigureRemoteAddress(char* remoteHost, char* remotePort, PROTOCOL protocol)
{
	return configureAddress(remoteHost, remotePort, protocol);
}

SOCKET CreateBindListeningSocket(struct addrinfo* bind_address)
{
	SOCKET socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, 
																									bind_address->ai_protocol);
	if ((socket_listen) == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed, exiting with error (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);  
	}

	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed, exiting with error  (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(bind_address);  //see note above.  this frees all the allocated mem.
	return socket_listen;
}

SOCKET CreateAndConnectRemoteSocket(struct addrinfo* peer_address)
{
	SOCKET socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
	if ((socket_peer) == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed, exiting with error (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed, exiting with error (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(peer_address);

	return socket_peer;
}

void StartListeningForConnections(SOCKET socket_listen)
{
	if (listen(socket_listen, MAXLISTENERS) < 0)      //allowed to queue up to 10 connections
	{
		fprintf(stderr, "listen() failed, exiting with and error of (%d)\n", WSAGetLastError());
		exit(1);
	}
}


SOCKET WaitForAndAcceptConnection(SOCKET socket_listen)
{
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	SOCKET socket_client = accept(socket_listen, (struct sockaddr*)&client_address, &client_len);
	if ((socket_client) == INVALID_SOCKET)
	{
		fprintf(stderr, "accept() failed. (%d)\n", WSAGetLastError());
		return 1;
	}
	
	//opportunity to clean this up!
	char address_buffer[DISPLAYBUFFERSIZE];
	getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, 
																							sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
	printf("%s\n", address_buffer);

	return socket_client;
}


void WaitForAndAcceptAndHandleMultiplexedConnections(SOCKET socket_listen)
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
					int bytes_received = recv(i, read, SENDBUFFERSIZE, 0);
					if (bytes_received < 1)  //in the event the client disappears during this time
					{
						FD_CLR(i, &master);
						CloseSocketConnection(i);
						continue;
					}






					printf("received message (size: %d bytes) from %I64d\n", bytes_received, i);
					char buffer[SENDBUFFERSIZE];



					handleReadAPI(read);

			
					createPayload(buffer);
					int bytes_sent = send(i, buffer, (int)strlen(buffer), 0);
					printf("sent message (size: %d bytes) to %I64d\n", bytes_sent, i);
				}
			}
		}

	}
}

enum PROTOCOL_TYPE {
	HTTP_ONE_POINT_ONE,
	HTTP_TWO
}typedef PROTOCOL_TYPE;

enum REQUEST_TYPE {
	GET, 
	POST,
	PUT, 
	DELETE_IT,
}typedef REQUEST_TYPE;

bool requestLineParser(char* response, REQUEST_TYPE* rT, char* dP, PROTOCOL_TYPE* pV, int* index, char* query)
{
	char requestTypeString [BUFSIZ], * rP; 
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
			*Buffer= *response; 
			if (*Buffer == '/')
			{
				for (int i = 0; i < strlen(dpBuffer); i++)
				{
					*dP = dpBuffer[i];  //go through Buffer and iterate through pointer
					dP++;
				}
		
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
		if ((*Buffer)!=NULL)
		num = strtol(Buffer, endPointer, BASE_TEN);
		if (*Buffer == '?') //query 
		{
			for (int i = 0; i < strlen(Buffer); i++)
			{
				*query++ = *(Buffer + i);
			}
			*query = '\0';
		}
		else if (num!= 0 && endPointer==Buffer+strlen(Buffer)) //index
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
		while (*response != '\n')
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
		else if (memcmp(pP, "HTTP/2", sizeof(pP)) == 0)
			*pV = HTTP_TWO;
		else
			return false;


	
	
	return true;
}
bool handleReadAPI(char* info)
{
	//read read and choose which function to take 

	//he request type, the document path, and the protocol version
	REQUEST_TYPE requestType; 
	char documentPath[BUFSIZ]; 
	char* dP = documentPath;
	memset(documentPath, '\0', BUFSIZ * (sizeof(char)));
	PROTOCOL_TYPE protocolVersion;
	int index = 0;
	char query[BUFSIZ], q; //extra bonus: query
	q = query;


	// requestLineParser: error checking here for request Type as well as protocolVersion
	if (!requestLineParser(info, &requestType, dP, &protocolVersion, &index, q))
		return false;
	//parse documentPath 
	//notes.htm then get all of the notes 
	//notes.htm/1 then you get note 1 
	//notes.htm/2 then you get note 2 
	//----> how many numbers 
	// notes.htm/
	//folder //



	if (protocolVersion == HTTP_ONE_POINT_ONE)
	{

		//PROTOCOL VERSION HTTP 1.1 

		//document path is the web page we're going to/ more importantly it's the type of data we want 
		//requestType is either Get Get All Collections Set Put Delete 

		if (memcmp(documentPath, "/notes", sizeof("/notes")) == 0)
		{
			switch (requestType)
			{
			case GET:
				if (index > 0)  //GET ONE
				{

				}
				else   //GET ALL 
				{

				}
				break;
			case POST: 
				break;
			case PUT: 
				break; 
			case DELETE_IT:
				break;
			}
		}
		else if (memcmp(documentPath, "/times", BUFSIZ) == 0)
		{
			
		}
		else
		{
			//if message is GET time .... 
			char buffer[SENDBUFFERSIZE];
			createPayload(buffer);
		}



	}
	else if (protocolVersion == HTTP_TWO)
	{
		printf("I'm sorry but we don't have HTTP 2 yet.");
		return false;
	}
	else
		return false;

	


	

	//if message is GET time .... 
	char buffer[SENDBUFFERSIZE];
	createPayload(buffer);
}


void createPayload(char* buffer)
{
	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n"
		"Local time is: ";

	time_t timer;
	time(&timer);
	char* time_msg = ctime(&timer);

	sprintf(buffer, "%s %s\n\0\0", response, time_msg);
}

void RecvRequestAndSendResponse(SOCKET socket_client)
{
	printf("Reading request...\n");
	char request[SENDBUFFERSIZE];
	int bytes_received = recv(socket_client, request, SENDBUFFERSIZE, 0);
	printf("Received %d bytes: \n", bytes_received);
	printf("%.*s\n", bytes_received, request);      //the %.*s makes sure we print exactly as many chars as was received (regardless of null termination)

	printf("building response\n");
	char buffer[SENDBUFFERSIZE];
	createPayload(buffer);

	printf("Sending response...\n");
	int bytes_sent = send(socket_client, buffer, strlen(buffer), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(buffer));
}

void RecvUDPRequestAndSendResponse(SOCKET listen_socket)
{
	struct sockaddr_in cliaddr;
	char buffer[SENDBUFFERSIZE];

	int clientLength = sizeof(cliaddr);  //len is value/resuslt

	int bytesReceived = recvfrom(listen_socket, 
														buffer, 
														SENDBUFFERSIZE,
														0, 
														(struct sockaddr*)&cliaddr,
														&clientLength);
	buffer[bytesReceived] = '\0';	// no guarantee the payload will be NULL terminated
	printf("\nClient sent : %s\n", buffer);

	createPayload(buffer);

	sendto(listen_socket, 
					buffer, 
					strlen(buffer),
					0, 
					(const struct sockaddr*)&cliaddr,
					clientLength);
	printf("Response message sent.\n");
}

void CloseSocketConnection(SOCKET this_socket)
{
	shutdown(this_socket, SD_BOTH);
	closesocket(this_socket);
}

void ShutdownWindowsSockets()
{
	WSACleanup();
}
