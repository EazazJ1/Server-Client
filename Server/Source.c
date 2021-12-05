// cscn72020f21 - a new and improved (using library v2) tcp server.
// steveh - nov 2021
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetworkingFunctions.h"
//#include "ServerFunctions.h"

//#define MAXLEN 50
#define LISTENINGPORT   "8080"      //needs to be a string

int main(void)
{
    LIST postings[10];
    
    InitList(postings);
    
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
    return 0;
}