//CSCN72020-F21
//Assignment 3
//
//Eazaz Jakda
//
//December 10. 2021
//


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

	int killSwitch = 0;

	while (killSwitch == 0)		//Main loop for client to keep running until EXIT
	{
		
		bool checkMenuInput = true;
		int choice;

		while (checkMenuInput)
		{

			printf("\nPick the option for the operation you would like to do:\n");
			printf("0. EXIT\n1. Get all posts.\n2. Get post by filter. \n3. Get one post. \n4. Create a new post.\n5. Update a post.\n6. Delete a post.\n");

			if (scanf_s("%d", &choice))
			{
				if (choice >= 0 && choice <= 6)
				{
					checkMenuInput = false;
				}
			}
			else
			{
				scanf_s("%*c");
				printf("\nWrong Input please try again.\n");
				checkMenuInput = true;
			}
		}
		
		char message[1000];
		memset(message, '\0', 1000);

		int menuExit = 0;

		switch (choice)
		{
		case 0:		//Exit

			printf("Exiting...");

			exitMenuandCall(message);
			killSwitch = 1;

			break;
		case 1:		//get collection

			getAllPosts(message);
			
			break;
		case 2:		//get with filtering

			getFilterInputandCall(message);
			if (strcmp(message, "QUIT ") == 0)
			{
				menuExit = 1;
			}

			break;
		case 3:		//get a single post 

			getSinglePostInputandCall(message);
			if (strcmp(message, "QUIT ") == 0)
			{
				menuExit = 1;
			}

			break;
		case 4:		//add a posting 

			postInputandCall(message);

			break;
		case 5:		//update a post

			putInputandCall(message);
			if (strcmp(message, "QUIT ") == 0)
			{
				menuExit = 1;
			}
			break;
		case 6:		//delete a post

			deleteInputandCall(message);
			if (strcmp(message, "QUIT ") == 0)
			{
				menuExit = 1;
			}
			break;
		default:

			printf("Default Switch");

			break;
		}

		if (menuExit == 0)		//if request is garbage then go back to menu and dont send request
		{
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
			{
				printf("%s", buffer);
				break;
			}
		}
		
	}

	printf("Closing the connection...\n");
	CloseSocketConnection(peer_socket);
	ShutdownWindowsSockets();
}