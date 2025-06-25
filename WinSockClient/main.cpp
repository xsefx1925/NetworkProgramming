#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>
#include<stdio.h>
#include<iostream>
#include<FormatLastError.h>


using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "FormatLastError.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 1500
CONST CHAR g_OVERFLOW[DEFAULT_BUFFER_LENGTH] = "Sorry, too many connection, try again later.";

//LPSTR FormatLastError(DWORD dwMessageID);
//VOID PrintLastError(DWORD dwMessageID);

void main()
{
	setlocale(LC_ALL, "Russian");
	cout << "WinSock Client" << endl;

	//1) Инициализиация WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup() failed with code " << iResult << endl;
		return;
	}
	//2)Cоздаем ClientSocket:
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //INET - TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//3 Определяем IP-адрес сервера:
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult)
	{
		cout << "getaddressinfo() failed with code " << iResult << endl;
		WSACleanup();
		return;
	}
	//cout << "hinsts:" << endl;
	//cout << "ai_addr:" << hints.ai_addr->sa_data << endl;

	SOCKET connect_socket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

	if (connect_socket == INVALID_SOCKET)
	{
		DWORD dwMessageID = WSAGetLastError();
		//cout << "Error: socket creatin failed with code: " << dwMessageID << ":\t";
		LPSTR szBuffer = NULL;

		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//4 Подключаемся к серверу:
	iResult = connect(connect_socket, result->ai_addr, result->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		
		PrintLastError(WSAGetLastError());
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//5)Отправка и получение данных с сервера:
    CHAR sendbuffer[DEFAULT_BUFFER_LENGTH] = "Hello Server, I am client";
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTH] = {};
	do
	{
	//
	   iResult = send(connect_socket, sendbuffer, strlen(sendbuffer), 0);
	   if (iResult == SOCKET_ERROR)
	   {
		PrintLastError(WSAGetLastError());
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	   }
	   ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTH);
		iResult = recv(connect_socket, recvbuffer, DEFAULT_BUFFER_LENGTH, 0);
		if (iResult > 0)cout << "Receved bytes: " << iResult << ", Message: " << recvbuffer << endl;
		else if (iResult == 0)cout << "Connection closing" << endl;
		else PrintLastError(WSAGetLastError());
		if (strcmp(recvbuffer, g_OVERFLOW) == 0)break;
		{
			system("PAUSE");
			break;
		}
		cout << "Введите сообщение: ";
		ZeroMemory(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(1251);
		cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(866);
	} while (iResult > 0 && strcmp(sendbuffer, "exit"));
    //   }while (true);
	//6) Закрываем соединение:
	iResult = shutdown(connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
		PrintLastError(WSAGetLastError());
	//7) Освобождаем ресурсы WinSock:
	closesocket(connect_socket);
	FreeAddrInfo(result); 
		WSACleanup();
}

VOID Receive(SOCKET connect_socket)
{

}