#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "t3_server.h"
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
void handleMessage(SOCKET ConnectSocket, std::string recvbuf, int recvbuflen); // handle messages received from TCP-packets

#define DEFAULT_BUFLEN 3
#define DEFAULT_PORT "27015"

static bool active = false;

char sendbuf[3] = { 0 };
static bool next = true;

static int recvbuflen = DEFAULT_BUFLEN;
static char recvbuf[DEFAULT_BUFLEN];

int t3::init_server(void) //Start on own thread
{
	if (active)return -1;
	active = true;

	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;


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
		printf("Listening to socket...\n");

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}
		printf("Accepted client!\n");

		// No longer need server socket
		closesocket(ListenSocket);


		printf("Sendbuf address %p\n", sendbuf);

		// Define a lamda expression 
		auto sending_thread = [ClientSocket]() {
			int iResult;

			printf("Sendbuf address %p\n", sendbuf);
			while (1) {
				if (!next)continue;
				next = false;
				std::chrono::milliseconds timespan(16);
				std::this_thread::sleep_for(timespan);
				// Send an initial buffer
				iResult = send(ClientSocket, sendbuf, (int)sizeof(sendbuf), 0);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					char p = getchar(); //just to stop prompt from closing
					return 1;
				}
				printf("Bytes sent: %d | %04X %04X %04X\n", iResult, sendbuf[0], sendbuf[1], sendbuf[2]);
			}
		};

		// Pass sending_thread and its parameters to thread  
		// object constructor as 
		std::thread thread_sending(sending_thread);



		// Define a lamda expression 
		auto receiving_thread = [ClientSocket]() {
			int iResult;
			// Wait for response


			do {
				std::chrono::milliseconds timespan(16);
				std::this_thread::sleep_for(timespan);
				iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
				if (iResult > 0) {
					printf("Bytes received: %d | %04X %04X %04X\n", iResult, recvbuf[0], recvbuf[1], recvbuf[2]);
					// handle message
					handleMessage(ClientSocket, recvbuf, recvbuflen);
				}
				else if (iResult == 0)
					printf("Connection closed\n");
				else
					printf("recv failed with error: %d\n", WSAGetLastError());

			} while (iResult > 0);

		};

		// Pass receiving_thread and its parameters to thread  
		// object constructor as 
		std::thread thread_receiver(receiving_thread);

		thread_receiver.join();

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

void t3::sendData(char* data, int size)
{
	//server send stuff
	for (int i = 0; i < 3; i++) {
		sendbuf[i] = data[i];
	}
}

namespace t3 { void(*receive_callback)(char*, int); };

void handleMessage(SOCKET ConnectSocket, std::string recvbuf, int recvbuflen)
{
	t3::receive_callback((char*)&recvbuf, recvbuflen);
	next = true;
}


void t3::register_receive_callback(void(*callback)(char*, int)) {
	t3::receive_callback = callback;
}