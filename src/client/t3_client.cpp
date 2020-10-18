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
#include "t3_client.h"
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
void handleMessage(SOCKET ConnectSocket, std::string recvbuf, int recvbuflen); // handle messages received from TCP-packets

#define DEFAULT_BUFLEN 3
#define DEFAULT_PORT "27015"
const char *serverAddress = "217.215.208.19";
//const char *serverAddress = "127.0.0.1";

extern char sendbuf[3];

int t3::init_client(void)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	//const char *sendbuf = "this is a test it is very nice";
	//const char *sendbuf = "I like coffee";
	
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;


	// Validate the parameters
	// input: ip-address of server
	/*
	if (argc != 2) {
	printf("usage: %s server-name\n", argv[0]);
	char p = getchar(); //just to stop prompt from closing
	return 1;
	}
	serverAddress = argv[1];
	*/

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		char p = getchar(); //just to stop prompt from closing
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	//iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	iResult = getaddrinfo(serverAddress, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		char p = getchar(); //just to stop prompt from closing
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			char p = getchar(); //just to stop prompt from closing
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		char p = getchar(); //just to stop prompt from closing
		return 1;
	}
	printf("Sendbuf address %p\n", sendbuf);


	// Define a lamda expression 
	auto sending_thread = [ConnectSocket]() {
		int iResult;
		printf("Sendbuf address %p\n", sendbuf);
		while (1) {
			std::chrono::milliseconds timespan(16);
			std::this_thread::sleep_for(timespan);
			// Send an initial buffer
			iResult = send(ConnectSocket, sendbuf, (int)sizeof(sendbuf), 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				char p = getchar(); //just to stop prompt from closing
				return 1;
			}

			printf("Bytes Sent: %ld\n", iResult);
		}
	};

	// Pass sending_thread and its parameters to thread  
	// object constructor as 
	std::thread thread_sender(sending_thread);



	// Define a lamda expression 
	auto receiving_thread = [ConnectSocket]() {
		int iResult;
		// Wait for response
		int recvbuflen = DEFAULT_BUFLEN;
		char recvbuf[DEFAULT_BUFLEN];
		
		do {
			std::chrono::milliseconds timespan(16);
			std::this_thread::sleep_for(timespan);
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				printf("Bytes received: %d | %04X %04X %04X\n", iResult, recvbuf[0], recvbuf[1], recvbuf[2]);
				// handle message
				handleMessage(ConnectSocket, recvbuf, recvbuflen);
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
	

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		char p = getchar(); //just to stop prompt from closing
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	char p = getchar(); //just to stop prompt from closing

	return 0;
}


namespace t3 { extern void(*receive_callback)(char*, int); };


static void handleMessage(SOCKET ConnectSocket, std::string recvbuf, int recvbuflen) {
	std::cout << "Recieved this message: " << recvbuf << std::endl;

	t3::receive_callback((char*)&recvbuf, recvbuflen);
}

/*
void t3::register_receive_callback(void(*callback)(char*, int)) {
	receive_callback = callback;
}*/