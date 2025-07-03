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
		//cout << "Error: socket creatin failed with code: " << dwMessageID << ":\t"
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
		if (strcmp(recvbuffer, g_OVERFLOW) == 0)
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

SOCKET g_connect_socket = INVALID_SOCKET; // Глобальная переменная для сокета клиента

// Прототипы функций потоков
DWORD WINAPI SendThread(LPVOID lpParam);
DWORD WINAPI ReceiveThread(LPVOID lpParam);

void main()
{
	setlocale(LC_ALL, "Russian");
	cout << "WinSock Client" << endl;

	// 1) Инициализация WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup() failed with code " << iResult << endl;
		return;
	}

	// 2) Создаем ClientSocket:
	addrinfo* result = NULL;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // INET - TCP/IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// 3) Определяем IP-адрес сервера:
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
		PrintLastError(WSAGetLastError()); // Использование PrintLastError
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// 4) Подключаемся к серверу:
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

	// 5) Отправка и получение данных с сервера в отдельных потоках:
	HANDLE hSendThread = CreateThread(
		NULL,
		0,
		SendThread,
		NULL, // Теперь сокет глобальный
		0,
		NULL
	);

	HANDLE hReceiveThread = CreateThread(
		NULL,
		0,
		ReceiveThread,
		NULL, // Теперь сокет глобальный
		0,
		NULL
	);

	
	HANDLE hThreads[] = { hSendThread, hReceiveThread };
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE); // Ждем завершения обоих потоков

	// 6) Закрываем соединение:
	iResult = shutdown(g_connect_socket, SD_SEND);
	if (iResult == SOCKET_ERROR)
		PrintLastError(WSAGetLastError());

	// 7) Освобождаем ресурсы WinSock:
	closesocket(g_connect_socket);
	FreeAddrInfo(result);
	WSACleanup();

	cout << "Client disconnected." << endl;
	system("PAUSE"); // Чтобы консольное окно не закрывалось сразу
}

// Поток для отправки данных
DWORD WINAPI SendThread(LPVOID lpParam)
{
	CHAR sendbuffer[DEFAULT_BUFFER_LENGTH];
	INT iResult;


	strcpy_s(sendbuffer, sizeof(sendbuffer), "Hello Server, I am client");
	iResult = send(g_connect_socket, sendbuffer, strlen(sendbuffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		PrintLastError(WSAGetLastError());
		return 1; // Завершение потока с ошибкой
	}

	do
	{
		cout << "Введите сообщение: ";
		ZeroMemory(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(1251); // Установка кодировки для ввода
		cin.getline(sendbuffer, DEFAULT_BUFFER_LENGTH);
		SetConsoleCP(866); // Возврат к системной кодировке

		if (strcmp(sendbuffer, "exit") == 0)
		{
			// Сообщаем серверу о выходе перед завершением
			send(g_connect_socket, sendbuffer, strlen(sendbuffer), 0);
			break;
		}

		iResult = send(g_connect_socket, sendbuffer, strlen(sendbuffer), 0);
		if (iResult == SOCKET_ERROR)
		{
			PrintLastError(WSAGetLastError());
			break;
		}

	} while (true); // Бесконечный цикл, прерывается при вводе "exit" или ошибке

	cout << "Send thread exiting." << endl;
	return 0;
}

// Поток для получения данных
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
			cout << "Получено байт: " << iResult << ", Сообщение: " << recvbuffer << endl;
			if (strcmp(recvbuffer, g_OVERFLOW) == 0)
			{
				cout << "Сервер переполнен. Отключение..." << endl;
				system("PAUSE");
				break; // Выход из цикла, так как соединение будет закрыто
			}
		}
		else if (iResult == 0)
		{
			cout << "Соединение закрывается сервером." << endl;
			break; // Соединение закрыто
		}
		else
		{
			PrintLastError(WSAGetLastError());
			break; // Ошибка получения
		}
	} while (true);


	cout << "Receive thread exiting." << endl;
	return 0;
}