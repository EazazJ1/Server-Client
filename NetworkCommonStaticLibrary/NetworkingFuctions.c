//CSCN72020-F21
//Assignment 3
//
//Eazaz Jakda
//
//December 10. 2021
//

// to create a library for use in multiple projects, follow:
//
//1. Create new empty project
//-- > Properties
//------ > General
//---------- > change Configuration Type to static library
//
//2. copy/create source files in new project
//
//3. Main Project(existing) that needs to use library :
//----->References
//--------->add new reference.select Library project
//
//4. Server project properties
//-- > C / C++
//------->General
//----------->Additional Include Directories, add 
//							$(SolutionDir)NetworkCommonStaticLibrary
//								* no \ after macro!

#include "NetworkingFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

void InitializeWindowsSockets()
{
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "failed to initialize network.  Exiting\n");
		exit(1);
	}
}

struct addrinfo* configureAddress(char* host, char* port, PROTOCOL protocol)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;                  // gonna be v4
	if (protocol == TCP)
	{
		hints.ai_socktype = SOCK_STREAM;    // gonna be tcp
		hints.ai_protocol = IPPROTO_TCP;
	}
	else if (protocol == UDP)
	{
		hints.ai_socktype = SOCK_DGRAM;		// gonna be udp
		hints.ai_protocol = IPPROTO_UDP;
	}
	else  //unknown   exit!
	{
		fprintf(stderr, "Unknown protocol selected. Exiting\n");
		exit(EXIT_FAILURE);
	}
	hints.ai_flags = AI_PASSIVE;                    //bind to * address

	struct addrinfo* bind_address;  //no malloc required here.  see note below
	if (!strncmp(host, "*", 1))	// 0 if same!
		getaddrinfo(0, port, &hints, &bind_address);
	else
		getaddrinfo(host, port, &hints, &bind_address);
	//
	// from: https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
	// All information returned by the getaddrinfo function pointed to by the ppResult 
	// parameter is dynamically allocated, including all addrinfo structures, socket address 
	// structures, and canonical host name strings pointed to by addrinfo structures. Memory
	// allocated by a successful call to this function must be released with a subsequent call
	// to freeaddrinfo.
	return bind_address;
}


struct addrinfo* ConfigureLocalAddress(char* port, PROTOCOL protocol)
{
	return configureAddress("*", port, protocol);
}

struct addrinfo* ConfigureRemoteAddress(char* remoteHost, char* remotePort, PROTOCOL protocol)
{
	return configureAddress(remoteHost, remotePort, protocol);
}

