#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ClientFunctions.h"


void getOnePost(char* message, int num)
{
	char defaultMessage[1000] = { "GET /posts/" };

	//strcat(defaultMessage, num);
	char number[5];
	sprintf(number, "%d", num);
	strcat(defaultMessage, number);

	snprintf(message, "%s", defaultMessage);

}

void getAllPosts(char* message)
{
	char defaultMessage[1000] = { "GET /posts " };

	snprintf(message, "%s", defaultMessage);

}

void addPost(char* message, char* author, char* topic, char* title) 
{
	char defaultMessage[1000] = { "POST /posts/" };

	//char temp[1000];

	//snprintf(temp, "%s%s/%s/%s/", defaultMessage, &author, &topic, &title);

	strcat(defaultMessage, author);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, topic);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, title);



	strcat(message, defaultMessage);


}

void updatePost(char* message, char* author, char* topic, char* title, int num)
{
	char number[5];
	sprintf(number, "%d", num);
	
	char defaultMessage[1000] = { "PUT /posts/" };

	strcat(defaultMessage, number);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, author);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, topic);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, title);



	strcat(message, defaultMessage);
}

void deletePost(char* message, int num)
{
	char defaultMessage[1000] = { "DELETE /posts/" };

	char number[5];
	sprintf(number, "%d", num);

	strcat(defaultMessage, number);

	strcat(message, defaultMessage);
}