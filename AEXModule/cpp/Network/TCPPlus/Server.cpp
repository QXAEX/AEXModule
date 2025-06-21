#include "../../../h/Network/TCPPlus.h"
#pragma comment(lib, "ws2_32.lib")

TCPPlus::Server::~Server() {
    this->stop();
}

bool TCPPlus::Server::start(DWORD port, TCPPlus::Server::Callback callback)
{
    if (this->isRun) {
        this->stop();
    }
    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        if (callback) callback(SERVER_STATUS_ERROR, INVALID_SOCKET);
        return false;
    }
    // 创建监听套接字
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        WSACleanup();
        if (callback) callback(SERVER_STATUS_ERROR, INVALID_SOCKET);
        return false;
    }

    // 绑定地址
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(listenSocket);
        WSACleanup();
        if (callback) callback(SERVER_STATUS_ERROR, listenSocket);
        return false;
    }

    // 开始监听
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(listenSocket);
        WSACleanup();
        if (callback) callback(SERVER_STATUS_ERROR, INVALID_SOCKET);
        return false;
    }

    // 启动监听线程
    this->isRun = TRUE;
    thread.add(NULL, [&, callback](PThread pthread) {
        if (callback) callback(SERVER_STATUS_STARTED, INVALID_SOCKET);
        while (this->isRun) {
            // 接受客户端连接
            SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                if (this->isRun && callback)
                    callback(SERVER_STATUS_CLIENTLINK_ERROR, clientSocket);
                continue;
            }
            // 保存客户端套接字并触发回调
            cSockets.push_back(clientSocket);
            if (callback) callback(SERVER_STATUS_CONNECTED, clientSocket);
            thread.add(clientSocket, [&, callback, clientSocket](PThread pthread) {
                while (this->isRun && !this->IsClientDisconnect(clientSocket)) {
                    if (callback) callback(SERVER_STATUS_CLIENT_CONTROLLER, clientSocket);
                }
                });
        };
        thread.remove(NULL, true);
        });

    return true;
}

void TCPPlus::Server::stop() {
    if (this->isRun) {
        this->isRun = false;
        for (SOCKET it : cSockets) {
            this->Disconnect(it);
        }
        closesocket(this->listenSocket);
        WSACleanup();
    }
}

Byteset TCPPlus::Server::Listen(SOCKET client, size_t size)
{
    Byteset data;
    if (size != NULL) {
        data.resize(size);
        if (recv(client, (PCHAR)data.data(), size, 0) == SOCKET_ERROR) {
            this->Disconnect(client);
        }
    }
    else {//接收全部数据
        char buffer[1024];
        int len = 0;
        while (true) {
            len = recv(client, buffer, sizeof(buffer), 0);
            if (len == SOCKET_ERROR) {
                this->Disconnect(client);
                break;
            }
            if (len == 0) {
                break;
            }
            else if (len != sizeof(buffer)) {
                data.insert(data.size(), Byteset(buffer, len), len);
                break;
            }
            data.insert(data.size(), Byteset(buffer, len), len);
        };
    }
    return data;
}

bool TCPPlus::Server::Send(SOCKET client, const Byteset data)
{
    return send(client, (PCHAR)data.data(), data.size(), 0) != SOCKET_ERROR;
}

void TCPPlus::Server::Disconnect(SOCKET client)
{
    closesocket(client);
    for (auto it = cSockets.begin(); it != cSockets.end(); ++it) {
        if (*it == client) {
            cSockets.erase(it);
            break;
        }
    }
    thread.remove(client, true);
    if (callback) callback(TCPPlus::Server::SERVER_STATUS_DISCONNECTED, client);
}

int TCPPlus::Server::GetClientCount() {
    return cSockets.size();
}

std::string TCPPlus::Server::GetClientIP(SOCKET client)
{
    sockaddr_in clientAddr{};
    int len = sizeof(clientAddr);
    if (getsockname(client, (sockaddr*)&clientAddr, &len) == SOCKET_ERROR) return "";
    return inet_ntoa(clientAddr.sin_addr);
}

int TCPPlus::Server::GetClientPort(SOCKET client)
{
    sockaddr_in clientAddr{};
    int len = sizeof(clientAddr);
    if (getsockname(client, (sockaddr*)&clientAddr, &len) == SOCKET_ERROR) return -1;
    return ntohs(clientAddr.sin_port);
}

bool TCPPlus::Server::IsClientDisconnect(SOCKET client)
{
    for (auto c : this->cSockets) {
        if (c == client) {
            return false;
        }
    }
    return true;
}
