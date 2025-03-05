#pragma once
#include <Windows.h>
namespace MemoryInject {
	/*
	* 注入类
	* 用于注入dll文件到指定进程的内存中
	*/
	class Inject {
	public:
		/*
		* 注入dll文件到指定进程的内存中
		* @param processPid 进程id
		* @param dllPath dll文件路径
		* @return 注入是否成功
		*/
		bool InjectDll(INT processPid, LPCSTR dllPath);
	private:
		/*
		* 获取反射加载器偏移
		* @param lpReflectiveDllBuffer 反射加载器的内存地址
		* @return 反射加载器偏移
		*/
		DWORD GetReflectiveLoaderOffset(PVOID lpReflectiveDllBuffer);
		/*
		* 加载dll文件
		* @param lpBuffer dll文件内存地址
		* @param dwLength dll文件长度
		* @return 加载dll文件句柄
		*/
		HMODULE WINAPI LoadLibraryR(LPVOID lpBuffer, DWORD dwLength);
		/*
		* 获取dll文件导出函数地址
		* @param hModule dll文件句柄
		* @param lpProcName 导出函数名称
		* @return 导出函数地址
		*/
		FARPROC WINAPI GetProcAddressR(HANDLE hModule, LPCSTR lpProcName);
		/*
		* 远程加载dll文件
		* @param hProcess 进程句柄
		* @param lpBuffer dll文件内存地址
		* @param dwLength dll文件长度
		* @param lpParameter 远程加载参数
		* @return 加载dll文件句柄
		*/
		HANDLE WINAPI LoadRemoteLibraryR(HANDLE hProcess, LPVOID lpBuffer, DWORD dwLength, LPVOID lpParameter);
	};
};