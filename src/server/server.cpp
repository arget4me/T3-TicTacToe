#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
void handleMessage(SOCKET ConnectSocket, std::string recvbuf, int recvbuflen); // handle messages received from TCP-packets

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		char p = getchar(); //just to stop prompt from closing
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	printf("Server is running!\n");
	while (1) {
		// Resolve the server address and port
		iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}

		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}

		freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}

		// No longer need server socket
		closesocket(ListenSocket);

		// Receive until the peer shuts down the connection
		do {

			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				//printf("Bytes received: %d\n", iResult);
				//printf("Buffer received: %s\n", recvbuf);
				handleMessage(ClientSocket, recvbuf, recvbuflen);

				
				// Echo the buffer back to the sender
				const char *sendAnswer = "I got your message, Bye bye!";
				iSendResult = send(ClientSocket, sendAnswer, (int)strlen(sendAnswer)+1, 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					char p = getchar(); //just to stop prompt from closing
					return 1;
				}
				printf("Bytes sent: %d\n", iSendResult);
			}
			else if (iResult == 0)
				//printf("Connection closing...\n");
				printf("Recieved all data!\n");
			else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				char p = getchar(); //just to stop prompt from closing
				return 1;
			}

		} while (iResult > 0);



		// shutdown the connection since we're done
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}
	}

	printf("Server closing...");

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	char p = getchar(); //just to stop prompt from closing


	return 0;
}


void handleMessage(SOCKET ConnectSocket, std::string recvbuf, int recvbuflen) {
	std::cout << "Recieved this message: " << recvbuf << std::endl;

	if (recvbuf.compare("I like coffee") == 0) {
		std::cout << "The client apparently likes coffee..." << std::endl;
	}
	if (recvbuf.compare("I want meatballs") == 0) {
		std::cout << "The client is hungry and wants some meatballs" << std::endl;
	}
}