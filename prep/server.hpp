#pragma once

#include <sstream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class CServer {
public:
	const USHORT server_port = 6666;
	SOCKET listening;
	FD_SET master;

	static CServer& getInstance()
	{
		static CServer instance;
		return instance;
	}
};
#define server CServer::getInstance()

bool server_init() {

	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		console.write("Cannot Initialize winsock!\n");
		return false;
	}

	server.listening = socket(AF_INET, SOCK_STREAM, 0);
	if (server.listening == INVALID_SOCKET)
	{
		console.write("Cannot create a socket!\n");
		return false;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(server.server_port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; 

	bind(server.listening, (sockaddr*)&hint, sizeof(hint));

	listen(server.listening, SOMAXCONN);
	
	FD_ZERO(&server.master);

	FD_SET(server.listening, &server.master);

	return true;
}