//CSCN72020-F21
//Assignment 3
//
//Eazaz Jakda
//
//December 10. 2021
//


#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "NetworkingFunctions.h"
#include "ServerFunctions.h"

//#define MAXLEN 50
#define LISTENINGPORT   "8080"      //needs to be a string

int main(void)
{
    LIST postings[10];

    load(postings);
    
    //InitList(postings);           //To set the list of posting to nothing
    
    InitializeWindowsSockets();

    printf("Config the local addr...\n");
    struct addrinfo* bind_address = ConfigureLocalAddress(LISTENINGPORT, TCP);

    printf("Creating socket...\n");
    SOCKET socket_listen = CreateBindListeningSocket(bind_address);

    printf("Start listening...\n");
    StartListeningForConnections(socket_listen);

    printf("Waiting for connection...\n");
    SOCKET socket_client = WaitForAndAcceptConnection(socket_listen);

    printf("Client is connected...");
    RecvRequestAndSendResponse(socket_client, postings);

    printf("Closing the connection...\n");
    CloseSocketConnection(socket_client);

    printf("Closing listening socket...\n");
    CloseSocketConnection(socket_listen);

    ShutdownWindowsSockets();

    save(postings);
    return 0;
}