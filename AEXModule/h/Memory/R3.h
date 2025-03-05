#pragma once
#include "../Byteset.h"
#include "../Text.h"
#include <Windows.h>
namespace MemoryR3 {
	/*
	* 内存读写类
	*/
	class R3 {
	public:
		HANDLE hProcess;
		DWORD PID;
	public:
		R3() = default;
		R3(DWORD pid);
		~R3();
		/*
		* 打开进程
		* @param pid 进程ID
		*/
		bool WINAPI Open(DWORD pid);
		/*
		* 关闭进程链接
		*/
		void WINAPI Close();
		/*
		* 是否64位进程
		* @return 是否64位进程,true:64位,false:32位
		*/
		bool WINAPI Is64() const;
		/*
		* 读模块基址
		* @param moduleName 模块名称
		* @return 模块基址
		*/
		PVOID WINAPI GetModuleBase(std::string moduleName) const;
		/*
		* 写内存
		* @param address 内存地址
		* @param value 写入值
		* @param isVirtual 是否破虚拟保护, 默认true
		* @return 是否写入成功
		*/
		bool WINAPI Write(PVOID address, Byteset value, bool isVirtual = true) const;
		/*
		* 读内存
		* @param address 内存地址
		* @param value 读取值
		* @param rlength 读取长度
		* @param isVirtual 是否破虚拟保护, 默认true
		* @return 是否读取成功
		*/
		bool WINAPI Read(PVOID address, Byteset& value, DWORD rlength, bool isVirtual = true) const;
		template<typename T>
		bool WINAPI Read(PVOID address, T& value, bool isVirtual = true) const {
			Byteset bs;
			if (!Read(address, bs, sizeof(T), isVirtual)) {
				return false;
			}
			value = *(T*)(bs.data());
			return true;
		}
		/*
		* 修改内存属性
		* @param address 内存地址
		* @param size 内存大小
		* @param protection 保护模式
		* @param oldProtection 原保护模式
		* @return 是否修改成功
		*/
		bool WINAPI VirtualProtect(PVOID address, ULONG_PTR size, DWORD protection, PDWORD oldProtection) const;
		/*
		* 申请内存空间
		* @param size 申请大小, 默认1024字节
		* @return 申请到的内存地址
		*/
		size_t WINAPI Allocate(DWORD size = 1024) const;
		/*
		* 释放内存空间
		* @param address 内存地址
		* @param size 内存大小, 默认1024字节
		* @return 是否释放成功
		* 注意: 释放的内存必须是由Allocate申请的
		*/
		bool WINAPI Free(PVOID address, DWORD size = 1024) const;
		/*
		* 搜索内存
		* @param buffer 要搜索的字节串
		* @param vagueContent 模糊查找的忽略字节集,默认为nullptr
		* @param startAddress 内存地址(起始), 默认NULL
		* @param endAddress 内存地址(结束), 默认-1(搜索整个进程)
		* @param type 查询内存属性(默认-1),-1:All,0:可读可写可执行,1:可读写,2:只读,3:可写可执行,4:可读可执行,5:可执行
		* @param limit 查找数量 0:不限制, 默认10
		* @param isVirtual 是否破虚拟保护, 默认false
		* @return 找到的内存地址集合
		*/
		std::vector<PVOID> WINAPI Search(Byteset buffer, Byteset vagueContent = nullptr, PVOID startAddress = nullptr, PVOID endAddress = nullptr, int type = -1, size_t limit = 10, bool isVirtual = false) const;
		/*
		* hook跳转(64位hook的字节码长度为14字节,32位hook的字节码长度为5字节,注意补码)
		* @param address 内存地址
		* @param newAddress 新跳转地址
		* @param complement 要补充的字节集,默认为nullptr,如果当前环境为64位程序,则hook的数据长度为14字节
		* @param isVirtual 是否破虚拟保护, 默认true
		* @return 是否跳转成功
		*/
		bool WINAPI hookJump(PVOID address, PVOID newAddress, Byteset complement = nullptr, bool isVirtual = true) const;
		/*
		* 指针连读
		* @param address 内存地址
		* @param offsets 指针偏移集合,默认为nullptr
		* @param isVirtual 是否破虚拟保护, 默认false
		* @return 连读的指针值
		*/
		Byteset WINAPI pointerChain(PVOID address, std::initializer_list<size_t> offsets = {}, bool isVirtual = false) const;
	};
};