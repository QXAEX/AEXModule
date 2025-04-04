#pragma once
namespace TCPPlusServer {
	class Server {
	public:
		Server();
		~Server();
		/*
		* 启动
		* @param port 端口号
		* @return bool 是否成功
		*/
		bool start(int port);
		/*
		* 停止
		*/
		void stop();
	};
};