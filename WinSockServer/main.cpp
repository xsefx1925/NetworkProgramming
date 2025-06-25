#define _CRT_SECURE_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !#define WIN32_LEAN_AND_MEAN


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

#define DEFAULT_PORT			"27015"
#define DEFAULT_BUFFER_LENGTH	1500

CONST CHAR g_OVERFLOW[DEFAULT_BUFFER_LENGTH] = "Sorry, too many connection, try again later.";

CONST INT MAX_CONNECTIONS = 3;
SOCKET sockets[MAX_CONNECTIONS] = {};
DWORD dwThreadIDs[MAX_CONNECTIONS] = {};
HANDLE hThreads[MAX_CONNECTIONS] = {};

VOID ClientHandler(SOCKET client_socket);


void main()
{
	setlocale(LC_ALL, "");

	//1) ������������� WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		PrintLastError(WSAGetLastError());
		return;
	}

	//2) ���������, �� ����� �� ������ ��� ����:
	addrinfo* result = NULL;
	addrinfo  hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo() failed with ";
		PrintLastError(WSAGetLastError());
		WSACleanup();
		return;
	}

	//3) ������� �����, ������� ����� ������� � ������� ����������� �� ��������:
	SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << "socket() failed with ";
		PrintLastError(WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//4) BindSocket - ��������� ����� � ������� IP-����� � ������:
	iResult = bind(listen_socket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "bind() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//5) �������� ������������� ������:
	iResult = listen(listen_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		cout << "listen() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//6) ��������� ������� �� ���������� �� ��������:

	INT i = 0;
	cout << "Waiting for clients..." << endl;
	do
	{
		SOCKET client_socket = accept(listen_socket, NULL, NULL);
		//sockets[i] = client_socket;
		if (client_socket == INVALID_SOCKET)
		{
			cout << "accept() failed with ";
			PrintLastError(WSAGetLastError());
			closesocket(listen_socket);
			freeaddrinfo(result);
			WSACleanup();
			return;
		}

		if (i < MAX_CONNECTIONS)
		{
			sockets[i] = client_socket;
		    hThreads[i] = CreateThread
		  (
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ClientHandler,
			(LPVOID)sockets[i],
			0,
			&dwThreadIDs[i]
		  );
		  i++;
	    }
		else
		{
			//CHAR send_buffer[DEFAULT_BUFFER_LENGTH] = "Sorry, too many connection, try again later.";
			CHAR recv_buffer[DEFAULT_BUFFER_LENGTH] = {};
			INT iResult = recv(client_socket, recv_buffer, DEFAULT_BUFFER_LENGTH, 0);
			cout << "ExtraClient sends: " << recv_buffer << endl;
			send(client_socket, g_OVERFLOW, strlen(g_OVERFLOW), 0);
			closesocket(client_socket);

		}
	//	ClientHandler(client_socket);
		//CreateThread()
	} while (true);
	WaitForMultipleObjects(MAX_CONNECTIONS, hThreads, TRUE, INFINITE);
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		CloseHandle(hThreads[i]);
		closesocket(sockets[i]);
	}
	//? ������������ �e�y���� WinSock:
	closesocket(listen_socket);
	freeaddrinfo(result);
	WSACleanup();
}
VOID ClientHandler(SOCKET client_socket)
{
	INT iResult = 0;
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTH] = {};
	do
	{
		ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTH);
		iResult = recv(client_socket, recvbuffer, DEFAULT_BUFFER_LENGTH, 0);
		if (iResult > 0)
		{
			cout << "Received Bytes: " << iResult << ", Message: " << recvbuffer << endl;
			for (int i = 0; i < MAX_CONNECTIONS; i++)
			{
				if (sockets[i])
				{
					if (send(sockets[i], recvbuffer, strlen(recvbuffer), 0) == SOCKET_ERROR)
					{
						cout << "send() failed with ";
						PrintLastError(WSAGetLastError());
						break;
					}
				}
				
			}
			
		}
		else if (iResult == 0)cout << "Connection closing..." << endl;
		else
		{
			cout << "recv() failed with ";
			PrintLastError(WSAGetLastError());
		}
	} while (iResult > 0);
	closesocket(client_socket);

}