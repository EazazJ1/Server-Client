#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "NetworkingFunctions.h"


void parsing(char buffer[])
{
	char author[] = "";
	char title[] = "";
	char topic[] = "";
	int postNum = 0;

	char delimiterSpace = " ";
	char delimSlash = "/";
	char delimAmpersand = "&";
	char delimQuestion = "?";


	for (int i = 0; i < 1000; i++)
	{
		char* firstWord = strtok(buffer, delimiterSpace);

		char* get = "GET\n";

		if (strcmp(firstWord, get) == 0)
		{
			createGetPayload(buffer);
		}
		else
		{
			createPayload(buffer);
		}

		//if GET then check to see if there is a number, if no number then display all

		//if POST then check for the inputs

		//if PUT then check for the postNumber and the input

		//if delete then check for the postNumber
	}
}