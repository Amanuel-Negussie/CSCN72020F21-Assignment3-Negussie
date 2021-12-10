#pragma once
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
#define MAX_NOTES 30
#define AUTHOR_LENGTH 60
#define TOPIC_LENGTH 60
#define NOTE_LENGTH 2000
#define CPU_DESIRED_TIME 1.00

typedef enum proto { UDP, TCP } PROTOCOL;

struct NOTE {
	char Author[AUTHOR_LENGTH];
	char topic[TOPIC_LENGTH];
	char theNote[NOTE_LENGTH];
} typedef NOTE;

//common
void InitializeWindowsSockets();
void InitializeNotea(NOTE*);  //passing an array of memset Notes and getting Notes
bool createNote(NOTE*); //make sure that you can create NOTE
bool isNoteAvailable(NOTE* theNote);
void copyNotetoNote(NOTE* a, NOTE* b);  //copy Note to Note
void CloseSocketConnection(SOCKET);
void ShutdownWindowsSockets();

bool getNote(int , NOTE* , NOTE* ); //note
bool produceAllNoteMessage(NOTE*, char*); //format
bool produceNoteMessage(NOTE*, char*);  //format
bool convertJSONtoNote(NOTE* newNote, char* response); //convert JSON to Note
bool isUnderTime(double changeInTime, time_t start); //isUnderTime
bool produce200OKHeader(char*);
bool produce204NoContent(char*);
bool produce404Error(char*); //404 error
bool produce405Error(char*); //405 ERROR -->Method Not Allowed
bool produce400Error(char*); //400 Bad Request

//server only
struct addrinfo* ConfigureLocalAddress(char*, PROTOCOL);
SOCKET CreateBindListeningSocket(struct addrinfo*);
void StartListeningForConnections(SOCKET);
SOCKET WaitForAndAcceptConnection(SOCKET);
void WaitForAndAcceptAndHandleMultiplexedConnections(SOCKET, NOTE*);
void createPayload(char*);
void RecvRequestAndSendResponse(SOCKET);
void RecvUDPRequestAndSendResponse(SOCKET);
void handleReadAPI(char*, char*, NOTE*);
bool requestLineParser(char*, enum REQUEST_TYPE*, char*, enum PROTOCOL_TYPE*, int*, char*, NOTE*);

//client only
struct addrinfo* ConfigureRemoteAddress(char*, char*, PROTOCOL);
SOCKET CreateAndConnectRemoteSocket(struct addrinfo*);
