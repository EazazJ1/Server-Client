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




void exitMenuandCall(char* message)
{
	strcpy(message, "QUIT ");
}

void getSinglePostInputandCall(char* message)
{
	int check = 0;
	int postNum;

	while (check == 0)
	{
		scanf_s("%*c");
		printf("Please enter the post number you would like to get between 1 and 10:");

		if (scanf("%d", &postNum))
		{
			if (postNum > 0 && postNum < 11)
			{
				getOnePost(message, postNum);
				check = 1;
			}
		}
	}
}

void postInputandCall(char* message)
{
	char author[100];
	memset(author, '\0', 100);

	char topic[100];
	memset(topic, '\0', 100);

	char title[100];
	memset(title, '\0', 100);
	
	scanf_s("%*c");
	printf("Please enter the name of the author: ");
	scanf("%[^\n]s", author);
	//fgets(author, sizeof(author), stdin);

	scanf_s("%*c");
	printf("Please enter the name of the topic: ");
	scanf("%[^\n]s", topic);
	//fgets(topic, sizeof(topic), stdin);

	scanf_s("%*c");
	printf("Please enter the name of the title: ");
	scanf("%[^\n]s", title);
	//fgets(title, sizeof(title), stdin);
	scanf_s("%*c");

	addPost(message, author, topic, title);
}

void putInputandCall(char* message)
{
	char author[100];
	memset(author, '\0', 100);

	char topic[100];
	memset(topic, '\0', 100);

	char title[100];
	memset(title, '\0', 100);

	int check = 0;
	int postNum;

	scanf_s("%*c");
	printf("Would like to change the author [y/n]? ");
	char authorchar;
	authorchar = tolower(getc(stdin));
	if (authorchar == 'y')
	{
		scanf_s("%*c");
		printf("Please enter the name of the author: ");
		scanf("%[^\n]s", author);
	}
	else if (authorchar == 'n')
	{
		strcpy(author, "unchanged");
	}
	else
	{
		printf("Wrong input. The author name will NOT be changed.");
	}

	scanf_s("%*c");
	printf("Would like to change the topic [y/n]? ");
	char topicchar;
	topicchar = tolower(getc(stdin));
	if (topicchar == 'y')
	{
		scanf_s("%*c");
		printf("Please enter the name of the topic: ");
		scanf("%[^\n]s", topic);
	}
	else if (topicchar == 'n')
	{
		strcpy(topic, "unchanged");
	}
	else
	{
		printf("Wrong input. The topic will NOT be changed.");
	}

	scanf_s("%*c");
	printf("Would like to change the title [y/n]? ");
	char titlechar;
	titlechar = tolower(getc(stdin));
	if (titlechar == 'y')
	{
		scanf_s("%*c");
		printf("Please enter the name of the title: ");
		scanf("%[^\n]s", title);
	}
	else if (titlechar == 'n')
	{
		strcpy(title, "unchanged");
	}
	else
	{
		printf("Wrong input. The title will NOT be changed.");
	}

	while (check == 0)
	{
		scanf_s("%*c");
		printf("What post number would you like to change?");
		if (scanf("%d", &postNum))
		{
			if (postNum > 0 && postNum < 11)
			{
				updatePost(message, author, topic, title, postNum);
				check = 1;

			}
			else if (postNum == 0)
			{
				break;
			}
			else
			{
				check = 0;
			}
		}
	}
}


void deleteInputandCall(char* message)
{
	int check = 0;
	int postNum;
	
	while (check == 0)
	{
		scanf_s("%*c");
		printf("What post number would you like to delete?");
		if (scanf("%d", &postNum))
		{
			if (postNum > 0 && postNum < 11)
			{
				deletePost(message, postNum);
				check = 1;

			}
			else if (postNum == 0)
			{
				break;
			}
			else
			{
				check = 0;
			}
		}
	}
}