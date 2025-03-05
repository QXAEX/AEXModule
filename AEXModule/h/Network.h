#pragma once
#include <windows.h>
#include <vector>
#include "Byteset.h"
#include "AEXStore.h"
/*
* 注意:本库不兼容MT多线程模式,其他模式可以使用
* 同源代码,一个程序中,不能同时启用本库的TCP的客户端和服务端
*/
namespace Network {
    INT MSG(std::string message, int result = NULL, std::string title = "Error");
	namespace TCP {
        typedef struct _CLIENT {
            SOCKET handle;
            sockaddr_in addr;
            int state; //客户状态,对应 _SERVER 枚举状态码
            PVOID server;//服务端对象
        } CLIENT, * PCLIENT;
        enum _SPEED_UNIT {
            SPEED_BIT,
            SPEED_KB,
            SPEED_MB,
            SPEED_GB,
        };
        typedef struct _SERVER_MSG_INFO {
            PCLIENT client;//客户对象
            PFile file = new File();//待发送/接收数据(以文件为单位)
            PSIZE_T size = nullptr;//已发送/接收数据大小
            DOUBLE speed = NULL;//速度
            INT unit = SPEED_BIT;//速度单位(根据速度大小进行单位变换)(1:BIT -> 2:KB -> 3:MB -> 4:GB)
            int timeout = -1;//超时时间
            bool is_end = false;//是否结束
        } SERVER_MSG_INFO, * PSERVER_MSG_INFO;
        typedef struct _CLIENTS_MSG_INFO {
            PFile file = new File();//待发送/接收数据(以文件为单位)【每次接收数据前都需要手动清空数据,Recvs函数不会自动清空】
            PSIZE_T size = nullptr;//已发送/接收数据大小
            DOUBLE speed = NULL;//速度
            INT unit = SPEED_BIT;//速度单位(根据速度大小进行单位变换)(1:BIT -> 2:KB -> 3:MB -> 4:GB)
            int timeout = -1;//超时时间
            bool is_end = false;//是否结束
        } CLIENTS_MSG_INFO, * PCLIENTS_MSG_INFO;
        enum _SERVER {//服务码
            SERVER_CLIENT_DISCONNECTION,//客户断开
            SERVER_CLIENT_CONNECTION,//客户连接
            SERVER_DATA_RECV,//数据接收
            SERVER_DATA_RECV_FAILED,//数据接收失败
            SERVER_DATA_SEND,//数据发送
            SERVER_DATA_SEND_FAILED,//数据发送失败
            SERVER_TIMEOUT,//超时
        };
        // 服务端类
        typedef class Server {
        private:
            std::string ip;
            int port;
            SOCKET _socket_;
            sockaddr_in _serverAddr_;
            std::vector<CLIENT> clients;
        public:
            bool started;//服务是否启动中
        public:
            Server();
            ~Server();
            // 拷贝赋值操作符
            Network::TCP::Server& operator=(const Server& other);
            /*
            * 启动服务
            * @param port 服务端口
            * @return 启动成功返回true，否则返回false
            */
            bool start(int port);
            /*
            * 停止服务
            */
            void stop();
            /*
            * 监听
            * @param timeout 超时时间，单位为毫秒，-1表示无限等待
            * @return 返回连接的客户对象
            */
            CLIENT Listen(int timeout = -1);
            /*
            * 断开连接
            * @param client 客户对象
            * @return 成功返回true，否则返回false
            */
            bool disconnect(PCLIENT client);
            /*
            * 发送数据(限制10KB)【两个发送功能最好只用其中一个】
            * @param client 客户对象
            * @param data 待发送的数据
            * @param timeout 超时时间，单位为毫秒，-1表示无限等待
            * @return 成功返回true，否则返回false
            */
            bool Send(PCLIENT client,Byteset data,int timeout = -1) const;
            /*
            * 发送数据(不限量数据,对性能有较大影响)【两个发送功能最好只用其中一个】
            * @param info 发送信息
            * @return 成功返回true，否则返回false
            */
            bool Sends(PSERVER_MSG_INFO info) const;
            /*
            * 接收数据(限制10KB)【两个接收功能最好只用其中一个】
            * @param client 客户对象
            * @param timeout 超时时间，单位为毫秒，-1表示无限等待
            * @return 接收到的数据
            */
            Byteset Recv(PCLIENT client, int timeout = -1) const;
            /*
            * 接收数据(不限量数据,对性能有较大影响)【两个接收功能最好只用其中一个】
            * @param info 接收信息
            * @return 成功返回true，否则返回false
            */
            bool Recvs(PSERVER_MSG_INFO info) const;
            /*
            * 获取客户数量
            * @return 客户数量
            */
            size_t GetClientCount();
            /*
            * 获取客户对象
            * @param index 客户索引
            * @return 客户对象
            */
            PCLIENT GetClient(__int64 index);
            /*
            * 取错误码/事件状态
            * @return 错误码/事件状态
            */
            int GetLastError() const;
        } * PSERVER;
        // 客户端类
        class Client {
        private:
            SOCKET _socket_;
            sockaddr_in _serverAddr_;
        public:
            bool started;//客户端是否连接中
        public:
            Client();
            ~Client();
            // 拷贝赋值操作符
            Network::TCP::Client& operator=(const Client& other);
            /*
            * 连接服务端
            * @param ip 服务端IP地址
            * @param port 服务端端口
            * @return 连接成功返回true，否则返回false
            */
            bool Connect(const std::string& ip, int port);
            /*
            * 断开连接
            * @return 成功返回true，否则返回false
            */
            bool disconnect();
            /*
            * 发送数据【两个发送功能最好只用其中一个】
            * @param data 待发送的数据
            * @param timeout 超时时间，单位为毫秒，-1表示无限等待
            * @return 成功返回true，否则返回false
            */
            bool Send(Byteset data, int timeout = -1) const;
            /*
            * 发送数据(不限量数据,对性能有较大影响)【两个发送功能最好只用其中一个】
            * @param info 发送信息
            * @return 成功返回true，否则返回false
            */
            bool Sends(PCLIENTS_MSG_INFO info) const;
            /*
            * 接收数据(限制10KB)
            * @param timeout 超时时间，单位为毫秒，-1表示无限等待
            * @return 接收到的数据
            */
            Byteset Recv(int timeout = -1) const;
            /*
            * 发送数据(不限量数据,对性能有较大影响)【两个发送功能最好只用其中一个】
            * @param info 发送信息
            * @return 成功返回true，否则返回false
            */
            bool Recvs(PCLIENTS_MSG_INFO info) const;
            /*
            * 取错误码/事件状态
            * @return 错误码/事件状态
            */
            int GetLastError() const;
        };

	};
};
// TCP示例代码
/*
static void handleClient(Network::TCP::PCLIENT client) {
    Network::TCP::PSERVER server = reinterpret_cast<Network::TCP::PSERVER>(client->server);
    while (server->started) {
        Byteset data = server->Recv(client);
        if (client->state == Network::TCP::_SERVER::SERVER_CLIENT_DISCONNECTION ||
            client->state == Network::TCP::_SERVER::SERVER_DATA_RECV_FAILED ||
            client->state == Network::TCP::_SERVER::SERVER_DATA_SEND_FAILED
            ) {
            std::cout << "Client disconnected" << std::endl;
            server->disconnect(client);
            break;
        }
        std::cout << "1Received data: " << (char*)data.data() << std::endl;
        server->Send(client, "Hello World!");
    }
}

int main()
{

    //Network::TCP::Server server;
    //if (!server.start(4001)) {
    //    std::cerr << "Failed to start server" << std::endl;
    //    return 1;
    //}
    //std::cout << "Server started on port 4001" << std::endl;
    //while (true) {
    //    Network::TCP::CLIENT client = server.Listen(-1);
    //    if (client.state == Network::TCP::_SERVER::SERVER_CLIENT_CONNECTION) {
    //        std::cout << "Client connected" << std::endl;
    //        std::thread clientThread(handleClient, &client);
    //        clientThread.detach();
    //    }
    //}
    //return 0;

    Network::TCP::Client client;
    if (!client.Connect("127.0.0.1", 4001)) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }
    std::cout << "Connected to server" << std::endl;
    Byteset response;
    do {
        std::string dd;
        std::cin >> dd;
        client.Send(dd);
        response = client.Recv();
        std::cout << "Received data: " << response << std::endl;
    } while (response != "end");
    client.disconnect();
    std::cin.get();
    return 0;
}
*/