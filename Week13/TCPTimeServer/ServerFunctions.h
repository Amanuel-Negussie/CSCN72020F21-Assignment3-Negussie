#pragma once
#include "NetworkingFunctions.h"
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

//file saving i/o 
bool saveNoteListToFileDAT(NOTE*, char*);
bool readNoteListFromFileDAT(NOTE*, char*);

//synchronous multiplexed connections 
void WaitForAndAcceptAndHandleMultiplexedConnections(SOCKET, NOTE*);

//handleReadAPI 

void handleReadAPI(char*, char*, NOTE*);
bool requestLineParser(char*, enum REQUEST_TYPE*, char*, enum PROTOCOL_TYPE*, int*, char*, NOTE*);

//NOTES 
bool isNoteAvailable(NOTE* theNote);
void copyNotetoNote(NOTE* a, NOTE* b);  //copy Note to Note
bool getNote(int, NOTE*, NOTE*); //note
//Converting Note Messages to JSON or Friendly Reading Text
bool produceAllNoteMessageJSON(NOTE* theListOfNotes, char* theMessage);
bool produceNoteMessageJSON(NOTE*, char*, int);
bool produceAllNoteMessage(NOTE*, char*); //format
bool produceNoteMessage(NOTE*, char*);  //format

//Server Response Codes
bool produce200OKHeader(char*); //Successful
bool produce204NoContent(char*); //Successful Deletion
bool produce404Error(char*); //404 error
bool produce405Error(char*); //405 ERROR -->Method Not Allowed
bool produce400Error(char*); //400 Bad Request


