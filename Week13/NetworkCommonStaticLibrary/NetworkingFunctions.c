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
	if (!strncmp(host, "*", 1))	// 0 if same!
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


void InitializeNote(NOTE* theListOfNotes)
{
	memset(theListOfNotes->Author, NULL, sizeof(theListOfNotes->Author));
	memset(theListOfNotes->topic, NULL, sizeof(theListOfNotes->topic));
	memset(theListOfNotes->theNote, NULL, sizeof(theListOfNotes->theNote));
}

bool convertJSONtoNote(NOTE* newNote, char* response)
{
	clock_t start; //clock time adapted from https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/ //
	start = clock();
	int count = 0;
	while (*response != ':' && count < SENDBUFFERSIZE)
	{
		response++;
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != '"' && count < SENDBUFFERSIZE)
	{
		response++;
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != '"' && count < SENDBUFFERSIZE)
	{
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		strncat(newNote->Author, response++, 1);
		count++;
	}
	response++;
	start = clock();
	while (*response != ':' && count < SENDBUFFERSIZE)
	{
		response++;
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != '"' && count < SENDBUFFERSIZE)
	{
		response++;
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != '"' && count < SENDBUFFERSIZE)
	{
		strncat(newNote->topic, response++, 1);
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != ':' && count < SENDBUFFERSIZE)
	{
		response++;
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != '"' && count < SENDBUFFERSIZE)
	{
		response++;
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != '"' && count < SENDBUFFERSIZE)
	{
		strncat(newNote->theNote, response++, 1);
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
	}
	response++;
	start = clock();
	while (*response != '}' && count < SENDBUFFERSIZE)
	{
		if (!isUnderTime(CPU_DESIRED_TIME, start))
			return false;
		count++;
		response++;
	}
	return (*response == '}');
}

//isUnderTime Allows us to make sure that while using a while loop we're not forever waiting
bool isUnderTime(double changeInTime, time_t start)
{
	time_t end = clock();
	double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	if (cpu_time_used > changeInTime)
		return false;
	return true;
}

//Closing Sockets and Cleanup

void CloseSocketConnection(SOCKET this_socket)
{
	shutdown(this_socket, SD_BOTH);
	closesocket(this_socket);
}

void ShutdownWindowsSockets()
{
	WSACleanup();
}