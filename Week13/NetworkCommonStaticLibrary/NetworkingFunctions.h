#pragma once

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

#define _CRT_SECURE_NO_WARNINGS

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x6000
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define DISPLAYBUFFERSIZE	100
#define SENDBUFFERSIZE	1000
#define MAXLISTENERS		10
#define BASE_TEN 10
#define NOTES_TXT_FILE "Notes.txt"
#define NOTES_DAT_FILE "Notes.dat"
#define LOCAL_HOST_NAME "http://www.notes.com/api"
#define MAX_NOTES 30
#define AUTHOR_LENGTH 60
#define TOPIC_LENGTH 60
#define NOTE_LENGTH 2000
#define CPU_DESIRED_TIME 5.00


typedef enum proto { UDP, TCP } PROTOCOL;

struct NOTE {
	char Author[AUTHOR_LENGTH];
	char topic[TOPIC_LENGTH];
	char theNote[NOTE_LENGTH];
} typedef NOTE;

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



//common
void InitializeWindowsSockets();
void InitializeNote(NOTE*);  //passing an array of memset Notes and getting Notes
void CloseSocketConnection(SOCKET);
void ShutdownWindowsSockets();


bool convertJSONtoNote(NOTE* newNote, char* response); //convert JSON to Note
bool isUnderTime(double changeInTime, time_t start); //isUnderTime

//server only
struct addrinfo* ConfigureLocalAddress(char*, PROTOCOL);
SOCKET CreateBindListeningSocket(struct addrinfo*);
void StartListeningForConnections(SOCKET);
SOCKET WaitForAndAcceptConnection(SOCKET);

//client only
struct addrinfo* ConfigureRemoteAddress(char*, char*, PROTOCOL);
SOCKET CreateAndConnectRemoteSocket(struct addrinfo*);
