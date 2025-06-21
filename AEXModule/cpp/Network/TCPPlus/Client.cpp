#include "../../../h/Network/TCPPlus.h"
#pragma comment(lib, "ws2_32.lib")

TCPPlus::Client::~Client() {
    stop();
}

TCPPlus::Client::CLIENT_STATUS TCPPlus::Client::start(std::string ip, DWORD port) {
    if (isRun) {
        stop();
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return TCPPlus::Client::CLIENT_STATUS_ERROR;
    }

    // 创建客户端套接字
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        WSACleanup();
        return TCPPlus::Client::CLIENT_STATUS_ERROR;
    }

    // 设置服务器地址
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(port);

    // 尝试连接
    const int connectResult = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

    if (connectResult == SOCKET_ERROR) {
        const int error = WSAGetLastError();
        stop();
        return (error == WSAETIMEDOUT) ? TCPPlus::Client::CLIENT_STATUS_CONNECTED_FAILED : TCPPlus::Client::CLIENT_STATUS_ERROR;
    }

    isRun = true;
    return TCPPlus::Client::CLIENT_STATUS_CONNECTED_SUCCESS;
}

TCPPlus::Client::CLIENT_STATUS TCPPlus::Client::stop() {
    if (clientSocket != INVALID_SOCKET) {
        shutdown(clientSocket, 0x02);
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return TCPPlus::Client::CLIENT_STATUS_ERROR;
    }
    WSACleanup();
    isRun = false;
    return TCPPlus::Client::CLIENT_STATUS_STOPPED;
}

bool TCPPlus::Client::Send(Byteset data, TCPPlus::Client::SendCalback callback, size_t size, DWORD time)
{
    // 状态预检查
    if (!isRun || clientSocket == INVALID_SOCKET) {
        if (!data.size()) {
            if (callback) callback(TCPPlus::Client::CLIENT_STATUS_DATA_EMPTY, Byteset());
        }
        else {
            if (callback) callback(TCPPlus::Client::CLIENT_STATUS_ERROR, Byteset());
        }
        return false;
    }

    // 发送阶段
    CLIENT_STATUS sendStatus = TCPPlus::Client::CLIENT_STATUS_SEND_SUCCESS;
    int totalSent = 0;
    const char* buffer = reinterpret_cast<const char*>(data.data());
    const int totalSize = static_cast<int>(data.size());

    while (totalSent < totalSize) {
        int sent = send(clientSocket, buffer + totalSent, totalSize - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            const int error = WSAGetLastError();
            sendStatus = (error == WSAEWOULDBLOCK) ? TCPPlus::Client::CLIENT_STATUS_SEND_FAILED : TCPPlus::Client::CLIENT_STATUS_SERVER_CLOSED;
            break;
        }
        totalSent += sent;
    }

    // 发送失败处理
    if (sendStatus != CLIENT_STATUS_SEND_SUCCESS) {
        if (callback) callback(sendStatus, Byteset());
        stop();
        return false;
    }

    // 不需要接收回传数据
    if (!callback) return true;

    // 接收回传数据阶段
    TCPPlus::Client::CLIENT_STATUS recvStatus = TCPPlus::Client::CLIENT_STATUS_RECV_SUCCESS;
    Byteset receivedData;
    DWORD originalTimeout = 0;
    int timeoutLen = sizeof(originalTimeout);

    // 设置临时超时
    if (time != NULL) {
        getsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,
            reinterpret_cast<char*>(&originalTimeout), &timeoutLen);
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,
            reinterpret_cast<const char*>(&time), sizeof(time));
    }

    try {
        if (size != NULL) {
            receivedData.resize(size);
            int recvSize = recv(clientSocket, (PCHAR)data.data(), size, 0);
            recvStatus = (WSAGetLastError() == WSAETIMEDOUT) ? TCPPlus::Client::CLIENT_STATUS_RECV_TIMEOUT : TCPPlus::Client::CLIENT_STATUS_SERVER_CLOSED;
        }
        else {
            char chunk[4096];
            while (true) {
                int recvSize = recv(clientSocket, chunk, sizeof(chunk), 0);
                if (recvSize == SOCKET_ERROR) {
                    recvStatus = (WSAGetLastError() == WSAETIMEDOUT) ? TCPPlus::Client::CLIENT_STATUS_RECV_TIMEOUT : TCPPlus::Client::CLIENT_STATUS_SERVER_CLOSED;
                    if (recvSize == 0) {
                        recvStatus = TCPPlus::Client::CLIENT_STATUS_SERVER_CLOSED;
                        break;
                    }
                    break;
                }
                if (recvSize == 0) {
                    recvStatus = TCPPlus::Client::CLIENT_STATUS_SERVER_CLOSED;
                    break;
                }
                else if (recvSize != sizeof(chunk)) {
                    receivedData.insert(receivedData.size(), chunk, recvSize);
                    break;
                }
                receivedData.insert(receivedData.size(), buffer, recvSize);
            }
        }
    }
    catch (...) {
        recvStatus = TCPPlus::Client::CLIENT_STATUS_RECV_ERROR;
    }

    // 恢复原始超时设置
    if (time != NULL) {
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&originalTimeout), sizeof(originalTimeout));
    }
    // 回调处理
    if (recvStatus == TCPPlus::Client::CLIENT_STATUS_RECV_SUCCESS && !receivedData.empty()) {
        if (callback) callback(TCPPlus::Client::CLIENT_STATUS_SEND_SUCCESS, receivedData);
    }
    else {
        if (callback) callback(recvStatus, receivedData);
        if (recvStatus == TCPPlus::Client::CLIENT_STATUS_SERVER_CLOSED) stop();
    }

    return (recvStatus == TCPPlus::Client::CLIENT_STATUS_RECV_SUCCESS);
}
