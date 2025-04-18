#include <iostream>
#include "../../../h/Network/TCPPlus/Client/Client.h"
void main() {
	TCPPlusClient::Client client;
	DWORD code = client.start("192.168.1.7", 4000);
	if (code == TCPPlusClient::Client::CLIENT_STATUS_CONNECTED_SUCCESS) {
		std::cout << "连接成功" << std::endl;
		while (true)
		{
			std::string data;
			std::cin >> data;
			client.Send(data, [](TCPPlusClient::Client::CLIENT_STATUS status, Byteset data) {
				std::cout << "收到数据:" << data.data() << " - SIZE:" << data.size() << " - STATUS:" << status << std::endl;
				});
		}
	}
	else {
		std::cout << "连接失败 CODE:"<< code << std::endl;
	}
	Sleep(1000000000);
}