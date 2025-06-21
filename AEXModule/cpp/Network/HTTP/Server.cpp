#include "../../../h/Network/Http.h"
#include <sstream>
#include <functional>
#include <regex>
#include "../../../h/File.h"
#pragma comment(lib, "ws2_32.lib")

HTTP::HServer::~HServer()
{
    this->stop();
}

bool HTTP::HServer::start(DWORD port, HTTP::HServer::Callback callback) {
    return this->_start(port, [&, callback](HTTP::HServer::SERVER_STATUS status, SOCKET client) {
        switch (status)
        {
        case HTTP::HServer::SERVER_STATUS_CLIENTLINK_ERROR: {
            this->Disconnect(client);
            break;
        }
        case HTTP::HServer::SERVER_STATUS_CONNECTED: {
            Byteset data = this->Listen(client, NULL);
            if (data.size() > 0) {
                printf("%s\n", data.data());
                if (callback) callback(client, this->ResolveRequests(data));
            }
            break;
        }
        case HTTP::HServer::SERVER_STATUS_DISCONNECTED: {
            this->Disconnect(client);
            break;
        }
        case HTTP::HServer::SERVER_STATUS_STARTED: {
            this->Disconnect(client);
            break;
        }
        case HTTP::HServer::SERVER_STATUS_CLOSED: {
            this->Disconnect(client);
            break;
        }
        case HTTP::HServer::SERVER_STATUS_ERROR: {
            this->Disconnect(client);
            break;
        }
        case HTTP::HServer::SERVER_STATUS_CLIENT_CONTROLLER: {
            this->Disconnect(client);
            break;
        }
        default:
                break;
        }
        });
}
void HTTP::HServer::stop() {
    if (this->isRun) {
        this->isRun = false;
        for (SOCKET it : cSockets) {
            this->Disconnect(it);
        }
        closesocket(this->listenSocket);
        WSACleanup();
    }
}

bool HTTP::HServer::Send(SOCKET client, const Byteset data, HTTP::HTTPHANDLE headers, HTTP::HServer::SERVER_SENDTYPE dataType)
{
    switch (dataType) {
    case HTTP::HServer::SERVER_SENDTYPE_TEXT: return SendTypeText(client, data, headers);
    case HTTP::HServer::SERVER_SENDTYPE_BINARY: return SendTypeBinary(client, data, headers);
    default: return false;
    }
}

void HTTP::HServer::msgBlock(std::string path, std::string method, HTTPHANDLE headers, HTTP::HServer::MsgBlockCallback callback)
{
    std::string hpath = std::string(headers["Path"].begin(), headers["Path"].end());
    if (hpath.find("?") == std::string::npos) {
        if (Byteset(path) == headers["Path"] && Byteset(method) == headers["Method"] && callback) callback(this->ParseParams(headers));
    }
    else {
        std::string::size_type pos = hpath.find('?');
        std::string hpath_without_params = hpath.substr(0, pos);
        if (path == hpath_without_params && Byteset(method) == headers["Method"] && callback) callback(this->ParseParams(headers));
    }

}

void HTTP::HServer::Disconnect(SOCKET client) {
    {
        std::lock_guard<std::mutex> lock(thread.mtx);
        closesocket(client);
        for (auto it = cSockets.begin(); it != cSockets.end(); ++it) {
            if (*it == client) {
                cSockets.erase(it);
                break;
            }
        }
    }
    thread.remove(client, true);
    if (callback) callback(HTTP::HServer::SERVER_STATUS_DISCONNECTED, client);
}

int HTTP::HServer::GetClientCount() const {
    return cSockets.size();
}

std::string HTTP::HServer::GetClientIP(SOCKET client) {
    sockaddr_in clientAddr{};
    int len = sizeof(clientAddr);
    if (getsockname(client, (sockaddr*)&clientAddr, &len) == SOCKET_ERROR) return "";
    return inet_ntoa(clientAddr.sin_addr);
}

int HTTP::HServer::GetClientPort(SOCKET client) {
    sockaddr_in clientAddr{};
    int len = sizeof(clientAddr);
    if (getsockname(client, (sockaddr*)&clientAddr, &len) == SOCKET_ERROR) return -1;
    return ntohs(clientAddr.sin_port);
}

bool HTTP::HServer::IsClientDisconnect(SOCKET client)
{
    for (auto c : this->cSockets) {
        if (c == client) {
            return false;
        }
    }
    return true;
}

bool HTTP::HServer::_start(DWORD port, std::function<void(SERVER_STATUS status, SOCKET client)> callback)
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

