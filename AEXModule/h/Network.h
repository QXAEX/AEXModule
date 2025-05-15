#pragma once
#include "./Network/tcp/tcp.h"
#include "./Network/TCPPlus/TCPPlus.h"
#include "./Network/HTTP/HTTP.h"
/*
* 网络库
*/
namespace Network {
	using namespace Network_TCP;//TCP 1.0 版本
	namespace TCPPlus = TCPPlus;//TCP 2.0 版本
	namespace HTTP = HTTP;//HTTP 通讯
};
