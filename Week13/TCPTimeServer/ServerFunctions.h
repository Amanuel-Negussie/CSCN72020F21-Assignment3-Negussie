#pragma once
#include "NetworkingFunctions.h"


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

//Server Response Codes
bool produce200OKHeader(char*); //Successful
bool produce204NoContent(char*); //Successful Deletion
bool produce404Error(char*); //404 error
bool produce405Error(char*); //405 ERROR -->Method Not Allowed
bool produce400Error(char*); //400 Bad Request


