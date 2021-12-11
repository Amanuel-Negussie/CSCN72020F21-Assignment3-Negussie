#pragma once
#include "NetworkingFunctions.h"
#define SINGLE_PATH "/note/"
#define COLLECTION_PATH "/notes"


bool createGETSingleRequestMessage(char*, int);  //buffer passes through, index is the note index we need /note/6
bool createGETCollectionRequestMessage(char*); //no information needed same request at all times
bool createPOSTRequestMessage(char*, NOTE*, int); //create POST with required NOTE and index 
bool createPUTRequestMessage(char*, NOTE*, int); //create PUT REQUEST with required NOTE and index
bool createDELETERequestMessage(char*, int ); //create DELETE REQUEST MESSAGE with just index
bool ShowMenuAndReceiveResponse(int*); //menu 
bool ReceiveNoteResponse(NOTE*);
bool AskForNoteIndex(int*); 

//Input/Output Buffer flusher
void cFlusher(void);

