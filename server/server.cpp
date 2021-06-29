
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdio.h"
#include <stdlib.h>
#include "winsock2.h"
#include "ws2tcpip.h"
#include <iostream>
#include <string.h>
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
WSADATA wsaData;
WORD wVersion = MAKEWORD(2, 2);
int SERVER_PORT;
void filterData(int ret, char *buff, char result[]) {
	char a[100] = "";
	char str[100] = ""; //array containing characters
	char number[100] = ""; //array containing number
	int numberchar = 0; //number of characters
	int m = 0; //number of numbers
	for (int i = 0; i < ret; i++) {
		// Filter the data
		if (buff[i] >= 48 && buff[i] <= 57) {
			number[m] = buff[i];
			m++;
		}
		else if ((buff[i] >= 65 && buff[i] <= 90) || (buff[i] >= 97 && buff[i] <= 122)) {
			str[numberchar] = buff[i];
			numberchar++;
		}
		else {
			a[0] = '-';
			break;
		}

	}
	if (result[0] != '-') {
		strcat_s(a, "+\n");
		strcat_s(a, str);
		strcat_s(a, "\n");
		strcat_s(a, number);
	}
	int stringlen = strlen(a);
	for (int i = 0; i < stringlen; i++) {
		result[i] = a[i];
	}
}
//Initiate WinSock
void InitiateWinSock() {
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
	}
}
//Input name and port
void InputCommandLineArgument(int argc, char *argv[]) {
	if (argc < 2)	
		printf("ERROR INPUT");
	SERVER_PORT = atoi(argv[1]);
}


int main(int argc, char* argv[])
{
	InputCommandLineArgument(argc,argv);
	InitiateWinSock();
	//Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}
	//Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}
	//Listen request from client
	if (listen(listenSock, 10)) {
		printf("(Error: %d)Cannot associate a local address with server socket", WSAGetLastError());
		return 0;
	}

	printf("Server started!:");

	//communicate with client
	sockaddr_in clientAddr;
	char buff[BUFF_SIZE], clientIP[INET_ADDRSTRLEN];
	int ret, clientAddrLen = sizeof(clientAddr), clientPort;
	SOCKET connectSock;
	//accept request
	connectSock = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);
	if (connectSock == SOCKET_ERROR) {
		printf("(Error: %d)Cannot permit incoming connection", WSAGetLastError());
		return 0;
	}
	else {
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
		clientPort = ntohs(clientAddr.sin_port);
		printf("Accept incoming connection from %s:%d\n", clientIP, clientPort);
	}



	while (1)
	{
		
		ret = recv(connectSock, buff, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: cannot receive data. but still works \n", WSAGetLastError());
			connectSock = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);
			continue;
		}
		else if (ret == 0) {
			printf("Client disconnects. \n");
			connectSock = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);
			continue;
		}
		else {
			buff[ret] = 0;
			printf("Reveive from client[%s:%d] %s\n", clientIP, clientPort, buff);
			//Echo to client
			char result[100] = "";
			// Filter data
			filterData(ret, buff, result);
			ret = send(connectSock, result, strlen(result), 0);
			if (ret == SOCKET_ERROR) {
				printf("Error %d: cannot receive data. ", WSAGetLastError());
				break;
			}
		}
	} //end while

	  //Step 6: Close socket
	closesocket(listenSock);
	closesocket(connectSock);

	//Step 7: Teminate Winsock
	WSACleanup();

	return 0;

}