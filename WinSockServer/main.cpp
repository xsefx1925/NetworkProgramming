/*
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

	//1) Инициализация WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		PrintLastError(WSAGetLastError());
		return;
	}

	//2) Проверяем, не занят ли нужный нам порт:
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

	//3) Создаем Сокет, который будет слушать и ожидать подключения от клиентов:
	SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << "socket() failed with ";
		PrintLastError(WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//4) BindSocket - связываем Сокет с целевым IP-адрес и портом:
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

	//5) Включаем прослушивание Сокета:
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

	//6) Принимаем запросы на соединение от клиентов:

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
	//? Освобождение рeсyрсов WinSock:
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

*/
#define _CRT_SECURE_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <vector> 
#include <string>  
#include <FormatLastError.h> 
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "FormatLastError.lib") 

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH 1500

CONST CHAR g_OVERFLOW[DEFAULT_BUFFER_LENGTH] = "Sorry, too many connection, try again later.";

CONST INT MAX_CONNECTIONS = 3;

CRITICAL_SECTION g_csSocketList; 
vector<SOCKET> g_client_sockets;

// Структура для передачи информации клиенту в поток ClientHandler
struct ClientInfo {
	SOCKET client_socket;
	sockaddr_in client_addr;
};

// Прототипы функций
DWORD WINAPI ClientHandler(LPVOID lpParam);
void BroadcastMessage(const char* message, SOCKET sender_socket = INVALID_SOCKET);

void main()
{
	setlocale(LC_ALL, "");

	// Инициализация критической секции
	InitializeCriticalSection(&g_csSocketList);

	// 1) Инициализация WinSock:
	WSADATA wsaData;
	INT iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		PrintLastError(WSAGetLastError());
		DeleteCriticalSection(&g_csSocketList);
		return;
	}

	// 2) Проверяем, не занят ли нужный нам порт:
	addrinfo* result = NULL;
	addrinfo hints;
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
		DeleteCriticalSection(&g_csSocketList);
		return;
	}

	// 3) Создаем Сокет, который будет слушать и ожидать подключения от клиентов:
	SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << "socket() failed with ";
		PrintLastError(WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		DeleteCriticalSection(&g_csSocketList);
		return;
	}

	// 4) BindSocket - связываем Сокет с целевым IP-адресом и портом:
	iResult = bind(listen_socket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "bind() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		DeleteCriticalSection(&g_csSocketList);
		return;
	}

	// 5) Включаем прослушивание Сокета:
	iResult = listen(listen_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		cout << "listen() failed with ";
		PrintLastError(WSAGetLastError());
		closesocket(listen_socket);
		freeaddrinfo(result);
		WSACleanup();
		DeleteCriticalSection(&g_csSocketList);
		return;
	}

	cout << "Waiting for clients..." << endl;
	// 6) Принимаем запросы на соединение от клиентов:
	do
	{
		sockaddr_in client_addr;
		int client_addr_len = sizeof(client_addr);
		SOCKET client_socket = accept(listen_socket, (sockaddr*)&client_addr, &client_addr_len);

		if (client_socket == INVALID_SOCKET)
		{
			cout << "accept() failed with ";
			PrintLastError(WSAGetLastError());
		
			continue;
		}

		EnterCriticalSection(&g_csSocketList); // Защита доступа к g_client_sockets
		if (g_client_sockets.size() < MAX_CONNECTIONS)
		{
			g_client_sockets.push_back(client_socket);

			// Отображаем IP-адрес и порт клиента
			char ip_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
			cout << "Client connected from: " << ip_str << ":" << ntohs(client_addr.sin_port) << endl;

			ClientInfo* pClientInfo = new ClientInfo;
			pClientInfo->client_socket = client_socket;
			pClientInfo->client_addr = client_addr;

			// Создаем поток для обработки нового клиента
			HANDLE hThread = CreateThread(
				NULL,
				0,
				ClientHandler,
				(LPVOID)pClientInfo, // Передаем информацию о клиенте
				0,
				NULL
			);

			if (hThread == NULL)
			{
				PrintLastError(GetLastError());
				closesocket(client_socket);
				g_client_sockets.pop_back(); // Удаляем сокет, если поток не создан
				delete pClientInfo;
			}
			else
			{
				CloseHandle(hThread); // Закрываем дескриптор потока, так как он нам больше не нужен
									  // Поток будет работать до завершения ClientHandler
			}
		}
		else
		{
			// Если превышено максимальное количество соединений
			char ip_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
			cout << "Client rejected (too many connections): " << ip_str << ":" << ntohs(client_addr.sin_port) << endl;

			CHAR recv_buffer[DEFAULT_BUFFER_LENGTH] = {};

			send(client_socket, g_OVERFLOW, strlen(g_OVERFLOW), 0);
			closesocket(client_socket);
		}
		LeaveCriticalSection(&g_csSocketList); // Отпускаем критическую секцию
	} while (true);

	
	closesocket(listen_socket);
	freeaddrinfo(result);
	WSACleanup();
	DeleteCriticalSection(&g_csSocketList); // Удаляем критическую секцию
}

