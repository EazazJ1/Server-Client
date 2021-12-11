//CSCN72020-F21
//Assignment 3
//
//Eazaz Jakda
//
//December 10. 2021
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ClientFunctions.h"


void getOnePost(char* message, int num)		//set the message up to send to the server for getting one post
{
	char defaultMessage[1000] = { "GET /posts/" };		//get format before adding number

	char number[5];
	sprintf(number, "%d", num);
	strcat(defaultMessage, number);		//add the post number to the string

	snprintf(message, "%s", defaultMessage);		//add it to the message to send to server

}

void getAllPosts(char* message)		//set the message up to send to the server for getting all posts
{
	char defaultMessage[1000] = { "GET /posts " };		//get format for all posts

	snprintf(message, "%s", defaultMessage);		//add to message to send to server

}

void getFilter(char* message, char* author, char* topic, char* title)		//set the message up to send to the server for getting a post by filtering
{
	char defaultMessage[1000] = { "GET /posts?" };		//get format for getting post with filter
	int count = 0;

	if (strcmp(author, "na") != 0)			//if author is one of the filters then add the information to the string so that it can be added to the message at the end
	{
		strcat(defaultMessage, "author=");
		strcat(defaultMessage, author);
		count++;
	}

	if (strcmp(topic, "na") != 0)		//if topic is one of the filters then add the information to the string so that it can be added to the message at the end
	{
		
		if (count == 0)
		{
			strcat(defaultMessage, "topic=");
			strcat(defaultMessage, topic);
			count++;
		}
		else
		{
			strcat(defaultMessage, "&");
			strcat(defaultMessage, "topic=");
			strcat(defaultMessage, topic);
			count++;
		}
		
		
	}

	if (strcmp(title, "na") != 0)		//if title is one of the filters then add the information to the string so that it can be added to the message at the end
	{
		
		if (count == 0)
		{
			strcat(defaultMessage, "title=");
			strcat(defaultMessage, title);
			count++;
		}
		else
		{
			strcat(defaultMessage, "&");
			strcat(defaultMessage, "title=");
			strcat(defaultMessage, title);
			count++;
		}

	}

	if (count == 0)			//if no filters then set message to "QUIT " and go back to menu
	{
		exitMenuandCall(message);
	}
	else
	{
		strcat(message, defaultMessage);	// add everything to the message to send to server
	}

	

}

void addPost(char* message, char* author, char* topic, char* title)		//set the message up to send to the server for adding a new post
{
	char defaultMessage[1000] = { "POST /posts/" };		//Post format before values

	strcat(defaultMessage, author);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, topic);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, title);



	strcat(message, defaultMessage);		//add everything to message to send to server


}

void updatePost(char* message, char* author, char* topic, char* title, int num)			//set the message up to send to the server for updating a post
{
	char number[5];
	sprintf(number, "%d", num);
	
	char defaultMessage[1000] = { "PUT /posts/" };		//put format

	strcat(defaultMessage, number);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, author);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, topic);
	strcat(defaultMessage, "/");
	strcat(defaultMessage, title);



	strcat(message, defaultMessage);		//add everything to the message to send to the server
}

void deletePost(char* message, int num)		//set the message up to send to the server for deleting a post
{
	char defaultMessage[1000] = { "DELETE /posts/" };		//delete format

	char number[5];
	sprintf(number, "%d", num);

	strcat(defaultMessage, number);

	strcat(message, defaultMessage);	//add number to the message to send to the server
}




void exitMenuandCall(char* message)		//send a message to let server know to exit
{
	strcpy(message, "QUIT ");
}

void getSinglePostInputandCall(char* message)		//get input for the get a single post function
{
	int check = 0;
	int postNum;

	while (check == 0)
	{
		scanf_s("%*c");
		printf("Please enter the post number you would like to get between 1 and 10 OR enter 0 to go back to menu: ");

		if (scanf("%d", &postNum))
		{
			if (postNum > 0 && postNum < 11)
			{
				getOnePost(message, postNum);
				check = 1;
			}
			if(postNum == 0)
			{
				exitMenuandCall(message);
				check = 1;
			}
		}
	}
}

void postInputandCall(char* message)		//get input for the add a post function
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

void putInputandCall(char* message)		//get input for the update a post function
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
	printf("Would you like to change the author [y/n]? ");
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
		printf("Wrong input. The author name will NOT be changed.\n");
		strcpy(author, "unchanged");
	}

	scanf_s("%*c");
	printf("Would you like to change the topic [y/n]? ");
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
		printf("Wrong input. The topic will NOT be changed.\n");
		strcpy(topic, "unchanged");
	}

	scanf_s("%*c");
	printf("Would you like to change the title [y/n]? ");
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
		printf("Wrong input. The title will NOT be changed.\n");
		strcpy(title, "unchanged");
	}

	while (check == 0)
	{
		scanf_s("%*c");
		printf("What post number would you like to change between 1-10? OR enter 0 to go back to menu: ");
		if (scanf("%d", &postNum))
		{
			if (postNum > 0 && postNum < 11)
			{
				updatePost(message, author, topic, title, postNum);
				check = 1;

			}
			else if (postNum == 0)
			{
				exitMenuandCall(message);
				check = 1;
				
				//break;
			}
			else
			{
				check = 0;
			}
		}
	}
}


void deleteInputandCall(char* message)		//get input for the delete a single post function
{
	int check = 0;
	int postNum;
	
	while (check == 0)
	{
		scanf_s("%*c");
		printf("What post number would you like to delete? OR enter 0 to go back to menu: ");
		if (scanf("%d", &postNum))
		{
			if (postNum > 0 && postNum < 11)
			{
				deletePost(message, postNum);
				check = 1;

			}
			else if (postNum == 0)
			{
				exitMenuandCall(message);
				check = 1;
				
				//break;
			}
			else
			{
				check = 0;
			}
		}
	}
}

void getFilterInputandCall(char* message)		//get input for the get a post with filters function
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
	printf("Would you like to filter by the author [y/n]? ");
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
		//scanf_s("%*c");
		strcpy(author, "na");
	}
	else
	{
		//scanf_s("%*c");
		printf("Wrong input. The author name will NOT be changed.\n");
		strcpy(author, "na");
	}

	scanf_s("%*c");
	printf("Would you like to filter by the topic [y/n]? ");
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
		//scanf_s("%*c");
		strcpy(topic, "na");
	}
	else
	{
		//scanf_s("%*c");
		printf("Wrong input. The topic will NOT be changed.\n");
		strcpy(topic, "na");
	}

	scanf_s("%*c");
	printf("Would you like to filter the title [y/n]? ");
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
		//scanf_s("%*c");
		strcpy(title, "na");
	}
	else
	{
		//scanf_s("%*c");
		printf("Wrong input. The title will NOT be changed.\n");
		strcpy(title, "na");
	}

	getFilter(message, author, topic, title);
	
}