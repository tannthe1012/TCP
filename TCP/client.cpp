#include "stdafx.h"
#include "stdio.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "string"
#include <stdlib.h>
#define BUFF_SIZE 2048
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
int SERVER_PORT;
char *SERVER_ADDR;
WSADATA wsaData;
WORD wVersion = MAKEWORD(2, 2);
void InputCommandLineArgument(int argc, char *argv[]) {
	if (argc < 3)
		printf("ERROR INPUT");
	SERVER_PORT = atoi(argv[2]);
	SERVER_ADDR = argv[1];
}
void InitiateWinSock() {
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
	}
}
int main(int argc, char* argv[])
{
	InputCommandLineArgument(argc, argv);
	InitiateWinSock();
	//Construct socket	
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	//Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	//Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("Error %d: cannot connect server. ", WSAGetLastError());
		return 0;
	}
	printf("Connected server!");

	//Communicate with server
	char buff[BUFF_SIZE];
	int ret, messageLen;
	bool flag = true;


	while (1)
	{
		//Send message 
		printf("\nSend to server: ");
		gets_s(buff, BUFF_SIZE);
		messageLen = strlen(buff);
		if (messageLen == 0) break;
		ret = send(client, buff, messageLen, 0);
		if (ret == SOCKET_ERROR)
			printf("Error %d: Cannot send data.", WSAGetLastError());
		
		//Receive echo message
		ret = recv(client, buff, BUFF_SIZE, 0);

		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot receive message.", WSAGetLastError());
		}
		else {
			// format data
			buff[ret]=0;
			printf("Receive from server: \n%s\n", buff);
		}
		
	} //end while

	  //Step 5: Close socket
	closesocket(client);

	//Step 6: Teminate Winsock
	WSACleanup();

	return 0;

}