// Функция для широковещательной рассылки сообщений
void BroadcastMessage(const char* message, SOCKET sender_socket)
{
	EnterCriticalSection(&g_csSocketList);
	for (size_t i = 0; i < g_client_sockets.size(); ++i)
	{

		if (send(g_client_sockets[i], message, strlen(message), 0) == SOCKET_ERROR)
		{
	
			PrintLastError(WSAGetLastError());
		}
	}
	LeaveCriticalSection(&g_csSocketList);
}

// Поток для обработки каждого клиента
DWORD WINAPI ClientHandler(LPVOID lpParam)
{
	ClientInfo* pClientInfo = static_cast<ClientInfo*>(lpParam);
	SOCKET client_socket = pClientInfo->client_socket;
	sockaddr_in client_addr = pClientInfo->client_addr;

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
	cout << "Handler started for client: " << ip_str << ":" << ntohs(client_addr.sin_port) << endl;

	INT iResult = 0;
	CHAR recvbuffer[DEFAULT_BUFFER_LENGTH] = {};

	do
	{
		ZeroMemory(recvbuffer, DEFAULT_BUFFER_LENGTH);
		iResult = recv(client_socket, recvbuffer, DEFAULT_BUFFER_LENGTH, 0);

		if (iResult > 0)
		{
			string received_message = string(recvbuffer, iResult); // Для корректной обработки сообщений с нулем
			cout << "Received Bytes from " << ip_str << ":" << ntohs(client_addr.sin_port) << ": " << iResult << ", Message: " << received_message << endl;

			if (received_message == "exit")
			{
				cout << "Client " << ip_str << ":" << ntohs(client_addr.sin_port) << " requested exit." << endl;
				break; // Выход из цикла
			}

			// Широковещательная рассылка полученного сообщения всем подключенным клиентам
			BroadcastMessage(recvbuffer, client_socket);
		}
		else if (iResult == 0)
		{
			cout << "Client " << ip_str << ":" << ntohs(client_addr.sin_port) << " disconnected." << endl;
			break; // Соединение закрыто клиентом
		}
		else
		{
			// Ошибка получения данных
			PrintLastError(WSAGetLastError());
			break; // Выход из цикла
		}
	} while (iResult > 0);

	// Удаление сокета из глобального списка
	EnterCriticalSection(&g_csSocketList);
	for (auto it = g_client_sockets.begin(); it != g_client_sockets.end(); ++it)
	{
		if (*it == client_socket)
		{
			g_client_sockets.erase(it);
			break;
		}
	}
	LeaveCriticalSection(&g_csSocketList);

	closesocket(client_socket); // Закрываем сокет для этого клиента
	delete pClientInfo; // Освобождаем выделенную память

	cout << "Handler for client " << ip_str << ":" << ntohs(client_addr.sin_port) << " exiting." << endl;
	return 0;
}