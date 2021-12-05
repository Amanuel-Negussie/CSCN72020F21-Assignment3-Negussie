//cscn72020f21 - simple networking library.  take 1
// steveh - nov 2021

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x6000
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <time.h>

#define DISPLAYBUFFERSIZE	100
#define SENDBUFFERSIZE	1000
#define MAXLISTENERS		10

//common
void InitializeWindowsSockets();

//server only
struct addrinfo* ConfigureLocalAddress();
SOCKET CreateBindListeningSocket(struct addrinfo*);
void StartListeningForConnections(SOCKET);
SOCKET WaitForAndAcceptConnection(SOCKET);
void RecvRequestAndSendResponse(SOCKET);

//client only
struct addrinfo* ConfigureRemoteAddress(char*, char*);
SOCKET CreateAndConnectRemoteSocket(struct addrinfo*);

void CloseSocketConnection(SOCKET);
void ShutdownWindowsSockets();