//cscn72020 - week12 - a simple example of a networked service.
//steveh - nov 2021

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

void createGetPayload(char* buffer)
{
	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Type: text/plain\r\n\r\n"
		"GET: Steve is an asshole. ";



	sprintf(buffer, "%s\n\0\0", response);
}

void RecvRequestAndSendResponse(SOCKET socket_client, LIST postings[])
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
	//char get[] = "GET\n";
	char* firstWord = "";

	firstWord = strtok(request, " ");

	int switchNum = OperationNum(firstWord);


	//here
	char temp[100];
	memset(temp, '\0', 100);
	strncpy(temp, &request[7], 99);		//changed from 5 to 7 because of DELETE

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
	//here

	switch (switchNum)
	{
	case 1:
		//printf("The GET was called");
		//GET /posts/ OR GET /posts/#

		if (request[11] == ' ') //changed from newline // can change to [10] if i want uri to be localhost:8080/posts
		{
			printf("Print all posts\n");
			//call get function to print all
			getAll(buffer, postings);
			//createPayload(buffer);
		}
		else
		{
			char number[10] = "";

			char num[2];
			memset(num, '\0', 1);
			strncpy(num, &request[11], 2);

			int postNum = atoi(num);

			getOne(buffer, postings[postNum - 1]);
			//printf("This is the post number you want %d\n", postNum);
			//call get function for post number

			//createPayload(buffer);
		}
		break;
	case 2:
		printf("The POST was called");
		//POST /posts/author/title/

		/*char temp[100];
		memset(temp, '\0', 100);
		strncpy(temp, &request[5], 99);

		char* newPost;
		newPost = strtok(temp, "/");
		
		char author[20];
		memset(author, '\0', 20);

		char title[20];
		memset(title, '\0', 20);

		char topic[20];
		memset(topic, '\0', 20);*/


		//int count = 1;
		while(newPost != NULL)
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

		postOne(author, topic, title, postings);

		//printf("\nAuthor: %s, Topic: %s, Title: %s\n", author, topic, title);

		//createPayload(buffer);
		//getOne(buffer, postings[1]);
		getAll(buffer, postings);
		
		break;
	case 3:
		printf("The PUT was called");
		//PUT /posts/#/author/title/topic/

		/*char tempPut[100];
		memset(tempPut, '\0', 100);
		strncpy(tempPut, &request[5], 99);

		char* newPost;
		newPost = strtok(tempPut, "/");

		char post[20];
		memset(post, '\0', 20);

		char author[20];
		memset(author, '\0', 20);

		char title[20];
		memset(title, '\0', 20);

		char topic[20];
		memset(topic, '\0', 20);*/


		//int count = 1;
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

		//if the variable doesnt equal "unchanged", then set the new value

		//printf("\nPost: %d Author: %s, Topic: %s, Title: %s\n",atoi(post), author, topic, title);

		//createPayload(buffer);
		int number = atoi( post);
		putOne(author, topic, title, &postings[number - 1]);

		getAll(buffer, postings);

		break;
	case 4:
		printf("The DELETE was called");
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

		//printf("\nPost: %d\n", atoi(post));

		//createPayload(buffer);

		int deleteNum = atoi(post) - 1;
		deleteOne(postings, deleteNum);

		getAll(buffer, postings);

		break;
	default:
	
		printf("default");
		break;
	
	}
	
	/*char* get = "GET\n";

	if (strcmp(request, get) == 0)
	{
		createGetPayload(buffer);
	}
	else
	{
		createPayload(buffer);
	}*/



	printf("Sending response...\n");
	int bytes_sent = send(socket_client, buffer, strlen(buffer), 0);
	printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(buffer));
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

void parse(char buffer[])
{
	char author[] = "";
	char title[] = "";
	char topic[] = "";
	int postNum = 0;

	const char delimiterSpace = ' ';
	char delimSlash = "/";
	char delimAmpersand = "&";
	char delimQuestion = "?";


	
		char* firstWord = "";

		firstWord = strtok(buffer, " ");

		char* get = "GET";

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
		
		char tempString[SENDBUFFERSIZE];
		memset(tempString, '\0', SENDBUFFERSIZE);
		snprintf(tempString, SENDBUFFERSIZE, "Post: %d Author: %s Topic: %s Title: %s \n", postings[i].postNum, postings[i].author, postings[i].topic, postings[i].title);
		strcat(buffer, tempString);
		
	}


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

void postOne(char* author, char* topic, char* title, LIST postings[])
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
}

void putOne(char* author, char* topic, char* title, LIST* postings)
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

}


void deleteOne(LIST postings[], int num)
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
				//memcpy(&postings[i], &postings[i + 1], sizeof(LIST));

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

}