#pragma once
#include <windows.h>
#include "Text.h"
#include <TlHelp32.h>
namespace Process {
	//ö�ٽ�����Ϣ�б�
	struct ProcessInfo {
		DWORD PID = NULL;//����ID
		std::string Name = "";//������
		std::wstring WName = L"";//������(���ַ�)
		std::string WinTitle = "";//���̴��ڱ���
		HWND WinHandle = NULL;//���̴��ھ��
		std::string Path = "";//����·��
		std::string CommandLine ="";//����������
		std::vector<MODULEENTRY32> Modules = {};//ģ���б�(�����ȡΪ�գ��������ԱȨ��)
	};
	/*
	* ��ȡ����·��
	* @param processID ����ID
	* @return ����·��
	*/
	std::string WINAPI GetProcessPath(DWORD processID);
	/*
	* ��ȡ����������
	* @param processID ����ID
	* @return ����������
	*/
	std::string WINAPI GetProcessCommandLine(DWORD processID);
	/*
	* ��ȡ���ڱ���
	* @param hwnd ���ھ��
	* @return ���ڱ���
	*/
	std::string WINAPI GetWindowTitle(HWND hwnd);
	/*
	* ����IDȡ���ھ��
	* @param processID ����ID
	* @return ���ھ��,NULL��ʾδ�ҵ�
	*/
	HWND WINAPI GetProcessWinHandle(DWORD processID);
	/*
	* ��ȡ���н�����Ϣ
	* @return ���н��̵���Ϣ
	*/
	std::vector<ProcessInfo> WINAPI ProcessList();
	/*
	* ������ȡ����ID
	* @param processName ������
	* @return ����ID,-1��ʾδ�ҵ�
	*/
	DWORD WINAPI ProcessGetID(std::string processName);
	/*
	* ����IDȡ������
	* @param processID ����ID
	* @return ������,���ַ�����ʾδ�ҵ�
	*/
	std::string WINAPI ProcessGetName(DWORD processID);
	/*
	* ����IDȡ���ھ��
	* @param processID ����ID
	* @return ���ھ��,NULL��ʾδ�ҵ�
	*/
	HWND WINAPI ProcessGetWinHandle(DWORD processID);
};