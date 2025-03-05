#include "../h/Network.h"
#include <iostream>
#include <string>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
constexpr auto SENDSRECVS_SIZE = 40960;
INT Network::MSG(std::string message, int result, std::string title)
{
    typedef int(*MessageBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);
    HMODULE hUser32 = LoadLibraryA("user32.dll");
    if (hUser32 == nullptr) {
        std::cerr << "Failed to load user32.dll!" << std::endl;
        return 0;
    }
    static MessageBoxA_t pfnMessageBoxA = (MessageBoxA_t)GetProcAddress(hUser32, "MessageBoxA");
    return pfnMessageBoxA(NULL, std::string(message).append(result ? std::to_string(result) : "").c_str(), "Error", MB_OK);
}

Network::TCP::Server::Server() {
    this->started = false;
    this->ip = "";
    this->port = 0;
    this->_socket_ = INVALID_SOCKET;
    this->_serverAddr_ = {};
}

Network::TCP::Server::~Server() {
    this->stop();
}

Network::TCP::Server& Network::TCP::Server::operator=(const Server& other)
{
    if (this == &other) return *this;
    this->started = other.started;
    this->ip = other.ip;
    this->port = other.port;
    this->_socket_ = other._socket_;
    this->_serverAddr_ = other._serverAddr_;
    this->clients = other.clients;
    return *this;
}

bool Network::TCP::Server::start(int port) {
    if (started) return false;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        MSG("WSAStartup failed: ", result);
        return false;
    }

    _socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket_ == INVALID_SOCKET) {
        MSG("Error at socket: ", this->GetLastError());
        WSACleanup();
        return false;
    }

    _serverAddr_.sin_family = AF_INET;
    _serverAddr_.sin_addr.s_addr = INADDR_ANY;
    _serverAddr_.sin_port = htons(port);

    result = bind(_socket_, (SOCKADDR*)&_serverAddr_, sizeof(_serverAddr_));
    if (result == SOCKET_ERROR) {
        MSG("bind failed with error: ", this->GetLastError());
        closesocket(_socket_);
        WSACleanup();
        return false;
    }

    result = listen(_socket_, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        MSG("listen failed with error: ", this->GetLastError());
        closesocket(_socket_);
        WSACleanup();
        return false;
    }

    this->port = port;
    this->started = true;
    return true;
}

void Network::TCP::Server::stop() {
    if (!started) return;

    for (auto& client : clients) {
        closesocket(client.handle);
    }
    clients.clear();

    closesocket(_socket_);
    WSACleanup();

    this->started = false;
}

Network::TCP::CLIENT Network::TCP::Server::Listen(int timeout) {
    if (!this->started) return CLIENT();
    CLIENT newClient = {};
    int addrlen = sizeof(newClient.addr);

    if (timeout != -1) {
        fd_set readSet{};
        FD_ZERO(&readSet);
        FD_SET(_socket_, &readSet);

        timeval tv{};
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        int result = select(NULL, &readSet, nullptr, nullptr, &tv);
        if (result == NULL) {
            newClient.state = _SERVER::SERVER_TIMEOUT;
            return newClient;
        }
        else if (result == SOCKET_ERROR) {
            newClient.state = _SERVER::SERVER_CLIENT_DISCONNECTION;
            return newClient;
        }
    }

    newClient.handle = accept(_socket_, (SOCKADDR*)&newClient.addr, &addrlen);
    if (newClient.handle == INVALID_SOCKET) {
        newClient.state = _SERVER::SERVER_CLIENT_DISCONNECTION;
        return newClient;
    }

    newClient.state = _SERVER::SERVER_CLIENT_CONNECTION;
    newClient.server = this;
    clients.push_back(newClient);
    return clients[clients.size() - 1];
}

bool Network::TCP::Server::disconnect(PCLIENT client) {
    for (auto it = this->clients.begin(); it != this->clients.end(); ++it) {
        if (it->handle == client->handle) {
            closesocket(client->handle);
            this->clients.erase(it);
            return true;
        }
    }
    return false;
}

bool Network::TCP::Server::Send(PCLIENT client, Byteset data, int timeout) const
{
    if (!started) return false;
    if (timeout != -1) {
        fd_set writeSet{};
        FD_ZERO(&writeSet);
        FD_SET(client->handle, &writeSet);

        timeval tv{};
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        int result = select(NULL, nullptr, &writeSet, nullptr, &tv);
        if (result == NULL) {
            client->state = _SERVER::SERVER_TIMEOUT;
            return false;
        }
        else if (result == SOCKET_ERROR) {
            client->state = _SERVER::SERVER_DATA_SEND_FAILED;
            return false;
        }
    }
    int result = send(client->handle, (char*)data.data(), static_cast<int>(data.size()), 0);
    if (result == SOCKET_ERROR) {
        client->state = _SERVER::SERVER_DATA_SEND_FAILED;
        return false;
    }
    client->state = _SERVER::SERVER_DATA_SEND;
    return true;
}

