#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include<string>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

void close_connection(SOCKET& Client_Socket) // закрыть сокет и освободить ресурсы системы
{
	shutdown(Client_Socket, SD_BOTH);
	closesocket(Client_Socket);
	WSACleanup();
}
int receive_message(SOCKET& ClientConn) // получить сообщение от сервера
{
	int state; // показатель состояния
	string message; // сообщение
	size_t message_size; // длина сообщения
	state = recv(ClientConn, (char*)&message_size, sizeof(size_t), 0); // получить размер принимаемого сообщения от клиента
	if (state == SOCKET_ERROR) // если ошибка получения
	{
		cout << "Размер сообщения не принят.\n"; // вывести диагностическое сообщение
		close_connection(ClientConn);
		return -1;
	}
	char* temporary = new char[message_size + 1];
	temporary[message_size] = '\0';
	state = recv(ClientConn, temporary, message_size, 0); // получить сообщение в строке от клиента
	if (state == SOCKET_ERROR) // если ошибка получения
	{
		cout << "Сообщение не получено.\n"; // вывести диагностическое сообщение
		close_connection(ClientConn);
		return -1;
	}
	for (size_t i = 0; i < message_size; i++)
	{
		message += temporary[i];
	}
	cout << "Полученное сообщение: " << message << endl;
	message.clear();
	message_size = 0;
	return 0;
}
int send_message(SOCKET& ClientConn)
{
	string message;
	size_t message_size;
	int state;
	cout << "Сообщение клиента: ";
	getline(cin, message);
	if (message == "close connection")
	{
		close_connection(ClientConn);
		return -1;
	}
	message_size = message.size();
	state = send(ClientConn, (char*)&message_size, sizeof(size_t), 0); // отправить клиенту размер сообщения
	if (state == SOCKET_ERROR) // если ошибка отправления
	{
		cout << "Сообщение не отправлено.\n"; // вывести диагностическое сообщение
		close_connection(ClientConn);
		return -1;
	}
	state = send(ClientConn, message.c_str(), message_size, 0);
	if (state == SOCKET_ERROR) // если ошибка отправления
	{
		cout << "Сообщение не отправлено.\n"; // вывести диагностическое сообщение
		close_connection(ClientConn);
		return -1;
	}
	message.clear();
	message_size = 0;
	return 0;
}
int main()
{
	//srand(time(0)); // установка 
	//string task_1 = "90;50;авиаудар";
	//string task_2 = "80;80;разведка";
	//double timer = 0; // счётчик времени
	//double time_for_task_1 = rand() % 40 + 1;
	//double time_for_task_2 = rand() % 40 + 50;

	//Key constants
	const char SERVER_IP[] = "127.0.0.2";
	const short SERVER_PORT_NUM = 80;				// Enter Listening port on Server side

	// Key variables for all program
	int erStat;										// For checking errors in sockets functions

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP, &ip_to_num);


	// WinSock initialization
	WSADATA wsData;
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0)
	{
		return 1;
	}
	SOCKET Client_Socket = socket(AF_INET, SOCK_STREAM, 0);

	if (Client_Socket == INVALID_SOCKET)
	{
		//cout << "Error initialization socket # " << WSAGetLastError() << endl;
		closesocket(Client_Socket);
		WSACleanup();
	}

	// Establishing a connection to Server
	sockaddr_in Server_Information;

	ZeroMemory(&Server_Information, sizeof(Server_Information));

	Server_Information.sin_family = AF_INET;
	Server_Information.sin_addr = ip_to_num;
	Server_Information.sin_port = htons(SERVER_PORT_NUM);

	// установка соединения
	if (connect(Client_Socket, (sockaddr*)&Server_Information, sizeof(Server_Information)) != 0) // если функция подключения вернула не нуль
	{
		cout << "Нет соединения с сервером.\n"; // вывести диагностическое сообщение об ошибке
		closesocket(Client_Socket); // закрыть сокет
		WSACleanup(); // освобождение ресурсов системы
		return 1;
	}
	else
	{
		cout << "Соединение с сервером установлено.\n"; // вывести диагностическое сообщение об установке соединения
	}
	send_message(Client_Socket); // отправить на ВЗПУ сообщение о готовности к работе
	bool connected = true; // показатель соединения
	while (connected) // пока соединение активно
	{
		if (send_message(Client_Socket) == -1) // прервать соединение
		{
			connected = false;
			continue;
		}
	}
	closesocket(Client_Socket);
	WSACleanup();
	return 0;
}