#include "../../../../AEXModule.h"

int main() {
    HTTP::HClient client;
    client.setServerInfo("127.0.0.1", 8081);
    HTTP::HTTPHANDLE params;
    params.insert(std::make_pair("name", "AEX"));
    params.insert(std::make_pair("age", "25"));
    client.Request("hello", "/", HTTP::HClient::HTTP_POST, [&](HTTP::HClient::CLIENT_STATUS status, Byteset recvData) {
        printf("客户端接收数据: %s\n", recvData.data());
        }, params);
    while (true) Sleep(1000);
    return 0;
}