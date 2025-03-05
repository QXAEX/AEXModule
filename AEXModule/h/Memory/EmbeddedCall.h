#pragma once
#include "R3.h"
namespace MemoryEmbeddedCall {
	/*
	* 嵌入式调用类
	*/
	class EmbeddedCall : private MemoryR3::R3 {
	private:
		typedef bool (WINAPI* embeddedCall)(int param1);
	private:
		DWORD PID;
		PVOID address;
		size_t size;

	public:
		EmbeddedCall();
		EmbeddedCall(DWORD pid, size_t size);
		~EmbeddedCall();
		/*
		* 初始化
		* @param pid 进程ID
		* @param size 内存大小
		* @return 是否初始化成功
		*/
		bool Init(DWORD pid, size_t size);
		/*
		* 释放资源
		*/
		void Unit();
		/*
		* 分配内存大小(可用于扩充内存空间)
		* @param size 内存大小
		* @return 是否分配成功
		*/
		bool AllocateMemory(size_t size);
		/*
		* 置入代码
		* @param code 代码
		* @param index 起始位置,默认为0
		* @return 是否设置成功
		*/
		bool SetCode(Byteset code, size_t index = NULL);
		/*
		* 调用函数
		* @param index 函数起始位置,从0开始
		* @param args 函数参数
		* @return 函数返回值
		*/
		template<typename Ret, typename... Args>
		Ret Call(int index, Args... args) {
			if (this->address == NULL) return Ret();
			PVOID addr = reinterpret_cast<PVOID>(reinterpret_cast<size_t>(this->address) + index);
			return (reinterpret_cast<Ret(WINAPI*)(Args...)>(addr))(args...);
		}
	};
};