#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <winsock2.h>
#include <string>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define DESTPORT 80
class Client {
public:
	int err;
	char* msg;
	struct hostent *host;


	SOCKET connSocket;
	SOCKADDR_IN sockAddr;

	void Stop();
	bool Start();
	int sendMessage(SOCKET socket, char* message, int messageSize);

};

int Client::sendMessage(SOCKET socket, char* message, int messageSize) {
	return send(socket, message, messageSize, 0);
}

//int networkService::receiveMessage(SOCKET socket, char* buffer, int bufferSize) {
//	return recv(socket, buffer, bufferSize, 0);
//}

bool Client::Start() {

	WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err!=0){
		cout << "WSAStartup failed." << endl;
		return false;
	}

	cout << "[+] WSAStartup Successed." << endl;

	connSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if ((host = (hostent*)gethostbyname("localhost")) == NULL) {
		cout << "[-] Failed to resolve hostname." << endl;
		WSACleanup();
		return false;
	}

	cout << "[+] Resolving hostname Successed." << endl;

	// setting details.
	sockAddr.sin_port = htons(DESTPORT);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	// connecting.
	err = connect(connSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));

	if (err) {
		cout << "[-] Failed to connect with the server." << endl;
		WSACleanup();
		return false;
	}

	cout << "[+] Connect to the server successed." << endl;
	
	return true;
}
void Client::Stop() {
	cout << "[-] Lost connection, Failed to send last request." << endl;
	closesocket(connSocket);
	WSACleanup();
}



void Network() {
	Client SessionOne;
	SessionOne.Start();
	SessionOne.err = WSAGetLastError();
	SessionOne.msg = (char*)malloc(1024);

	while (SessionOne.err == 0){
		cin >> SessionOne.msg;
		SessionOne.sendMessage(SessionOne.connSocket, SessionOne.msg, strlen(SessionOne.msg));
		SessionOne.err = WSAGetLastError();
	}

	SessionOne.Stop();
}
void main() {
	Network();
}