Byteset HTTP::HServer::Listen(SOCKET client, size_t size, DWORD time)
{
    Byteset data;
    // 设置接收超时
    if (time != 0) { // 0 表示无限等待
        DWORD timeout = time;
        if (setsockopt(client, SOL_SOCKET, SO_RCVTIMEO,
            reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == SOCKET_ERROR) {
            this->Disconnect(client);
            return data;
        }
    }
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

HTTP::HTTPHANDLE HTTP::HServer::ResolveRequests(const Byteset& data)
{
    HTTP::HTTPHANDLE headers;
    std::istringstream stream(std::string(data.begin(), data.end()));
    std::string request_line;
    Byteset _data;
    std::getline(stream, request_line);
    request_line.erase(std::remove(request_line.begin(), request_line.end(), '\r'), request_line.end());
    std::istringstream line_stream(request_line);
    std::string method, path, protocol;
    line_stream >> method >> path >> protocol;
    headers["Method"] = method;
    headers["Path"] = path;
    headers["Protocol"] = protocol;
    std::string line;
    while (std::getline(stream, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        size_t colon = 0;
        if (line.empty()) {
            colon = stream.str().find("\r\n\r\n") + std::string("\r\n\r\n").size();
            if (colon != std::string::npos) {
                _data = data.subBytes(colon, data.size() - colon - 1);
                headers["HTTPDATA"] = std::string(_data.begin(), _data.end());
                    stream.str().substr(colon, stream.str().size() - 1);
                if (headers["content-type"].find("boundary=") != std::string::npos) {
                    headers["boundary"] = headers["content-type"].subBytes(headers["content-type"].find("boundary=") + Byteset("boundary=").size());
                    std::istringstream _stream = std::istringstream((PCHAR)headers["HTTPDATA"].data());
                    while (std::getline(_stream, line)) {
                        colon = line.find(": ");
                        if (colon != std::string::npos) {
                            std::string key = line.substr(0, colon);
                            size_t value_start = line.find_first_not_of(' ', colon + 1);
                            std::string value = (value_start != std::string::npos) ? line.substr(value_start) : "";
                            headers[key] = value;
                        }
                        else  if(line.find((PCHAR)headers["content-type"].subBytes(headers["content-type"].find("boundary=") + std::string("boundary=").size()).data()) != std::string::npos) {
                            break;
                        }
                    }
                    colon = data.find("\r\n\r\n", data.find("Content-Type:")) + std::string("\r\n\r\n").size();
                    if (colon != std::string::npos) {
                        size_t size = data.size() - colon - Byteset("\r\n--").append(headers["boundary"]).append("--\r\n").size();
                        _data = data.subBytes(colon, size);
                        headers["file"] = std::string(_data.begin(), _data.end());
                    }
                }
            }
        }
        else {
            colon = line.find(": ");
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                size_t value_start = line.find_first_not_of(' ', colon + 1);
                std::string value = (value_start != std::string::npos) ? line.substr(value_start) : "";
                headers[key] = value;
            }
        }

    }
    return headers;
}

HTTP::HTTPPARAMS HTTP::HServer::ParseParams(HTTP::HTTPHANDLE headers)
{
    HTTP::HTTPPARAMS params;
    // 通用查询参数处理
    if (headers["Path"].find("?") != std::string::npos) {
        Byteset byte = headers["Path"].subBytes(headers["Path"].find("?") + 1);
        std::string query = std::string(byte.begin(), byte.end());
        std::vector<std::string> pairs = Text::text_split(query, "&");
        for (const std::string& pair : pairs) {
            std::vector<std::string> kv = Text::text_split(pair, "=");
            if (kv.size() == 2) {
                std::string key = kv[0];
                std::string value = kv[1];

                // 定义正则表达式
                static const std::regex intRegex(R"(^[-+]?\d+$)");
                static const std::regex doubleRegex(R"(^[-+]?(\d+\.?\d*|\.\d+)([eE][-+]?\d+)?$)");
                static const std::regex boolRegex(R"(^(true|false)$)", std::regex::icase);
                // 类型匹配
                if (std::regex_match(value, intRegex)) {
                    // 整数类型
                    params[key] = std::stoll(value);
                }
                else if (std::regex_match(value, doubleRegex)) {
                    // 浮点类型（包含科学计数法）
                    params[key] = std::stod(value);
                }
                else {
                    // 布尔类型检测
                    std::string lowerVal = value;
                    std::transform(lowerVal.begin(), lowerVal.end(), lowerVal.begin(),
                        [](unsigned char c) { return std::tolower(c); });

                    if (std::regex_match(lowerVal, boolRegex)) {
                        params[key] = lowerVal == "true" ? true : false;
                    }
                    else {
                        // 默认文本类型
                        params[key] = value;
                    }
                }
            }
        }
    }
    else {
        std::istringstream stream((PCHAR)headers["HTTPDATA"].data());
        Json::CharReaderBuilder reader;
        JSONCPP_STRING errs;
        Json::parseFromStream(reader, stream, &params, &errs);
    }
    return params;
}