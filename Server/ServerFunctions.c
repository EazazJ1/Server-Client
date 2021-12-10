#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ServerFunctions.h"
#include "NetworkingFunctions.h"



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