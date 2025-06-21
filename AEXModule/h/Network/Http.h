#pragma once
#include <functional>
#include <map>
#include <string>
#include <algorithm>
#include <windows.h>
#include "../Byteset.h"
#include "../Thread.h"
#include "../../json/include/json/json.h"
namespace HTTP {
	using HTTPPARAMS = Json::Value;//请求参数
	using HTTPHANDLE = std::map<std::string, Byteset>;//请求头
	bool SendTypeText(SOCKET client, Byteset data, HTTP::HTTPHANDLE headers);// 发送文本数据
	bool SendTypeBinary(SOCKET client, Byteset data, HTTP::HTTPHANDLE headers);// 发送二进制数据
	typedef class HClient {
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
		typedef std::function<void(HTTP::HClient::CLIENT_STATUS status, Byteset recvData)> SendCalback;
		enum HTTP_METHOD {
			HTTP_GET,
			HTTP_POST,
			HTTP_PUT,
			HTTP_DELETE,
		};
	public:
		HClient() = default;
		HClient(std::string ip, DWORD port);
		~HClient();
		/*
		* @brief 设置服务端信息
		* @param ip 服务器IP地址
		* @param port 服务器端口号
		* @return 无
		*/
		void setServerInfo(std::string ip, DWORD port);
		/*
		* @brief 发送数据，请求模式Request
		* @param data 要发送的数据(主体数据，POST等请求可用于传Json结构)
		* @param url 请求的url地址
		* @param method 请求方法，默认HTTP_GET
		* @param callback 发送回调函数，可选参数，默认为空(发送数据但不接收回传数据)
		* @param params 请求参数，可选参数，默认为空
		* @param header 自定义请求头，可选参数，默认为空
		* @param size 接收数据大小, 0表示接收全部数据, 其他值表示接收指定大小数据, 超过指定大小则自动丢弃, 默认接收全部数据
		* @param time 超时时间，单位：毫秒，NULL表示不超时, 默认10秒
		* @return 无
		*/
		void Request(Byteset data, std::string url, HTTP_METHOD method = HTTP_GET, HTTP::HClient::SendCalback callback = nullptr, HTTPPARAMS params = {}, HTTPHANDLE header = std::map<std::string, Byteset>(), size_t size = NULL, DWORD time = 10000);
		/*
		* @brief 设置是否保持连接
		* @param keepAlive 是否保持连接
		* @return void
		* @note 保持连接会在请求或响应时保持TCP连接，减少TCP连接建立和断开的开销，提高通信效率。
		* @note 但是如果长时间没有数据交互，则会导致连接断开，需要客户端定时发送数据来保持连接。
		*/
		void setKeepAlive(BOOL keepAlive);
	public:
		std::string ip = "127.0.0.1";
		DWORD port = 80;
		PVOID client;//TCP客户端
		BOOL keepAlive_ = FALSE;//是否保持连接
	private:
		BOOL isRun = false;//是否连接
		SOCKET clientSocket;//客户端套接字
		WSADATA wsaData;//套接字数据
	private:
		CLIENT_STATUS start(std::string ip, DWORD port);
		CLIENT_STATUS stop();
		bool Send(Byteset data, HTTP::HClient::SendCalback callback = nullptr, size_t size = NULL, DWORD time = NULL);
	private:
		std::string urlEncode(const std::string& value);
		std::string buildQuery(const HTTPPARAMS& params);
		std::string methodToString(HTTP::HClient::HTTP_METHOD method);
		//处理http客户端请求数据结构
		Byteset HandleRequest(Byteset data, std::string url, HTTP::HClient::HTTP_METHOD method, HTTPPARAMS params, HTTPHANDLE header);
		//处理http客户端响应数据结构
		Byteset HandleResponse(Byteset data, std::string url, HTTP::HClient::HTTP_METHOD method, HTTPPARAMS params);
	};
	typedef class HServer {
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
		enum SERVER_SENDTYPE {
			SERVER_SENDTYPE_TEXT,
			SERVER_SENDTYPE_BINARY
		};
		/*
		* @brief 全局监听数据回调
		* @param client 客户端套接字，如果status是为NULL则错误来自本地服务
		* @param headers 请求头
		*/
		typedef std::function<void(SOCKET client, HTTP::HTTPHANDLE headers)> Callback;
		/*
		* @brief 消息块回调
		*/
		typedef std::function<void(HTTPPARAMS params)> MsgBlockCallback;
	public:
		HServer() = default;
		~HServer();
		/*
		* @brief 开始
		* @param port 端口号
		* @param callback 回调函数, 可空
		* @return bool 是否成功
		*/
		bool start(DWORD port, HTTP::HServer::Callback callback = NULL);
		/*
		* @brief 停止
		* @return void
		*/
		void stop();
		/*
		* @brief 发送数据
		* @param client 客户端套接字
		* @param data 发送数据
		* @param headers 请求头
		* @param dataType 数据发送类型, 默认为TEXT
		* @return bool 是否成功
		*/
		bool Send(SOCKET client, const Byteset data, HTTP::HTTPHANDLE headers, HTTP::HServer::SERVER_SENDTYPE dataType = HTTP::HServer::SERVER_SENDTYPE_TEXT);
		/*
		* @brief 注册消息块
		* @param path 路径，自己指定的API路径如"/api/login"
		* @param method 请求方法, 自己指定此API事件的有效请求类型，如"POST"、"GET"等
		* @param headers 请求头
		* @param callback 回调函数
		* @return void
		*/
		void msgBlock(std::string path, std::string method, HTTP::HTTPHANDLE headers, HTTP::HServer::MsgBlockCallback callback);
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
		int GetClientCount() const;
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
	public:
		std::vector<SOCKET> cSockets;//客户端套接字
	private:
		BOOL isRun;//是否运行
		DWORD port;//端口号
		WSADATA wsaData;//套接字数据
		SOCKET listenSocket;//监听套接字
		Thread thread;//客户线程
		std::function<void(SERVER_STATUS status, SOCKET client)> callback;//全局回调函数
	private:
		bool _start(DWORD port, std::function<void(SERVER_STATUS status, SOCKET client)> callback);
		Byteset Listen(SOCKET client, size_t size = NULL, DWORD time = NULL);
		HTTP::HTTPHANDLE ResolveRequests(const Byteset& data);
		HTTP::HTTPPARAMS ParseParams(HTTP::HTTPHANDLE path);
	};
};