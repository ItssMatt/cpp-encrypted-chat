#include "main.h"

bool toggle_say = false;

void* server_listener();

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

int main(void) {
	SET_CONSOLE_COLOR(COLOR_RED);
	console.write("|***********************************|  \n");
	console.write("\n|             Server                |\n\n");
	console.write("|***********************************|\n");
	SET_CONSOLE_COLOR(COLOR_YELLOW);
	console.write("Use /help for a list of available commands.\n\n");
	if (server_init())
	{
		SET_CONSOLE_COLOR(COLOR_DARK_GREEN);
		std::stringstream str;
		str << "Server started on port " << server.server_port << ".\n";
		console.write(str.str());
		SET_CONSOLE_COLOR(COLOR_WHITE);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)server_listener, nullptr, 0, 0);
	}
	else exit(0);

	while (true) {
		std::string command = console.read();
		if (!toggle_say) {
			if (!strcmp(command.c_str(), "help")) {
				SET_CONSOLE_COLOR(COLOR_DARK_GREEN);
				console.write("Commands: list, say.");
				SET_CONSOLE_COLOR(COLOR_WHITE);
			}
			else if (!strcmp(command.c_str(), "list")) {
				SET_CONSOLE_COLOR(COLOR_GOLD);
				std::stringstream str;
				str << "List:\n";
				for (auto i = sockets.begin(); i != sockets.end(); i++) {
					str << "(Name: " << (*i).name << " - Socket: #" << (*i).socket << ")";
				}
				str << "\n";
				console.write(str.str());
				SET_CONSOLE_COLOR(COLOR_WHITE);
			}
			else if (!strcmp(command.c_str(), "say")) {
				SET_CONSOLE_COLOR(COLOR_GOLD);
				console.write("Toggled ON /say. Leave BLANK to toggle OFF.");
				toggle_say = true;
				SET_CONSOLE_COLOR(COLOR_WHITE);
			}
			else {
				SET_CONSOLE_COLOR(COLOR_GREY);
				console.write("Unknown command. Use /help for a list of commands.\n");
				SET_CONSOLE_COLOR(COLOR_WHITE);
			}
		}
		else {
			if (command.length() == 0)
			{
				toggle_say = false;
				SET_CONSOLE_COLOR(COLOR_GOLD);
				console.write("Toggled OFF /say.");
				SET_CONSOLE_COLOR(COLOR_WHITE);
			}
			else {
				std::stringstream str;
				str << "SERVER SAID: " << command << "\n";
				for (auto i = sockets.begin(); i != sockets.end(); i++) {
					send((*i).socket, xor_encryption(str.str().c_str()), strlen(str.str().c_str()) + 1, 0);
				}
				SET_CONSOLE_COLOR(COLOR_RED);
				console.write(str.str());
				SET_CONSOLE_COLOR(COLOR_WHITE);
			}
		}
		Sleep(300);
	}
}

void* server_listener() {
	while (true)
	{
		fd_set copy = server.master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == server.listening)
			{
				sockaddr_in peer_addr;
				socklen_t peer_addrlen = sizeof(peer_addr);
				memset(&peer_addr, '\0', peer_addrlen);
				SOCKET client = accept(server.listening, (struct sockaddr*)&peer_addr, &peer_addrlen);

				FD_SET(client, &server.master);

				SET_CONSOLE_COLOR(COLOR_DARK_GREEN);
				std::stringstream str;
				char strr[INET_ADDRSTRLEN];
				str << "Connection from " << inet_ntop(AF_INET, &peer_addr.sin_addr, strr, sizeof(strr)) << ":" << ntohs(peer_addr.sin_port);
				console.write(str.str());
				SET_CONSOLE_COLOR(COLOR_WHITE);
				CSocket socket = CSocket(" ", client);
				sockets.push_back(socket);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					closesocket(sock);
					FD_CLR(sock, &server.master);
					SET_CONSOLE_COLOR(COLOR_DARK_GREEN);
					std::stringstream str;
					str << "Socket #" << sock << " has disconnected.\n";
					console.write(str.str());
					SET_CONSOLE_COLOR(COLOR_WHITE);
					for (auto i = sockets.begin(); i != sockets.end(); i++) {
						if ((*i).socket == sock) {
							sockets.erase(i);
							break;
						}
					}
				}
				else
				{
					std::stringstream str;
					for (auto i = sockets.begin(); i != sockets.end(); i++) {
						if ((*i).socket == sock) {
							str << (*i).name << "#" << sock << ": " << xor_encryption(buf) << "\n";
							break;
						}
					}
					for (auto i = sockets.begin(); i != sockets.end(); i++) {
						send((*i).socket, xor_encryption(str.str().c_str()), strlen(str.str().c_str()) + 1, 0);
					}
					console.write(str.str());
				}
			}
		}
	}
}