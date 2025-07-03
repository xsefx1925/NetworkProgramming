/*

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

	//1) �������������� WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup() failed with code " << iResult << endl;
		return;
	}
	//2)C������ ClientSocket:
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; //INET - TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//3 ���������� IP-����� �������:
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
		//cout << "Error: socket creatin failed with code: " << dwMessageID << ":\t"
		LPSTR szBuffer = NULL;

		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//4 ������������ � �������:
	iResult = connect(connect_socket, result->ai_addr, result->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		
		PrintLastError(WSAGetLastError());
		closesocket(connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	//5)�������� � ��������� ������ � �������:
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
		if (strcmp(recvbuffer, g_OVERFLOW) == 0)
		{
			system("PAUSE");
			break;
		}
		cout << "������� ���������: ";
		ZeroMemory(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(1251);
		cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(866);
	} while (iResult > 0 && strcmp(sendbuffer, "exit"));
    //   }while (true);
	//6) ��������� ����������:
	iResult = shutdown(connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
		PrintLastError(WSAGetLastError());
	//7) ����������� ������� WinSock:
	closesocket(connect_socket);
	FreeAddrInfo(result); 
		WSACleanup();
}

VOID Receive(SOCKET connect_socket)
{

}
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <FormatLastError.h> 

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "FormatLastError.lib") 

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 1500
CONST CHAR g_OVERFLOW[DEFAULT_BUFFER_LENGTH] = "Sorry, too many connection, try again later.";

SOCKET g_connect_socket = INVALID_SOCKET; // ���������� ���������� ��� ������ �������

// ��������� ������� �������
DWORD WINAPI SendThread(LPVOID lpParam);
DWORD WINAPI ReceiveThread(LPVOID lpParam);

void main()
{
	setlocale(LC_ALL, "Russian");
	cout << "WinSock Client" << endl;

	// 1) ������������� WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup() failed with code " << iResult << endl;
		return;
	}

	// 2) ������� ClientSocket:
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // INET - TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// 3) ���������� IP-����� �������:
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult)
	{
		cout << "getaddrinfo() failed with code " << iResult << endl;
		WSACleanup();
		return;
	}

	g_connect_socket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

	if (g_connect_socket == INVALID_SOCKET)
	{
		PrintLastError(WSAGetLastError()); // ������������� PrintLastError
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// 4) ������������ � �������:
	iResult = connect(g_connect_socket, result->ai_addr, result->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		PrintLastError(WSAGetLastError());
		closesocket(g_connect_socket);
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	cout << "Connected to server." << endl;

	// 5) �������� � ��������� ������ � ������� � ��������� �������:
	HANDLE hSendThread = CreateThread(
		NULL,
		0,
		SendThread,
		NULL, // ������ ����� ����������
		0,
		NULL
	);

	HANDLE hReceiveThread = CreateThread(
		NULL,
		0,
		ReceiveThread,
		NULL, // ������ ����� ����������
		0,
		NULL
	);

	
	HANDLE hThreads[] = { hSendThread, hReceiveThread };
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE); // ���� ���������� ����� �������

	// 6) ��������� ����������:
	iResult = shutdown(g_connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
		PrintLastError(WSAGetLastError());

	// 7) ����������� ������� WinSock:
	closesocket(g_connect_socket);
	FreeAddrInfo(result);
	WSACleanup();

	cout << "Client disconnected." << endl;
	system("PAUSE"); // ����� ���������� ���� �� ����������� �����
}

// ����� ��� �������� ������
DWORD WINAPI SendThread(LPVOID lpParam)
{
	CHAR sendbuffer[DEFAULT_BUFFER_LENGTH];
	INT iResult;


	strcpy_s(sendbuffer, sizeof(sendbuffer), "Hello Server, I am client");
	iResult = send(g_connect_socket, sendbuffer, strlen(sendbuffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		PrintLastError(WSAGetLastError());
		return 1; // ���������� ������ � �������
	}

	do
	{
		cout << "������� ���������: ";
		ZeroMemory(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(1251); // ��������� ��������� ��� �����
		cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(866); // ������� � ��������� ���������

		if (strcmp(sendbuffer, "exit") == 0)
		{
			// �������� ������� � ������ ����� �����������
			send(g_connect_socket, sendbuffer, strlen(sendbuffer), 0);
			break;
		}

		iResult = send(g_connect_socket, sendbuffer, strlen(sendbuffer), 0);
		if (iResult == SOCKET_ERROR)
		{
			PrintLastError(WSAGetLastError());
			break;
		}

	} while (true); // ����������� ����, ����������� ��� ����� "exit" ��� ������

	cout << "Send thread exiting." << endl;
	return 0;
}

// ����� ��� ��������� ������
DWORD WINAPI ReceiveThread(LPVOID lpParam)
{
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTH] = {};
	INT iResult;

	do
	{
		ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTH);
		iResult = recv(g_connect_socket, recvbuffer, DEFAULT_BUFFER_LENGTH, 0);

		if (iResult > 0)
		{
			cout << "�������� ����: " << iResult << ", ���������: " << recvbuffer << endl;
			if (strcmp(recvbuffer, g_OVERFLOW) == 0)
			{
				cout << "������ ����������. ����������..." << endl;
				system("PAUSE");
				break; // ����� �� �����, ��� ��� ���������� ����� �������
			}
		}
		else if (iResult == 0)
		{
			cout << "���������� ����������� ��������." << endl;
			break; // ���������� �������
		}
		else
		{
			PrintLastError(WSAGetLastError());
			break; // ������ ���������
		}
	} while (true);


	cout << "Receive thread exiting." << endl;
	return 0;
}