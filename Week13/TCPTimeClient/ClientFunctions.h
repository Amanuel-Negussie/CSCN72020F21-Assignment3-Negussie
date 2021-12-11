#pragma once
#include "NetworkingFunctions.h"
#define SINGLE_PATH "/note/"
#define COLLECTION_PATH "/notes"

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

//User Interface
bool ShowMenuAndReceiveResponse(int*); //menu 
bool ReceiveNoteResponse(NOTE*);
bool AskForNoteIndex(int*);

//Input/Output Buffer flusher
void cFlusher(void);

//All Client Methods for Accessing Data
bool createGETSingleRequestMessage(char*, int);  //buffer passes through, index is the note index we need /note/6
bool createGETCollectionRequestMessage(char*); //no information needed same request at all times
bool createPOSTRequestMessage(char*, NOTE*, int); //create POST with required NOTE and index 
bool createPUTRequestMessage(char*, NOTE*, int); //create PUT REQUEST with required NOTE and index
bool createDELETERequestMessage(char*, int ); //create DELETE REQUEST MESSAGE with just index