SOCKET CreateBindListeningSocket(struct addrinfo* bind_address)
{
	SOCKET socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype,
		bind_address->ai_protocol);
	if ((socket_listen) == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed, exiting with error (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		fprintf(stderr, "bind() failed, exiting with error  (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(bind_address);  //see note above.  this frees all the allocated mem.
	return socket_listen;
}

SOCKET CreateAndConnectRemoteSocket(struct addrinfo* peer_address)
{
	SOCKET socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
	if ((socket_peer) == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed, exiting with error (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed, exiting with error (%d)\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(peer_address);

	return socket_peer;
}

void StartListeningForConnections(SOCKET socket_listen)
{
	if (listen(socket_listen, MAXLISTENERS) < 0)      //allowed to queue up to 10 connections
	{
		fprintf(stderr, "listen() failed, exiting with and error of (%d)\n", WSAGetLastError());
		exit(1);
	}
}


SOCKET WaitForAndAcceptConnection(SOCKET socket_listen)
{
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	SOCKET socket_client = accept(socket_listen, (struct sockaddr*)&client_address, &client_len);
	if ((socket_client) == INVALID_SOCKET)
	{
		fprintf(stderr, "accept() failed. (%d)\n", WSAGetLastError());
		return 1;
	}

	//opportunity to clean this up!
	char address_buffer[DISPLAYBUFFERSIZE];
	getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer,
		sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
	printf("%s\n", address_buffer);

	return socket_client;
}

void createPayload(char* buffer)
{
	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n"
		"Local time is: ";

	time_t timer;
	time(&timer);
	char* time_msg = ctime(&timer);

	sprintf(buffer, "%s %s\n\0\0", response, time_msg);
}

void RecvRequestAndSendResponse(SOCKET socket_client, LIST postings[])
{
	int killSwitch = 0;

	while (killSwitch == 0)			//Main loop for server to keep running until EXIT
	{
		printf("Reading request...\n");
		char request[SENDBUFFERSIZE];
		memset(request, '\0', SENDBUFFERSIZE);
		int bytes_received = recv(socket_client, request, SENDBUFFERSIZE, 0);
		printf("Received %d bytes: \n", bytes_received);
		printf("%.*s\n", bytes_received, request);      //the %.*s makes sure we print exactly as many chars as was received (regardless of null termination)

		printf("building response\n");
		char buffer[SENDBUFFERSIZE];
		memset(buffer, '\0', SENDBUFFERSIZE);

		char* firstWord = "";
		firstWord = strtok(request, " ");

		int switchNum = OperationNum(firstWord);

		char temp[100];
		memset(temp, '\0', 100);
		strncpy(temp, &request[7], 99);		

		char* newPost;
		newPost = strtok(temp, "/");

		char post[20];
		memset(post, '\0', 20);

		char author[20];
		memset(author, '\0', 20);

		char title[20];
		memset(title, '\0', 20);

		char topic[20];
		memset(topic, '\0', 20);

		int count = 1;		

		switch (switchNum)
		{
		case 1:		//GET
			//GET /posts OR GET /posts/# OR GET /posts?
			if (request[10] == ' ')		//GET collection
			{
				getAll(buffer, postings);
			}
			else if (request[10] == '?')		//GET by filter
			{
				parseFilter(buffer, request, postings);
			}
			else		//GET one
			{			
				char num[2];
				memset(num, '\0', 1);
				strncpy(num, &request[11], 2);

				int postNum = atoi(num);

				getOne(buffer, postings[postNum - 1]);
			}
			break;
		case 2:		//POST

			while (newPost != NULL)
			{
				newPost = strtok(NULL, "/");
				if (count == 1)
				{
					strcpy(author, newPost);
					count++;
				}
				else if (count == 2)
				{
					strcpy(topic, newPost);
					count++;
				}
				else if (count == 3)
				{
					strcpy(title, newPost);
					count++;
				}
				else
				{
					break;
				}
			}

			postOne(buffer, author, topic, title, postings);

			break;
		case 3:		//PUT

			while (newPost != NULL)
			{
				newPost = strtok(NULL, "/");
				if (count == 1)
				{
					strcpy(post, newPost);
					count++;
				}
				else if (count == 2)
				{
					strcpy(author, newPost);
					count++;
				}
				else if (count == 3)
				{
					strcpy(topic, newPost);
					count++;
				}
				else if (count == 4)
				{
					strcpy(title, newPost);
					count++;
				}
				else
				{
					break;
				}
			}

			int number = atoi(post);

			putOne(buffer, author, topic, title, &postings[number - 1]);

			break;
		case 4:		//DELETE
			//DELETE /posts/#/
			while (newPost != NULL)
			{
				newPost = strtok(NULL, "/");
				if (count == 1)
				{
					strcpy(post, newPost);
					count++;
				}
				else
				{
					break;
				}
			}

			int deleteNum = atoi(post) - 1;
			deleteOne(buffer, postings, deleteNum);

			break;
		case 5:		//EXIT

			killSwitch = 1;
			break;
		default:

			//printf("default");
			break;
		}

		printf("Sending response...\n");
		int bytes_sent = send(socket_client, buffer, strlen(buffer), 0);
		printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(buffer));
	}
}

void RecvUDPRequestAndSendResponse(SOCKET listen_socket)
{
	struct sockaddr_in cliaddr;
	char buffer[SENDBUFFERSIZE];

	int clientLength = sizeof(cliaddr);  //len is value/resuslt

	int bytesReceived = recvfrom(listen_socket,
		buffer,
		SENDBUFFERSIZE,
		0,
		(struct sockaddr*)&cliaddr,
		&clientLength);
	buffer[bytesReceived] = '\0';	// no guarantee the payload will be NULL terminated
	printf("\nClient sent : %s\n", buffer);

	createPayload(buffer);

	sendto(listen_socket,
		buffer,
		strlen(buffer),
		0,
		(const struct sockaddr*)&cliaddr,
		clientLength);
	printf("Response message sent.\n");
}

void CloseSocketConnection(SOCKET this_socket)
{
	shutdown(this_socket, SD_BOTH);
	closesocket(this_socket);
}

void ShutdownWindowsSockets()
{
	WSACleanup();
}


int OperationNum(char firstword[])
{
	int result;

	if (strcmp(firstword, "GET") == 0)
	{
		result = 1;

	}
	else if (strcmp(firstword, "POST") == 0)
	{
		result = 2;
	}
	else if (strcmp(firstword, "PUT") == 0)
	{
		result = 3;
	}
	else if (strcmp(firstword, "DELETE") == 0)
	{
		result = 4;
	}
	else if (strcmp(firstword, "QUIT") == 0)
	{
		result = 5;
	}
	else
	{
		return 0;
	}

	return result;
}

void InitList(LIST postings[])
{
	for (int i = 0; i < 10; i++)
	{
		postings[i].postNum = i + 1;
		strcpy(postings[i].author, "unset");
		strcpy(postings[i].topic, "unset");
		strcpy(postings[i].title, "unset");
		postings[i].set = false;
	}
}

void getAll(char* buffer, LIST postings[])
{
	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n";

	strcat(buffer, response);

	for (int i = 0; i < 10; i++)
	{
		if (postings[i].set == true)
		{
			char tempString[SENDBUFFERSIZE];
			memset(tempString, '\0', SENDBUFFERSIZE);
			snprintf(tempString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
			strcat(buffer, tempString);
		}

	}


}
void getFilterStrings(char* buffer, char* request, LIST postings[], char* str[])
{
	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n";

	strcat(buffer, response);

	char wantedString[SENDBUFFERSIZE];
	memset(wantedString, '\0', SENDBUFFERSIZE);

	if (strcmp(str[0], "author") == 0 && strcmp(str[2], "topic") == 0 && strcmp(str[4], "title") == 0)
	{
		printf("All 3");

		for (int i = 0; i < MAXPOSTINGS; i++)
		{
			if (strcmp(str[1], postings[i].author) == 0)
			{
				if (strcmp(str[3], postings[i].topic) == 0)
				{
					if (strcmp(str[5], postings[i].title) == 0)
					{
						snprintf(wantedString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
						strcat(buffer, wantedString);
					}
				}
			}
		}
	}
	else if (strcmp(str[0], "topic") == 0 && strcmp(str[2], "title") == 0 && strcmp(str[4], "") == 0)
	{
		printf("topic and title");

		for (int i = 0; i < MAXPOSTINGS; i++)
		{
			if (strcmp(str[1], postings[i].topic) == 0)
			{
				if (strcmp(str[3], postings[i].title) == 0)
				{
					snprintf(wantedString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
					strcat(buffer, wantedString);
				}
			}
		}
	}
	else if (strcmp(str[0], "author") == 0 && strcmp(str[2], "topic") == 0 && strcmp(str[4], "") == 0)
	{
		printf("author and topic");

		for (int i = 0; i < MAXPOSTINGS; i++)
		{
			if (strcmp(str[1], postings[i].author) == 0)
			{
				if (strcmp(str[3], postings[i].topic) == 0)
				{
					snprintf(wantedString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
					strcat(buffer, wantedString);
				}
			}
		}
	}
	else if (strcmp(str[0], "author") == 0 && strcmp(str[2], "title") == 0 && strcmp(str[4], "") == 0)
	{
		printf("author and title");

		for (int i = 0; i < MAXPOSTINGS; i++)
		{
			if (strcmp(str[1], postings[i].author) == 0)
			{
				if (strcmp(str[3], postings[i].title) == 0)
				{
					snprintf(wantedString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
					strcat(buffer, wantedString);
				}
			}
		}
	}
	else if (strcmp(str[0], "author") == 0 && strcmp(str[2], "") == 0 && strcmp(str[4], "") == 0)
	{
		printf("Only author");

		for (int i = 0; i < MAXPOSTINGS; i++)
		{
			if (strcmp(str[1], postings[i].author) == 0)
			{
				snprintf(wantedString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
				strcat(buffer, wantedString);
			}
		}
	}
	else if (strcmp(str[0], "topic") == 0 && strcmp(str[2], "") == 0 && strcmp(str[4], "") == 0)
	{
		printf("Only topic");

		for (int i = 0; i < MAXPOSTINGS; i++)
		{
			if (strcmp(str[1], postings[i].topic) == 0)
			{
				snprintf(wantedString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
				strcat(buffer, wantedString);
			}
		}
	}
	else if (strcmp(str[0], "title") == 0 && strcmp(str[2], "") == 0 && strcmp(str[4], "") == 0)
	{
		printf("Only title");

		for (int i = 0; i < MAXPOSTINGS; i++)
		{
			if (strcmp(str[1], postings[i].title) == 0)
			{
				snprintf(wantedString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
				strcat(buffer, wantedString);
			}
		}
	}
	else
	{
		printf("post not found");
	}
}

void parseFilter(char* buffer, char* request, LIST postings[])
{
	char delim[] = "= &";

	char filterString[100];
	memset(filterString, '\0', 100);

	strncpy(filterString, &request[11], 99);

	char* filter;
	filter = strtok(filterString, delim);

	char* str[6];


	for (int i = 0; i < 6; i++)
	{
		str[i] = calloc((strlen(filter) + 1), *filter);
	}

	int myNum = 1;
	strcpy(str[0], filter);

	while (filter != NULL)
	{
		filter = strtok(NULL, delim);

		if (filter == NULL)
		{
			break;
		}

		if (myNum == 1)
		{
			strcpy(str[1], filter);
			myNum++;
		}
		else if (myNum == 2)
		{
			strcpy(str[2], filter);
			myNum++;
		}
		else if (myNum == 3)
		{
			strcpy(str[3], filter);
			myNum++;
		}
		else if (myNum == 4)
		{
			strcpy(str[4], filter);
			myNum++;
		}
		else if (myNum == 5)
		{
			strcpy(str[5], filter);
			myNum++;
		}
		else
		{
			break;
		}

	}

	getFilterStrings(buffer, request, postings, str);		//actually use filter to get posting and send back

	free(*str);
}

void getOne(char* buffer, LIST postings)
{

	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n";

	strcat(buffer, response);
	char tempStringTwo[SENDBUFFERSIZE];
	memset(tempStringTwo, '\0', SENDBUFFERSIZE);

	snprintf(tempStringTwo, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings.postNum, postings.author, postings.topic, postings.title);

	strcat(buffer, tempStringTwo);


}

void postOne(char* buffer, char* author, char* topic, char* title, LIST postings[])
{
	int count = 0;

	for (int i = 0; i < 10; i++)
	{
		if (postings[i].set == false)
		{
			strcpy(postings[i].author, author);
			strcpy(postings[i].topic, topic);
			strcpy(postings[i].title, title);
			postings[i].set = true;
			int count = 1;
			break;
		}

		if (count == 1)
		{
			break;
		}
	}

	const char* response =
		"\nHTTP/1.1 200 CREATED\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n";


	strcat(buffer, response);

}

void putOne(char* buffer, char* author, char* topic, char* title, LIST* postings)
{
	char* unchanged = "unchanged";
	if (strcmp(author, "unchanged") != 0)
	{
		strcpy(postings->author, author);
	}
	if (strcmp(topic, "unchanged") != 0)
	{
		strcpy(postings->topic, topic);
	}
	if (strcmp(title, "unchanged") != 0)
	{
		strcpy(postings->title, title);
	}

	const char* response =
		"\nHTTP/1.1 200 CREATED\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n";


	strcat(buffer, response);

}


void deleteOne(char* buffer, LIST postings[], int num)
{
	strcpy(postings[num].author, "UNSET");
	strcpy(postings[num].topic, "UNSET");
	strcpy(postings[num].title, "UNSET");
	postings[num].set = false;

	for (int i = 0; i < 10; i++)
	{
		if (postings[i].set == false)
		{
			if (postings[i + 1].set == true)
			{
				strcpy(postings[i].author, postings[i + 1].author);
				strcpy(postings[i].topic, postings[i + 1].topic);
				strcpy(postings[i].title, postings[i + 1].title);
				postings[i].set = true;

				strcpy(postings[i + 1].author, "unset");
				strcpy(postings[i + 1].topic, "unset");
				strcpy(postings[i + 1].title, "unset");
				postings[i + 1].set = false;
			}
		}
	}

	const char* response =
		"\nHTTP/1.1 200 NO CONTENT\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n";


	strcat(buffer, response);

}