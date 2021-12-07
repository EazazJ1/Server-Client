//cscn72020f21 - simple network client to attach to our simple time server
// steveh - nov 2021
#define _CRT_SECURE_NO_WARNINGS

#include "NetworkingFunctions.h"
#include "ClientFunctions.h"
#include <stdio.h>
#include <ctype.h>


#define MAXBUFFER	1000

int main(void)
{
	// first set up Winsock  (same as server)
	InitializeWindowsSockets();

	printf("Config the remote addr...\n");
	struct addrinfo* peer_address = ConfigureRemoteAddress("127.0.0.1", "8080", TCP);

	printf("Creating socket and connect...\n");
	SOCKET peer_socket = CreateAndConnectRemoteSocket(peer_address);

	//send initial 'greeting'
	//char* message = "";

	char message[1000];
	memset(message, '\0', 1000);

	char author[100];
	memset(author, '\0', 100);

	char topic[100];
	memset(topic, '\0', 100);

	char title[100];
	memset(title, '\0', 100);

	int postNum;




	printf("\nPick the option for the operation you would like to do:\n");
	printf("0. EXIT\n1. Get all posts.\n2. Get one post.\n3. Create a new post.\n4. Update a post.\n5. Delete a post.\n");

	int choice;

	scanf_s("%d", &choice);
	//printf("%d", choice);

	/*if (scanf("%d", &choice))
	{
		if (choice >= 0 && choice <=6)
		{
			
			

		}
	}*/


	int hi = 0;
	
	switch (choice)
	{
	case 0:
		
		printf("Exiting...");

		strcpy(message, "QUIT ");

		break;
	case 1:

		getAllPosts(message);

		break;
	case 2:
		
		while (hi == 0)
		{
			scanf_s("%*c");
			printf("Please enter the post number you would like to get between 1 and 10:");
			
			if (scanf("%d", &postNum))
			{
				if (postNum > 0 && postNum < 11)
				{					
					getOnePost(message, postNum);
					hi = 1;			
					
				}
			}

			/*char line[256];

			if (fgets(line, sizeof line, stdin) != NULL)
			{
				sscanf(line, "%d", &postNum);
			}*/
		}
		break;
	case 3:
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

		break;
	case 4:
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

		while (hi == 0)
		{
			scanf_s("%*c");
			printf("What post number would you like to change?");
			if (scanf("%d", &postNum))
			{
				if (postNum > 0 && postNum < 11)
				{
					updatePost(message, author, topic, title, postNum);
					hi = 1;

				}
				else if(postNum == 0)
				{
					break;
				}
				else
				{
					hi = 0;
				}
			}
		}

		break;
	case 5:

		while (hi == 0)
		{
			scanf_s("%*c");
			printf("What post number would you like to delete?");
			if (scanf("%d", &postNum))
			{
				if (postNum > 0 && postNum < 11)
				{
					deletePost(message, postNum);
					hi = 1;

				}
				else if (postNum == 0)
				{
					break;
				}
				else
				{
					hi = 0;
				}
			}
		}

		break;
	default:
	
		printf("Default Switch");

		break;

	}




	int sent = send(peer_socket, message, strlen(message), 0);
	if (sent == 0)
	{
		fprintf(stderr, "send failed\n");
		exit(1);
	}

	//receive reply from server
	char buffer[MAXBUFFER];
	memset(buffer, '\0', MAXBUFFER);

	while (recv(peer_socket, buffer, MAXBUFFER, 0) != 0)
		;

	printf("%s", buffer);

	printf("Closing the connection...\n");
	CloseSocketConnection(peer_socket);
	ShutdownWindowsSockets();
}