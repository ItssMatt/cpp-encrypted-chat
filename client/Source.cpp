#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define ENCRYPTION_SEED 0x10
char* xor_encryption(const char* string)
{
	char* finish = (char*)(malloc(strlen(string) + 1));
	strcpy_s(finish, strlen(string) + 1, string);

	for (int i = 0; i < strlen(string); i++) {
		finish[i] = string[i] ^ (ENCRYPTION_SEED);
	}
	return finish;
}

std::string name;
std::string key;

void CACATHREAD(void* sock)
{
	while (true)
	{
		char buf[4096];
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(reinterpret_cast<SOCKET>(sock), buf, 4096, 0);
		if (bytesReceived > 0)
		{
			std::cout << std::string(xor_encryption(buf), 0, bytesReceived);
		}
	}
}

void main()
{
	std::cout << xor_encryption("188.27.156.83") << "\n";
	std::cout << "Name: "; std::cin >> name;
	std::cout << "Key: "; std::cin >> key;
	std::string ipAddress = xor_encryption(key.c_str());
	int port = 6666;			

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		return;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		return;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));

	if (1 == SOCKET_ERROR)
	{
		printf("Cannot connect to server.\n");
		closesocket(sock);
		WSACleanup();
		return;
	}

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CACATHREAD, (LPVOID)sock, 0, 0);

	std::string userInput;
	while(true)
	{	
		std::getline(std::cin, userInput);

		if (userInput.size() > 0)
		{
			int sendResult = send(sock, xor_encryption(userInput.c_str()), userInput.size() + 1, 0);
		}
	}
	closesocket(sock);
	WSACleanup();
}
