#pragma once
#include <windows.h>
#include <vector>
#include <functional>
#include <string>
#include "../../../Thread.h"
#include "../../../Byteset.h"
namespace TCPPlusServer {
	typedef class Server {
	public:
		typedef enum SERVER_STATUS {
			SERVER_STATUS_CLIENTLINK_ERROR,//客户端连接错误
			SERVER_STATUS_CONNECTED,//用户连接
			SERVER_STATUS_DISCONNECTED,//用户断开连接
			SERVER_STATUS_STARTED,//服务器启动
			SERVER_STATUS_CLOSED,//服务器关闭
			SERVER_STATUS_ERROR,//服务器错误
			SERVER_STATUS_CLIENT_CONTROLLER,//客户端控制消息(用来处理客户端的通讯，内部自循环体)
		};
		/*
		* @brief 全局监听数据回调
		* @param status 服务器状态
		* @param client 客户端套接字，如果status是为NULL则错误来自本地服务
		*/
		typedef std::function<void(SERVER_STATUS status, SOCKET client)> Callback;
	public:
		Server() = default;
		~Server();
		/*
		* @brief 开始
		* @param port 端口号
		* @param callback 回调函数, 可空
		* @return bool 是否成功
		*/
		bool start(DWORD port, TCPPlusServer::Server::Callback callback = NULL);
		/*
		* @brief 停止
		* @return void
		*/
		void stop();
		/*
		* @brief 监听数据
		* @param client 客户端套接字
		* @param size 接收数据大小, 0表示接收全部数据, 其他值表示接收指定大小数据, 超过指定大小则自动丢弃, 默认接收全部数据
		* @return 数据
		*/
		Byteset Listen(SOCKET client, size_t size = NULL);
		/*
		* @brief 发送数据
		* @param client 客户端套接字
		* @param data 发送数据
		* @return bool 是否成功
		*/
		bool Send(SOCKET client, const Byteset data);
		/*
		* @brief 断开客户
		* @param client 客户端套接字
		* @return void
		*/
		void Disconnect(SOCKET client);
		/*
		* @brief 获取客户端数量
		* @return int 客户端数量
		*/
		int GetClientCount();
		/*
		* @brief 获取客户IP
		* @param client 客户端套接字
		* @return std::string 客户IP
		*/
		std::string GetClientIP(SOCKET client);
		/*
		* @brief 获取客户端口
		* @param client 客户端套接字
		* @return int 客户端口
		*/
		int GetClientPort(SOCKET client);
		/*
		* @brief 客户是否断开
		* @param client 客户端套接字
		* @return bool 是否断开
		*/
		bool IsClientDisconnect(SOCKET client);
	private:
		BOOL isRun;//是否运行
		DWORD port;//端口号
		WSADATA wsaData;//套接字数据
		SOCKET listenSocket;//监听套接字
		std::vector<SOCKET> cSockets;//客户端套接字
		Thread thread;//客户线程
		Callback callback;//全局回调函数
	};
};