#pragma once
#include <Windows.h>
namespace MemoryInject {
	/*
	* ע����
	* ����ע��dll�ļ���ָ�����̵��ڴ���
	*/
	class Inject {
	public:
		/*
		* ע��dll�ļ���ָ�����̵��ڴ���
		* @param processPid ����id
		* @param dllPath dll�ļ�·��
		* @return ע���Ƿ�ɹ�
		*/
		bool InjectDll(INT processPid, LPCSTR dllPath);
	private:
		/*
		* ��ȡ���������ƫ��
		* @param lpReflectiveDllBuffer ������������ڴ��ַ
		* @return ���������ƫ��
		*/
		DWORD GetReflectiveLoaderOffset(PVOID lpReflectiveDllBuffer);
		/*
		* ����dll�ļ�
		* @param lpBuffer dll�ļ��ڴ��ַ
		* @param dwLength dll�ļ�����
		* @return ����dll�ļ����
		*/
		HMODULE WINAPI LoadLibraryR(LPVOID lpBuffer, DWORD dwLength);
		/*
		* ��ȡdll�ļ�����������ַ
		* @param hModule dll�ļ����
		* @param lpProcName ������������
		* @return ����������ַ
		*/
		FARPROC WINAPI GetProcAddressR(HANDLE hModule, LPCSTR lpProcName);
		/*
		* Զ�̼���dll�ļ�
		* @param hProcess ���̾��
		* @param lpBuffer dll�ļ��ڴ��ַ
		* @param dwLength dll�ļ�����
		* @param lpParameter Զ�̼��ز���
		* @return ����dll�ļ����
		*/
		HANDLE WINAPI LoadRemoteLibraryR(HANDLE hProcess, LPVOID lpBuffer, DWORD dwLength, LPVOID lpParameter);
	};
};