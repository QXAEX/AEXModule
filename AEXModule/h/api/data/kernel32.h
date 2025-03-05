#pragma once
#include <windows.h>
#include <TlHelp32.h>
namespace KERNEL32 {
	extern HMODULE Kernel32;
	/*
	* �������̿���
	* @param dwFlags ��������
	* @param th32ProcessID ����ID
	* @return ���վ��
	*/
	HANDLE WINAPI CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
	/*
	* �̵߳�һ����Ϣ
	* @param hSnapshot ���վ��
	* @param lpte �߳���Ϣ�ṹ��
	* @return �Ƿ�ɹ�
	*/
	BOOL WINAPI Thread32First(HANDLE hSnapshot, PTHREADENTRY32 lpte);
	/*
	* �߳���һ����Ϣ
	* @param hSnapshot ���վ��
	* @param lpte �߳���Ϣ�ṹ��
	* @return �Ƿ�ɹ�
	*/
	BOOL WINAPI Thread32Next(HANDLE hSnapshot, PTHREADENTRY32 lpte);
};