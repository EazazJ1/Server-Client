#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ServerFunctions.h"
#include "NetworkingFunctions.h"
//#define MAXLEN 50
//typedef struct list {
//	int postNum;
//	char author[MAXLEN];
//	char title[MAXLEN];
//	char topic[MAXLEN];
//
//} LIST;

//void InitList(LIST postings[])
//{
//	for (int i = 0; i < 10; i++)
//	{
//		postings[i].postNum = i + 1;
//		strcpy(postings[i].author, "unset");
//		strcpy(postings[i].topic, "unset");
//		strcpy(postings[i].title, "unset");
//	}
//}

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

void load(LIST postings[])
{
	FILE* loadFile;
	loadFile = fopen("Data.txt", "r");

	if (loadFile == NULL)
	{
		fprintf(stderr, "\nError opening file\n");
		exit(1);
	}

	for (int i = 0; i < 10; i++)
	{
		fread(&postings[i], sizeof(LIST), 1, loadFile);
	}

	fclose(loadFile);
}

void save(LIST postings[])
{
	FILE* saveFile;

	// open file for writing
	saveFile = fopen("Data.txt", "w+");
	if (saveFile == NULL)
	{
		fprintf(stderr, "\nError opened file\n");
		exit(1);
	}
	for (int i = 0; i < 10; i++)
	{
		fwrite(&postings[i], sizeof(LIST), 1, saveFile);

		//fprintf(saveFile, "%d %s %s %s %d\n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title, postings[i].set);
	}

	if (fwrite != 0)
		printf("contents to file written successfully !\n");
	else
		printf("error writing file !\n");

	// close file
	fclose(saveFile);



	FILE* saveFile2;

	// open file for writing
	saveFile2 = fopen("DataCheck.txt", "w+");
	if (saveFile2 == NULL)
	{
		fprintf(stderr, "\nError opened file\n");
		exit(1);
	}
	for (int i = 0; i < 10; i++)
	{
		//fwrite(&postings[i], sizeof(LIST), 1, saveFile);

		fprintf(saveFile2, "%d %s %s %s %d\n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title, postings[i].set);
	}

	if (fwrite != 0)
		printf("contents to file written successfully !\n");
	else
		printf("error writing file !\n");

	// close file
	fclose(saveFile2);
}