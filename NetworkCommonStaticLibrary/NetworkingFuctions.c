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

void RecvRequestAndSendResponse(SOCKET socket_client)
{
	printf("Reading request...\n");
	char request[SENDBUFFERSIZE];
	memset(request, '\0', SENDBUFFERSIZE);
	int bytes_received = recv(socket_client, request, SENDBUFFERSIZE, 0);
	printf("Received %d bytes: \n", bytes_received);
	printf("%.*s\n", bytes_received, request);      //the %.*s makes sure we print exactly as many chars as was received (regardless of null termination)

	printf("building response\n");
	char buffer[SENDBUFFERSIZE];

	//char get[] = "GET\n";

	char* get = "GET\n";

	if (strcmp(request, get) == 0)
	{
		createGetPayload(buffer);
	}
	else
	{
		createPayload(buffer);
	}



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
