#include "ClientFunctions.h"

//MENU 

bool ShowMenuAndReceiveResponse(int* choice)
{
	printf("\n--------------------------------------\n");
	printf("\tWWW.NOTELY.COM/API\n\n\tMENU OPTIONS\n");
	printf("--------------------------------------\n");
	printf("1. View A Note \n");
	printf("2. View All Notes\n");
	printf("3. Update An Existing Note\n");
	printf("4. Post a New Note\n");
	printf("5. Delete An Existing Note\n");
	printf("6. Exit\n\n\n");
	printf("Enter your choice :  ");
	if (scanf("%i", choice) == EOF)
	{
		printf("\nPlease adhere to the instruction. Enter a valid number.");
		return false;
	}
	else
	{
		printf("\n--------------------------------------\n");
		return true;
	}
}

bool AskForNoteIndex(int* index)
{
	printf("Please Enter the Page Number:  ");
	if (scanf("%i", index) == EOF)
	{
		printf("\nPlease adhere to the instruction. Enter a valid number.");
		return false;
	}
	else
	{
		printf("\n--------------------------------------\n");
		return true;
	}
}


void cFlusher(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

bool ReceiveNoteResponse(NOTE* theNote)
{
	cFlusher();
	printf("Please Enter Author's Name:   ");
	scanf_s("%[^\n]%*c", theNote->Author, (unsigned int)sizeof(theNote->Author));
	printf("Please Enter the Topic:   ");
	scanf_s("%[^\n]%*c", theNote->topic, (unsigned int) sizeof(theNote->topic));
	printf("Please Enter the Note:   ");
	scanf_s("%[^\n]%*c", theNote->theNote, (unsigned int)sizeof(theNote->theNote));
	/*fgets(theNote->topic, TOPIC_LENGTH, stdin);
	fgets(theNote->theNote, NOTE_LENGTH, stdin);*/
	//scanf_s("%s", theNote->Author, sizeof(theNote->Author));
}

//Creating REQUESTS

bool createGETSingleRequestMessage(char* buffer, int index)  //buffer passes through, index is the note index we need /note/6
{
	if (index <= 0)
		return false;
	return (sprintf(buffer + strlen(buffer), "GET %s%d HTTP/1.1\r\n\r\n",SINGLE_PATH, index) >= 0);

}
bool createGETCollectionRequestMessage(char* buffer) //no information needed same request at all times
{
	return (sprintf(buffer + strlen(buffer), "GET %s HTTP/1.1\r\n\r\n", COLLECTION_PATH) >= 0);
}
bool createPOSTRequestMessage(char* buffer, NOTE* np, int index) //create POST with required NOTE and index 
{
	if (index <= 0)
		return false;
	return (sprintf(buffer + strlen(buffer), "POST %s%d HTTP/1.1\r\n\r\n{Author: \"%s\",Topic: \"%s\",Note: \"%s\"}\r\n\r\n",SINGLE_PATH, index, np->Author, np->topic, np->theNote) >= 0);
}
bool createPUTRequestMessage(char* buffer, NOTE* np, int index) //create PUT REQUEST with required NOTE and index
{
	if (index <= 0)
		return false;
	return (sprintf(buffer + strlen(buffer), "PUT %s%d HTTP/1.1\r\n\r\n{Author: \"%s\",Topic: \"%s\",Note: \"%s\"}\r\n\r\n", SINGLE_PATH, index, np->Author, np->topic, np->theNote) >= 0);
}
bool createDELETERequestMessage(char* buffer, int index) //create DELETE REQUEST MESSAGE with just index
{
	if (index <= 0)
		return false;
	return (sprintf(buffer + strlen(buffer), "DELETE %s%d HTTP/1.1\r\n\r\n", SINGLE_PATH, index) >= 0);
}


