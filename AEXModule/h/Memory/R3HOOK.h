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
	* �ڴ湳����
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
		* �����Ƿ������Ᵽ��
		* @param isVirtual �Ƿ������Ᵽ��
		*/
		void setVirtual(bool isVirtual);
		/*
		* ʵʱ��ȡ(������Ҫ����Capture�����������ȡ)
		* @param flag �Ƿ�����,true:����,false:����
		*/
		void setCapture(bool flag);
		/*
		* ��װ����(64λhook���ֽ��볤��Ϊ14�ֽ�,32λhook���ֽ��볤��Ϊ5�ֽ�,ע�ⲹ��)��д����Ĭ������������
		* @param pid ����ID
		* @param address �ڴ��ַ
		* @param callBack ���ӻص�����
		* @param complement Ҫ������ֽڼ�,Ĭ��Ϊnullptr
		* @return �Ƿ�װ�ɹ�
		*/
		bool Install(DWORD pid, PVOID address, Byteset complement = nullptr);
		/*
		* ��ȡ����
		*/
		void Capture();
		/*
		* ��ȡ����
		* @return ��ȡ������
		*/
		R3HOOK_INFO ReadData() const;
		/*
		* ж�ع���
		*/
		void Uninstall();
	};
};