#pragma once

#include <WS2tcpip.h>
#include <vector>
#pragma comment (lib, "ws2_32.lib")

class CSocket {
public:
	std::string name;
	SOCKET socket;

	CSocket() { }
	CSocket(std::string name, SOCKET socket) : name(name), socket(socket) { }
};