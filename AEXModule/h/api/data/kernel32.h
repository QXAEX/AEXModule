#pragma once
#include <windows.h>
#include <TlHelp32.h>
namespace KERNEL32 {
	extern HMODULE Kernel32;
	/*
	* 创建进程快照
	* @param dwFlags 快照类型
	* @param th32ProcessID 进程ID
	* @return 快照句柄
	*/
	HANDLE WINAPI CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
	/*
	* 线程第一条信息
	* @param hSnapshot 快照句柄
	* @param lpte 线程信息结构体
	* @return 是否成功
	*/
	BOOL WINAPI Thread32First(HANDLE hSnapshot, PTHREADENTRY32 lpte);
	/*
	* 线程下一条信息
	* @param hSnapshot 快照句柄
	* @param lpte 线程信息结构体
	* @return 是否成功
	*/
	BOOL WINAPI Thread32Next(HANDLE hSnapshot, PTHREADENTRY32 lpte);
};