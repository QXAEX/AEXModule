#pragma once
#include "Text.h"
#include <windows.h>
#include <string>
namespace System {
	/*
	* 获取系统时间
	* @param format 是否格式化
	* @param hideSecond 是否不显示时分秒
	*/
	std::string WINAPI GetTime(bool format = false, bool hideSecond = false);
	/*
	* 取运行目录
	* @param pid 进程ID,默认为当前进程ID
	* @return 运行目录
	*/
	std::string WINAPI GetRunPath(DWORD pid = NULL);
	/*
	* 取运行时间
	* @return 运行时间(毫秒)
	*/
	__int64 WINAPI GetRunTime();
	/*
	* 数据大小换算(1:BIT -> 2:KB -> 3:MB -> 4:GB)
	* @param size 大小(位)
	* @param type 转换类型,传址(1:BIT -> 2:KB -> 3:MB -> 4:GB)
	* @param time 时间计量,可空(根据每秒来计算1秒的数据大小)
	* @return 转换后的内存大小
	*/
	double WINAPI DataSizeConversion(double size, int& type, __int64 time = -1);
	/*
	* 创建目录
	* @param path 目录路径
	* @return 是否成功，只有当目录不存在时才创建，如果没有创建操作或着权限不足时返回false
	*/
	bool WINAPI CreateDir(std::string path);
	/*
	* 删除目录
	* @param path 目录路径
	* @return 是否成功
	*/
	bool WINAPI RemoveDir(std::string path);
};