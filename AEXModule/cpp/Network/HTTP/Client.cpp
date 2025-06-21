#include "../../../h/Network/Http.h"
#include <sstream>
#include <iomanip>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

HTTP::HClient::HClient(std::string ip, DWORD port)
{
    this->setServerInfo(ip, port);
}

HTTP::HClient::~HClient()
{
    this->stop();
}

void HTTP::HClient::setServerInfo(std::string ip, DWORD port)
{
    this->ip = ip;
    this->port = port;
}

HTTP::HClient::CLIENT_STATUS HTTP::HClient::start(std::string ip, DWORD port)
{
    if (isRun) {
        stop();
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return HTTP::HClient::CLIENT_STATUS_ERROR;
    }

    // 创建客户端套接字
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        WSACleanup();
        return HTTP::HClient::CLIENT_STATUS_ERROR;
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
        return (error == WSAETIMEDOUT) ? HTTP::HClient::CLIENT_STATUS_CONNECTED_FAILED : HTTP::HClient::CLIENT_STATUS_ERROR;
    }

    isRun = true;
    return HTTP::HClient::CLIENT_STATUS_CONNECTED_SUCCESS;
}

HTTP::HClient::CLIENT_STATUS HTTP::HClient::stop() {
    if (clientSocket != INVALID_SOCKET) {
        shutdown(clientSocket, 0x02);
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return HTTP::HClient::CLIENT_STATUS_ERROR;
    }
    WSACleanup();
    isRun = false;
    return HTTP::HClient::CLIENT_STATUS_STOPPED;
}

bool HTTP::HClient::Send(Byteset data, HTTP::HClient::SendCalback callback, size_t size, DWORD time)
{
    // 状态预检查
    if (!isRun || clientSocket == INVALID_SOCKET) {
        if (!data.size()) {
            if (callback) callback(HTTP::HClient::CLIENT_STATUS_DATA_EMPTY, Byteset());
        }
        else {
            if (callback) callback(HTTP::HClient::CLIENT_STATUS_ERROR, Byteset());
        }
        return false;
    }

    // 发送阶段
    CLIENT_STATUS sendStatus = HTTP::HClient::CLIENT_STATUS_SEND_SUCCESS;
    int totalSent = 0;
    const char* buffer = reinterpret_cast<const char*>(data.data());
    const int totalSize = static_cast<int>(data.size());

    while (totalSent < totalSize) {
        int sent = send(clientSocket, buffer + totalSent, totalSize - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            const int error = WSAGetLastError();
            sendStatus = (error == WSAEWOULDBLOCK) ? HTTP::HClient::CLIENT_STATUS_SEND_FAILED : HTTP::HClient::CLIENT_STATUS_SERVER_CLOSED;
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
    HTTP::HClient::CLIENT_STATUS recvStatus = HTTP::HClient::CLIENT_STATUS_RECV_SUCCESS;
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
            recvStatus = (WSAGetLastError() == WSAETIMEDOUT) ? HTTP::HClient::CLIENT_STATUS_RECV_TIMEOUT : HTTP::HClient::CLIENT_STATUS_SERVER_CLOSED;
        }
        else {
            char chunk[4096];
            while (true) {
                int recvSize = recv(clientSocket, chunk, sizeof(chunk), 0);
                if (recvSize == SOCKET_ERROR) {
                    recvStatus = (WSAGetLastError() == WSAETIMEDOUT) ? HTTP::HClient::CLIENT_STATUS_RECV_TIMEOUT : HTTP::HClient::CLIENT_STATUS_SERVER_CLOSED;
                    if (recvSize == 0) {
                        recvStatus = HTTP::HClient::CLIENT_STATUS_SERVER_CLOSED;
                        break;
                    }
                    break;
                }
                if (recvSize == 0) {
                    recvStatus = HTTP::HClient::CLIENT_STATUS_SERVER_CLOSED;
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
        recvStatus = HTTP::HClient::CLIENT_STATUS_RECV_ERROR;
    }

    // 恢复原始超时设置
    if (time != NULL) {
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&originalTimeout), sizeof(originalTimeout));
    }
    // 回调处理
    if (recvStatus == HTTP::HClient::CLIENT_STATUS_RECV_SUCCESS && !receivedData.empty()) {
        if (callback) callback(HTTP::HClient::CLIENT_STATUS_SEND_SUCCESS, receivedData);
    }
    else {
        if (callback) callback(recvStatus, receivedData);
        if (recvStatus == HTTP::HClient::CLIENT_STATUS_SERVER_CLOSED) stop();
    }

    return (recvStatus == HTTP::HClient::CLIENT_STATUS_RECV_SUCCESS);
}

std::string HTTP::HClient::urlEncode(const std::string& value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        }
        else {
            escaped << '%' << std::setw(2) << int((unsigned char)c);
        }
    }
    return escaped.str();
}

