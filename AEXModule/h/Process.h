#pragma once
#include <windows.h>
#include "Text.h"
#include <TlHelp32.h>
namespace Process {
	//枚举进程信息列表
	struct ProcessInfo {
		DWORD PID = NULL;//进程ID
		std::string Name = "";//进程名
		std::wstring WName = L"";//进程名(宽字符)
		std::string WinTitle = "";//进程窗口标题
		HWND WinHandle = NULL;//进程窗口句柄
		std::string Path = "";//进程路径
		std::string CommandLine ="";//进程命令行
		std::vector<MODULEENTRY32> Modules = {};//模块列表(如果获取为空，请检查管理员权限)
	};
	/*
	* 获取进程路径
	* @param processID 进程ID
	* @return 进程路径
	*/
	std::string WINAPI GetProcessPath(DWORD processID);
	/*
	* 获取进程命令行
	* @param processID 进程ID
	* @return 进程命令行
	*/
	std::string WINAPI GetProcessCommandLine(DWORD processID);
	/*
	* 获取窗口标题
	* @param hwnd 窗口句柄
	* @return 窗口标题
	*/
	std::string WINAPI GetWindowTitle(HWND hwnd);
	/*
	* 进程ID取窗口句柄
	* @param processID 进程ID
	* @return 窗口句柄,NULL表示未找到
	*/
	HWND WINAPI GetProcessWinHandle(DWORD processID);
	/*
	* 获取所有进程信息
	* @return 所有进程的信息
	*/
	std::vector<ProcessInfo> WINAPI ProcessList();
	/*
	* 进程名取进程ID
	* @param processName 进程名
	* @return 进程ID,-1表示未找到
	*/
	DWORD WINAPI ProcessGetID(std::string processName);
	/*
	* 进程ID取进程名
	* @param processID 进程ID
	* @return 进程名,空字符串表示未找到
	*/
	std::string WINAPI ProcessGetName(DWORD processID);
	/*
	* 进程ID取窗口句柄
	* @param processID 进程ID
	* @return 窗口句柄,NULL表示未找到
	*/
	HWND WINAPI ProcessGetWinHandle(DWORD processID);
};