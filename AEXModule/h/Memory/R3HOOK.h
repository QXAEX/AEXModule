#pragma once
#include "R3.h"
namespace MemoryR3HOOK {
#pragma pack(push, 1)
#ifdef _WIN64
	typedef struct _R3HOOK_INFO {
		unsigned long long rax; unsigned int eax; unsigned short ax; unsigned char al;
		unsigned long long rcx; unsigned int ecx; unsigned short cx; unsigned char cl;
		unsigned long long rdx; unsigned int edx; unsigned short dx; unsigned char dl;
		unsigned long long rbx; unsigned int ebx; unsigned short bx; unsigned char bl;
		unsigned long long rsp; unsigned int esp; unsigned short sp; unsigned char spl;
		unsigned long long rbp; unsigned int ebp; unsigned short bp; unsigned char bpl;
		unsigned long long rsi; unsigned int esi; unsigned short si; unsigned char sil;
		unsigned long long rdi; unsigned int edi; unsigned short di;	unsigned char dil;
		unsigned long long r8; unsigned int r8d; unsigned short r8w; unsigned char r8b;
		unsigned long long r9; unsigned int r9d; unsigned short r9w; unsigned char r9b;
		unsigned long long r10; unsigned int r10d; unsigned short r10w; unsigned char r10b;
		unsigned long long r11; unsigned int r11d; unsigned short r11w; unsigned char r11b;
		unsigned long long r12; unsigned int r12d; unsigned short r12w; unsigned char r12b;
		unsigned long long r13; unsigned int r13d; unsigned short r13w; unsigned char r13b;
		unsigned long long r14; unsigned int r14d; unsigned short r14w; unsigned char r14b;
		unsigned long long r15; unsigned int r15d; unsigned short r15w; unsigned char r15b;
	} R3HOOK_INFO, * PR3HOOK_INFO;
#else
	typedef struct _R3HOOK_INFO {
		unsigned int eax; unsigned short ax; unsigned char al;
		unsigned int ecx; unsigned short cx; unsigned char cl;
		unsigned int edx; unsigned short dx; unsigned char dl;
		unsigned int ebx; unsigned short bx; unsigned char bl;
		unsigned int esp; unsigned short sp; unsigned int ebp;
		unsigned short bp; unsigned int esi; unsigned short si;
		unsigned int edi; unsigned short di;
	} R3HOOK_INFO, * PR3HOOK_INFO;
#endif
#pragma pack(pop)
	/*
	* 内存钩子类
	*/
	class R3HOOK : private MemoryR3::R3 {
	private:
		bool isInstalled;
		bool is64_;
		bool isVirtual;
		bool isCapture;
		PVOID address;
		Byteset complement;
		Byteset temp;
		PVOID newAddress;
		DWORD PID;
	public:
		R3HOOK();
		R3HOOK(DWORD pid, PVOID address, Byteset complement = nullptr);
		~R3HOOK();
		/*
		* 设置是否破虚拟保护
		* @param isVirtual 是否破虚拟保护
		*/
		void setVirtual(bool isVirtual);
		/*
		* 实时截取(不再需要调用Capture函数来请求截取)
		* @param flag 是否启用,true:启用,false:禁用
		*/
		void setCapture(bool flag);
		/*
		* 安装钩子(64位hook的字节码长度为14字节,32位hook的字节码长度为5字节,注意补码)，写数据默认启动破虚拟
		* @param pid 进程ID
		* @param address 内存地址
		* @param callBack 钩子回调函数
		* @param complement 要补充的字节集,默认为nullptr
		* @return 是否安装成功
		*/
		bool Install(DWORD pid, PVOID address, Byteset complement = nullptr);
		/*
		* 截取数据
		*/
		void Capture();
		/*
		* 读取数据
		* @return 读取的数据
		*/
		R3HOOK_INFO ReadData() const;
		/*
		* 卸载钩子
		*/
		void Uninstall();
	};
};