std::string HTTP::HClient::buildQuery(const HTTPPARAMS& params) {
    std::ostringstream oss;
    bool first_param = true;

    for (auto it = params.begin(); it != params.end(); ++it) {
        const std::string key = it.key().asString();
        std::string value;
        // 根据类型转换值
        if (it->isNull()) {
            value = "null";
        }
        else if (it->isBool()) {
            value = it->asBool() ? "true" : "false";
        }
        else if (it->isIntegral()) {
            value = std::to_string(it->asLargestInt());
        }
        else if (it->isDouble()) {
            value = std::to_string(it->asDouble());
        }
        else if (it->isString()) {
            value = it->asString();
        }
        else if (it->isArray() || it->isObject()) {
            Json::StreamWriterBuilder writer;
            value = Json::writeString(writer, *it);
        }
        else {
            value = it->asString();
        }
        if (!first_param) {
            oss << "&";
        }
        oss << urlEncode(key) << "=" << urlEncode(value);
        first_param = false;
    }

    return oss.str();
}

std::string HTTP::HClient::methodToString(HTTP::HClient::HTTP_METHOD method)
{
    static const std::string methods[] = { "GET", "POST", "PUT", "DELETE" };
    return methods[static_cast<int>(method)];
}

Byteset HTTP::HClient::HandleRequest(Byteset data, std::string url, HTTP_METHOD method, HTTPPARAMS params, HTTPHANDLE header) {
    std::string encodedParams = buildQuery(params);
    std::ostringstream oss;
    // ======================
    // 1. 请求行生成优化
    // ======================
    const bool isQueryMethod = (method == HTTP_GET || method == HTTP_DELETE);
    oss << methodToString(method) << " "
        << url << (isQueryMethod && !encodedParams.empty() ? "?" + encodedParams : "")
        << " HTTP/1.1\r\n";

    // ======================
    // 2. 头部生成优化
    // ======================
    // 自动添加必要头字段
    if (!header.count("Host")) {
        oss << "Host: " << ip << ":" << port << "\r\n";
    }
    oss << "Connection: " << (keepAlive_ ? "keep-alive" : "close") << "\r\n";

    // 预处理内容类型（先于自定义头处理）
    const bool needBody = (method == HTTP_POST || method == HTTP_PUT);
    const bool useFormData = needBody && data.empty();
    if (needBody && !header.count("Content-Type")) {
        oss << "Content-Type: " << (useFormData
            ? "application/x-www-form-urlencoded"
            : "application/octet-stream") << "\r\n";
    }

    // 用户自定义头（后处理以保证优先级）
    for (const auto& [key, value] : header) {
        oss << key << ": "
            << std::string(value.begin(), value.end()) // 显式二进制转换
            << "\r\n";
    }

    // ======================
    // 3. 请求体生成优化
    // ======================
    if (needBody) {
        const std::string body = useFormData
            ? encodedParams
            : std::string(reinterpret_cast<const char*>(data.data()), data.size());

        // 精准内容长度计算（避免用户错误设置）
        oss << "Content-Length: " << body.size() << "\r\n\r\n"
            << body;
    }
    else {
        oss << "\r\n";  // 非body方法结束头部
    }

    return Byteset(oss.str().data(), oss.str().size());
}



Byteset HTTP::HClient::HandleResponse(Byteset data, std::string url, HTTP::HClient::HTTP_METHOD method, HTTPPARAMS params)
{
    return Byteset();
}

void HTTP::HClient::Request(Byteset data, std::string url, HTTP_METHOD method, HTTP::HClient::SendCalback callback, HTTPPARAMS params, HTTPHANDLE header, size_t size, DWORD time)
{
    this->start(this->ip, this->port);
    Byteset request = HandleRequest(data, url, method, params, header);
    this->Send(request, [&](HTTP::HClient::CLIENT_STATUS status, Byteset recvData) {
        if (callback) callback((HTTP::HClient::CLIENT_STATUS)status, recvData);
        this->stop();
        }, size, time);
}

void HTTP::HClient::setKeepAlive(BOOL keepAlive)
{
    this->keepAlive_ = keepAlive;
}