bool Network::TCP::Server::Sends(PSERVER_MSG_INFO info) const
{
    if (!started) return false;
    __int64 maxLen = info->file->Size();
        __int64 index = NULL,time = NULL;
        Byteset data;
        do {
            info->file->Seek(index);
            data = info->file->Read(SENDSRECVS_SIZE);
            time = System::GetRunTime();
            this->Send(info->client, data, info->timeout);
            this->Recv(info->client, info->timeout);
            time = System::GetRunTime() - time;
            maxLen -= data.size();
            info->is_end = !(maxLen > NULL);
            index += data.size();
            info->speed = System::DataSizeConversion(data.size(), info->unit, time);
        } while (!info->is_end);
    return true;
}

Byteset Network::TCP::Server::Recv(PCLIENT client, int timeout) const
{
    if (!started) return Byteset();
    if (timeout != -1) {
        fd_set readSet{};
        FD_ZERO(&readSet);
        FD_SET(client->handle, &readSet);
        timeval tv{};
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        int result = select(NULL, &readSet, nullptr, nullptr, &tv);
        if (result == NULL) {
            client->state = _SERVER::SERVER_TIMEOUT;
            return Byteset();
        }
        else if (result == SOCKET_ERROR) {
            client->state = _SERVER::SERVER_DATA_RECV_FAILED;
            return Byteset();
        }
    }
    Byteset data;
    __int64 time = System::GetRunTime();
    char* buffer = new char[SENDSRECVS_SIZE];
    int result = recv(client->handle, buffer, SENDSRECVS_SIZE, 0);
    if (result == SOCKET_ERROR || result == 0) {
        client->state = _SERVER::SERVER_DATA_RECV_FAILED;
        return Byteset();
    }
    data.append(buffer, result);
    delete[] buffer;
    client->state = data.size() != NULL ? _SERVER::SERVER_DATA_RECV : _SERVER::SERVER_DATA_RECV_FAILED;
    return data;
}

bool Network::TCP::Server::Recvs(PSERVER_MSG_INFO info) const
{
    if (!started) return false;
    __int64 index = NULL, time = NULL, size = NULL;
    Byteset data;
    info->file->Close();
    info->file->DelFile();
    if (info->file->Open(System::GetRunPath() + "\\temp\\recvs_" + Text::text_random_num() + ".dat", true)) {
        do {
            info->file->Seek(index);
            time = System::GetRunTime();
            data = this->Recv(info->client, info->timeout);
            this->Send(info->client, { 1 }, info->timeout);
            time = System::GetRunTime() - time;
            info->file->Write(data);
            size = data.size();
            info->is_end = !(size == SENDSRECVS_SIZE);
            index += size;
            info->speed = System::DataSizeConversion(size, info->unit, time);
        } while (!info->is_end);
        return true;
    }
    return false;
}

size_t Network::TCP::Server::GetClientCount() {
    return clients.size();
}

Network::TCP::PCLIENT Network::TCP::Server::GetClient(__int64 index) {
    if (index < 0 || index >= clients.size()) {
        return nullptr;
    }
    return &clients[index];
}

int Network::TCP::Server::GetLastError() const
{
    return WSAGetLastError();
}

Network::TCP::Client::Client()
{
    this->started = false;
    this->_socket_ = INVALID_SOCKET;
    this->_serverAddr_ = {};
}

Network::TCP::Client::~Client()
{
    this->disconnect();
}

Network::TCP::Client& Network::TCP::Client::operator=(const Client& other)
{
    if (this == &other) return *this;
    this->started = other.started;
    this->_socket_ = other._socket_;
    this->_serverAddr_ = other._serverAddr_;
    return *this;
}

