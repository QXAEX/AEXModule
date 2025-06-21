#include "../../../../AEXModule.h"

int main() {
    HTTP::HServer server;
    HTTP::HClient client;
    server.start(8081, [&](SOCKET client, HTTP::HTTPHANDLE headers) {
        std::cout << "▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂\n";
        std::cout << "接收到 " << headers["Method"] << " 请求\n";
        std::cout << "请求地址: " << headers["Path"] << "\n";
        std::cout << "协议版本: " << headers["Protocol"] << "\n";
        std::cout << "▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂\n\n";
        server.msgBlock("/", "GET", headers, [&](HTTP::HTTPHANDLE params) {
            std::string content = "Hello World";
            server.Send(client, content, headers);
            server.Disconnect(client);
            printf("Response sent and connection closed\n");
            });
        server.msgBlock("/", "POST", headers, [&](HTTP::HTTPHANDLE params) {
            std::string content = "Hello World 【POST】";
            server.Send(client, content, headers);
            server.Disconnect(client);
            printf("Response sent and connection closed\n");
            });
        server.msgBlock("/favicon.ico", "GET", headers, [&](HTTP::HTTPHANDLE params) {
            File file("D:\\IMG\\AEX.ico");
            server.Send(client, file.Read(file.Size()), headers, HTTP::HServer::SERVER_SENDTYPE_BINARY);
            server.Disconnect(client);
            printf("Response sent and connection closed\n");
            });
        });
    while (true) Sleep(1000);
	return 0;
}