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

	/*char author[100];
	memset(author, '\0', 100);

	char topic[100];
	memset(topic, '\0', 100);

	char title[100];
	memset(title, '\0', 100);

	int postNum;*/


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


	//int hi = 0;
	
	switch (choice)
	{
	case 0:
		
		printf("Exiting...");

		exitMenuandCall(message);

		break;
	case 1:

		getAllPosts(message);

		break;
	case 2:
		
		getSinglePostInputandCall(message);

		break;
	case 3:
		
		postInputandCall(message);
		
		break;
	case 4:

		putInputandCall(message);

		break;
	case 5:

		deleteInputandCall(message);

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