bool Network::TCP::Client::Connect(const std::string& ip, int port)
{
    if (started) return false;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        MSG("WSAStartup failed: ", result);
        return false;
    }

    _socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket_ == SOCKET_ERROR) {
        MSG("Error at socket: ", this->GetLastError());
        WSACleanup();
        return false;
    }

    u_long iMode = 0;
    result = ioctlsocket(_socket_, FIONBIO, &iMode);
    if (result != NO_ERROR) {
        MSG("ioctlsocket failed with error: ", result);
        closesocket(_socket_);
        WSACleanup();
        return false;
    }

    sockaddr_in socketAddr{ 0 };
    _serverAddr_.sin_family = AF_INET;
    _serverAddr_.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
    _serverAddr_.sin_port = htons(port);
    result = connect(_socket_, (SOCKADDR*)&_serverAddr_, sizeof(SOCKADDR));
    if (result == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            MSG("connect failed with error: ", this->GetLastError());
            closesocket(_socket_);
            WSACleanup();
            return false;
        }
    }

    // 使用 select 等待连接完成
    fd_set writefds{};
    FD_ZERO(&writefds);
    FD_SET(_socket_, &writefds);
    timeval timeout{};
    timeout.tv_sec = 5;//5 s
    timeout.tv_usec = 0;

    result = select(0, NULL, &writefds, NULL, &timeout);
    if (result == SOCKET_ERROR) {
        MSG("select failed with error: ", this->GetLastError());
        closesocket(_socket_);
        WSACleanup();
        return false;
    }
    if (result == 0) {
        MSG("connect timed out");
        closesocket(_socket_);
        WSACleanup();
        return false;
    }

    // 检查连接是否成功
    int error = 0;
    int len = sizeof(error);
    getsockopt(_socket_, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
    if (error != 0) {
        MSG("connect failed with error: ", error);
        closesocket(_socket_);
        WSACleanup();
        return false;
    }

    this->started = true;
    return true;
}

bool Network::TCP::Client::disconnect()
{
    if (!started) return false;

    closesocket(_socket_);
    WSACleanup();

    this->started = false;
    return true;
}

bool Network::TCP::Client::Send(Byteset data, int timeout) const
{
    if (!started) return false;
    if (timeout != -1) {
        fd_set writeSet{};
        FD_ZERO(&writeSet);
        FD_SET(_socket_, &writeSet);

        timeval tv{};
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        int result = select(NULL, nullptr, &writeSet, nullptr, &tv);
        if (result == NULL) {
            return false;
        }
        else if (result == SOCKET_ERROR) {
            return false;
        }
    }
    int result = send(_socket_, (char*)data.data(),static_cast<int>(data.size()), 0);
    if (result == SOCKET_ERROR) {
        return false;
    }
    return false;
}

bool Network::TCP::Client::Sends(PCLIENTS_MSG_INFO info) const
{
    if (!started) return false;
    __int64 maxLen = info->file->Size();
    __int64 index = NULL, time = NULL, size = NULL;
    Byteset data;
    do {
        info->file->Seek(index);
        data = info->file->Read(SENDSRECVS_SIZE);
        time = System::GetRunTime();
        this->Send(data, info->timeout);
        this->Recv(info->timeout);
        time = System::GetRunTime() - time;
        size = data.size();
        maxLen -= size;
        info->is_end = !(maxLen > NULL);
        index += size;
        info->speed = System::DataSizeConversion(size, info->unit, time);
    } while (!info->is_end);
    return true;
}

Byteset Network::TCP::Client::Recv(int timeout) const
{
    if (!started) return Byteset();
    if (timeout != -1) {
        fd_set readSet{};
        FD_ZERO(&readSet);
        FD_SET(_socket_, &readSet);

        timeval tv{};
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        int result = select(NULL, &readSet, nullptr, nullptr, &tv);
        if (result == NULL) {
            return Byteset();
        }
        else if (result == SOCKET_ERROR) {
            return Byteset();
        }
    }
    Byteset data;
    char* buffer = new char[SENDSRECVS_SIZE];
    int result = recv(_socket_, buffer, SENDSRECVS_SIZE, 0);
    if (result == SOCKET_ERROR || result == 0) {
        return Byteset();
    }
    data.append(buffer, result);
    delete[] buffer;
    return data;
}

bool Network::TCP::Client::Recvs(PCLIENTS_MSG_INFO info) const
{
    if (!started) return false;
    bool flag = false;
    __int64 index = NULL,time = NULL,size = NULL;
    info->file->Seek(NULL);
    info->file->Delete(info->file->Size());
    Byteset data;
    do {
        info->file->Seek(index);
        time = System::GetRunTime();
        data = this->Recv(info->timeout);
        this->Send({ 1 }, info->timeout);
        time = System::GetRunTime() - time;
        info->file->Write(data);
        size = data.size();
        info->is_end = !(size == SENDSRECVS_SIZE);
        index += size;
        info->speed = System::DataSizeConversion(size, info->unit, time);
    } while (!info->is_end);
    return true;
}

int Network::TCP::Client::GetLastError() const
{
    return WSAGetLastError();
}
