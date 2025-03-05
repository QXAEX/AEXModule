#pragma once
#include "R3.h"
namespace MemoryEmbeddedCall {
	/*
	* Ƕ��ʽ������
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
		* ��ʼ��
		* @param pid ����ID
		* @param size �ڴ��С
		* @return �Ƿ��ʼ���ɹ�
		*/
		bool Init(DWORD pid, size_t size);
		/*
		* �ͷ���Դ
		*/
		void Unit();
		/*
		* �����ڴ��С(�����������ڴ�ռ�)
		* @param size �ڴ��С
		* @return �Ƿ����ɹ�
		*/
		bool AllocateMemory(size_t size);
		/*
		* �������
		* @param code ����
		* @param index ��ʼλ��,Ĭ��Ϊ0
		* @return �Ƿ����óɹ�
		*/
		bool SetCode(Byteset code, size_t index = NULL);
		/*
		* ���ú���
		* @param index ������ʼλ��,��0��ʼ
		* @param args ��������
		* @return ��������ֵ
		*/
		template<typename Ret, typename... Args>
		Ret Call(int index, Args... args) {
			if (this->address == NULL) return Ret();
			PVOID addr = reinterpret_cast<PVOID>(reinterpret_cast<size_t>(this->address) + index);
			return (reinterpret_cast<Ret(WINAPI*)(Args...)>(addr))(args...);
		}
	};
};