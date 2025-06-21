#pragma once
#include <windows.h>
#include <string>
#include <functional>
#include <vector>
#include "../Thread.h"
#include "../Byteset.h"
namespace TCPPlus {
	typedef class Client {
	public:
		enum CLIENT_STATUS {
			CLIENT_STATUS_ERROR,//错误
			CLIENT_STATUS_CONNECTED,//开始连接
			CLIENT_STATUS_CONNECTED_SUCCESS,//连接成功
			CLIENT_STATUS_CONNECTED_FAILED,//连接失败
			CLIENT_STATUS_STOPPED,//停止连接
			CLIENT_STATUS_SEND_SUCCESS,//发送成功
			CLIENT_STATUS_SEND_FAILED,//发送失败
			CLIENT_STATUS_RECV_SUCCESS,//接收成功
			CLIENT_STATUS_RECV_FAILED,//接收失败
			CLIENT_STATUS_RECV_TIMEOUT,//接收超时
			CLIENT_STATUS_RECV_ERROR,//接收错误
			CLIENT_STATUS_SERVER_CLOSED,//服务器主动断开连接(如果已连接的状态下触发了此事件则不会触发第二次，除非重新连接)
			CLIENT_STATUS_DATA_EMPTY,//数据为空
		};
		/*
		* @brief 发送回调函数
		* @param status 发送状态
		* @param recvData 接收到的数据
		* @return void
		*/
		typedef std::function<void(CLIENT_STATUS status, Byteset recvData)> SendCalback;
	public:
		Client() = default;
		~Client();
		/*
		* @brief 开始
		* @param ip 服务器IP地址
		* @param port 服务器端口号
		* @return bool 成功返回true，失败返回false
		*/
		CLIENT_STATUS start(std::string ip, DWORD port);
		/*
		* @brief 停止
		* @return void
		*/
		CLIENT_STATUS stop();
		/*
		* @brief 发送数据
		* @param data 要发送的数据
		* @param callback 发送回调函数，可选参数，默认为空(发送数据但不接收回传数据)
		* @param size 接收数据大小, 0表示接收全部数据, 其他值表示接收指定大小数据, 超过指定大小则自动丢弃, 默认接收全部数据
		* @param time 超时时间，单位：毫秒，NULL表示不超时
		* @return 是否成功
		*/
		bool Send(Byteset data, TCPPlus::Client::SendCalback callback = nullptr, size_t size = NULL, DWORD time = NULL);
	private:
		BOOL isRun = false;//是否连接
		SOCKET clientSocket;//客户端套接字
		WSADATA wsaData;//套接字数据
	};
	typedef class Server {
	public:
		enum SERVER_STATUS {
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
		bool start(DWORD port, TCPPlus::Server::Callback callback = NULL);
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