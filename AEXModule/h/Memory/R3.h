#pragma once
#include "../Byteset.h"
#include "../Text.h"
#include <Windows.h>
namespace MemoryR3 {
	/*
	* �ڴ��д��
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
		* �򿪽���
		* @param pid ����ID
		*/
		bool WINAPI Open(DWORD pid);
		/*
		* �رս�������
		*/
		void WINAPI Close();
		/*
		* �Ƿ�64λ����
		* @return �Ƿ�64λ����,true:64λ,false:32λ
		*/
		bool WINAPI Is64() const;
		/*
		* ��ģ���ַ
		* @param moduleName ģ������
		* @return ģ���ַ
		*/
		PVOID WINAPI GetModuleBase(std::string moduleName) const;
		/*
		* д�ڴ�
		* @param address �ڴ��ַ
		* @param value д��ֵ
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��true
		* @return �Ƿ�д��ɹ�
		*/
		bool WINAPI Write(PVOID address, Byteset value, bool isVirtual = true) const;
		/*
		* ���ڴ�
		* @param address �ڴ��ַ
		* @param value ��ȡֵ
		* @param rlength ��ȡ����
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��true
		* @return �Ƿ��ȡ�ɹ�
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
		* �޸��ڴ�����
		* @param address �ڴ��ַ
		* @param size �ڴ��С
		* @param protection ����ģʽ
		* @param oldProtection ԭ����ģʽ
		* @return �Ƿ��޸ĳɹ�
		*/
		bool WINAPI VirtualProtect(PVOID address, ULONG_PTR size, DWORD protection, PDWORD oldProtection) const;
		/*
		* �����ڴ�ռ�
		* @param size �����С, Ĭ��1024�ֽ�
		* @return ���뵽���ڴ��ַ
		*/
		size_t WINAPI Allocate(DWORD size = 1024) const;
		/*
		* �ͷ��ڴ�ռ�
		* @param address �ڴ��ַ
		* @param size �ڴ��С, Ĭ��1024�ֽ�
		* @return �Ƿ��ͷųɹ�
		* ע��: �ͷŵ��ڴ��������Allocate�����
		*/
		bool WINAPI Free(PVOID address, DWORD size = 1024) const;
		/*
		* �����ڴ�
		* @param buffer Ҫ�������ֽڴ�
		* @param vagueContent ģ�����ҵĺ����ֽڼ�,Ĭ��Ϊnullptr
		* @param startAddress �ڴ��ַ(��ʼ), Ĭ��NULL
		* @param endAddress �ڴ��ַ(����), Ĭ��-1(������������)
		* @param type ��ѯ�ڴ�����(Ĭ��-1),-1:All,0:�ɶ���д��ִ��,1:�ɶ�д,2:ֻ��,3:��д��ִ��,4:�ɶ���ִ��,5:��ִ��
		* @param limit �������� 0:������, Ĭ��10
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��false
		* @return �ҵ����ڴ��ַ����
		*/
		std::vector<PVOID> WINAPI Search(Byteset buffer, Byteset vagueContent = nullptr, PVOID startAddress = nullptr, PVOID endAddress = nullptr, int type = -1, size_t limit = 10, bool isVirtual = false) const;
		/*
		* hook��ת(64λhook���ֽ��볤��Ϊ14�ֽ�,32λhook���ֽ��볤��Ϊ5�ֽ�,ע�ⲹ��)
		* @param address �ڴ��ַ
		* @param newAddress ����ת��ַ
		* @param complement Ҫ������ֽڼ�,Ĭ��Ϊnullptr,�����ǰ����Ϊ64λ����,��hook�����ݳ���Ϊ14�ֽ�
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��true
		* @return �Ƿ���ת�ɹ�
		*/
		bool WINAPI hookJump(PVOID address, PVOID newAddress, Byteset complement = nullptr, bool isVirtual = true) const;
		/*
		* ָ������
		* @param address �ڴ��ַ
		* @param offsets ָ��ƫ�Ƽ���,Ĭ��Ϊnullptr
		* @param isVirtual �Ƿ������Ᵽ��, Ĭ��false
		* @return ������ָ��ֵ
		*/
		Byteset WINAPI pointerChain(PVOID address, std::initializer_list<size_t> offsets = {}, bool isVirtual = false) const;
	};
};