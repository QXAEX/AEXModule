#include <iostream>
#include "../../../h/Network/TCPPlus/Server/Server.h"
void main() {
	TCPPlusServer::Server server;
	bool flag = server.start(4000, [&](TCPPlusServer::Server::SERVER_STATUS status, SOCKET client) {
		switch (status)
		{
		case TCPPlusServer::Server::SERVER_STATUS::SERVER_STATUS_CONNECTED:
			std::cout << "Client connected" << std::endl;
			server.Disconnect(client);
			break;
		case TCPPlusServer::Server::SERVER_STATUS::SERVER_STATUS_CLIENT_CONTROLLER:
			Byteset data = server.Listen(client);
			if (data.size() > 0) {
				std::cout << "Received data: " << data.data() << "length:" << data.size() << std::endl;
				std::string data;
				std::cin >> data;
				server.Send(client, data);
			}
			break;
		}
		});
	if (flag) {
		std::cout << "Æô¶¯³É¹¦" << std::endl;
	}
	Sleep(1000000000);
}