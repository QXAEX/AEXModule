#include "../../../../h/Network/TCPPlus/Server/Server.h"
#pragma comment(lib, "ws2_32.lib")

TCPPlusServer::Server::~Server() {
    this->stop();
}
bool TCPPlusServer::Server::start(DWORD port, Callback callback)
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
    });

    return true;
}

void TCPPlusServer::Server::stop() {
	if (this->isRun) {
		this->isRun = false;
        for (auto it = cSockets.begin(); it != cSockets.end(); ++it) {
            this->Disconnect(*it);
        }
		closesocket(this->listenSocket);
		WSACleanup();
	}
}

Byteset TCPPlusServer::Server::Listen(SOCKET client, size_t size)
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
                data.insert(data.size(), buffer, len);
                break;
            }
            data.insert(data.size(), buffer, len);
        };
    }
    return data;
}

bool TCPPlusServer::Server::Send(SOCKET client,const Byteset data)
{
    return send(client, (PCHAR)data.data(), data.size(), 0) != SOCKET_ERROR;
}

void TCPPlusServer::Server::Disconnect(SOCKET client)
{
    closesocket(client);
    for (auto it = cSockets.begin(); it != cSockets.end(); ++it) {
        if (*it == client) {
            cSockets.erase(it);
            break;
        }
    }
    if (callback) callback(TCPPlusServer::Server::SERVER_STATUS_DISCONNECTED, client);
}

int TCPPlusServer::Server::GetClientCount() {
    return cSockets.size();
}

std::string TCPPlusServer::Server::GetClientIP(SOCKET client)
{
    sockaddr_in clientAddr{};
    int len = sizeof(clientAddr);
    if (getsockname(client, (sockaddr*)&clientAddr, &len) == SOCKET_ERROR) return "";
    return inet_ntoa(clientAddr.sin_addr);
}

int TCPPlusServer::Server::GetClientPort(SOCKET client)
{
    sockaddr_in clientAddr{};
    int len = sizeof(clientAddr);
    if (getsockname(client, (sockaddr*)&clientAddr, &len) == SOCKET_ERROR) return -1;
    return ntohs(clientAddr.sin_port);
}

bool TCPPlusServer::Server::IsClientDisconnect(SOCKET client)
{
    for (auto c : cSockets) {
        if (c == client) {
            return false;
        }
    }
    return true;
}
