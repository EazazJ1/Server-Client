//CSCN72020-F21
//Assignment 3
//
//Eazaz Jakda
//
//December 10. 2021
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x6000
#endif


#include <WinSock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <stdbool.h>




#define DISPLAYBUFFERSIZE	100
#define SENDBUFFERSIZE	1000
#define MAXLISTENERS		10
#define MAXLEN 50
#define MAXPOSTINGS 10
#define MAXSTRINGLEN 100

typedef struct list {
	int postNum;
	char author[MAXLEN];
	char title[MAXLEN];
	char topic[MAXLEN];
	bool set;

} LIST;

typedef enum proto { UDP, TCP } PROTOCOL;

//common
void InitializeWindowsSockets();
void CloseSocketConnection(SOCKET);
void ShutdownWindowsSockets();

//server only
struct addrinfo* ConfigureLocalAddress(char*, PROTOCOL);
SOCKET CreateBindListeningSocket(struct addrinfo*);
void StartListeningForConnections(SOCKET);
SOCKET WaitForAndAcceptConnection(SOCKET);
void RecvRequestAndSendResponse(SOCKET, LIST postings[]);
void RecvUDPRequestAndSendResponse(SOCKET);

//client only
struct addrinfo* ConfigureRemoteAddress(char*, char*, PROTOCOL);
SOCKET CreateAndConnectRemoteSocket(struct addrinfo*);




void InitList(LIST postings[]);

void getAll(char* buffer, LIST postings[]);
void parseFilter(char* buffer, char* request, LIST postings[]);
void getFilterStrings(char* buffer, char* request, LIST postings[], char* str[]);
void getOne(char* buffer, LIST postings);

void postOne(char* buffer, char* author, char* topic, char* title, LIST postings[]);

void putOne(char* buffer, char* author, char* topic, char* title, LIST* postings);

void deleteOne(char* buffer, LIST postings[], int num);

int OperationNum(char firstword[]);



