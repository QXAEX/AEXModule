#include "../../h/Memory/Inject.h"
#include "../../h/api/api.h"
#pragma comment(lib,"Advapi32.lib")
//===============================================================================================//
#ifndef _REFLECTIVEDLLINJECTION_REFLECTIVEDLLINJECTION_H
#define _REFLECTIVEDLLINJECTION_REFLECTIVEDLLINJECTION_H
//===============================================================================================//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// we declare some common stuff in here...

#define DLL_METASPLOIT_ATTACH	4
#define DLL_METASPLOIT_DETACH	5
#define DLL_QUERY_HMODULE		6

#define DEREF( name )*(UINT_PTR *)(name)
#define DEREF_64( name )*(DWORD64 *)(name)
#define DEREF_32( name )*(DWORD *)(name)
#define DEREF_16( name )*(WORD *)(name)
#define DEREF_8( name )*(BYTE *)(name)

typedef ULONG_PTR(WINAPI* REFLECTIVELOADER)(VOID);
typedef BOOL(WINAPI* DLLMAIN)(HINSTANCE, DWORD, LPVOID);

#define DLLEXPORT   __declspec( dllexport ) 

//===============================================================================================//
#endif
//===============================================================================================//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// we declare some common stuff in here...

#define DLL_METASPLOIT_ATTACH	4
#define DLL_METASPLOIT_DETACH	5
#define DLL_QUERY_HMODULE		6

#define DEREF( name )*(UINT_PTR *)(name)
#define DEREF_64( name )*(DWORD64 *)(name)
#define DEREF_32( name )*(DWORD *)(name)
#define DEREF_16( name )*(WORD *)(name)
#define DEREF_8( name )*(BYTE *)(name)

typedef ULONG_PTR(WINAPI* REFLECTIVELOADER)(VOID);
typedef BOOL(WINAPI* DLLMAIN)(HINSTANCE, DWORD, LPVOID);

#define DLLEXPORT   __declspec( dllexport ) 

static DWORD Rva2Offset(DWORD dwRva, UINT_PTR uiBaseAddress)
{
	WORD wIndex = 0;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	PIMAGE_NT_HEADERS pNtHeaders = NULL;

	pNtHeaders = (PIMAGE_NT_HEADERS)(uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew);

	pSectionHeader = (PIMAGE_SECTION_HEADER)((UINT_PTR)(&pNtHeaders->OptionalHeader) + pNtHeaders->FileHeader.SizeOfOptionalHeader);

	if (dwRva < pSectionHeader[0].PointerToRawData)
		return dwRva;

	for (wIndex = 0; wIndex < pNtHeaders->FileHeader.NumberOfSections; wIndex++)
	{
		if (dwRva >= pSectionHeader[wIndex].VirtualAddress && dwRva < (pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].SizeOfRawData))
			return (dwRva - pSectionHeader[wIndex].VirtualAddress + pSectionHeader[wIndex].PointerToRawData);
	}

	return 0;
}
bool MemoryInject::Inject::InjectDll(INT processPid, LPCSTR dllPath)
{
	HANDLE hFile = CreateFileA(dllPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;
	DWORD dwLength = GetFileSize(hFile, NULL);
	if (dwLength == INVALID_FILE_SIZE || dwLength == 0) return false;
	LPVOID lpBuffer = HeapAlloc(GetProcessHeap(), 0, dwLength);
	if (!lpBuffer) return false;
	DWORD dwBytesRead = NULL;
	if (ReadFile(hFile, lpBuffer, dwLength, &dwBytesRead, NULL) == FALSE) return false;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES priv = { 0 };
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
		AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);
		CloseHandle(hToken);
	}
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processPid);
	if (!hProcess) return false;
	HANDLE hModule = this->LoadRemoteLibraryR(hProcess, lpBuffer, dwLength, NULL);
	if (!hModule) return false;
	WaitForSingleObject(hModule, -1);
	if (lpBuffer) HeapFree(GetProcessHeap(), 0, lpBuffer);
	if (hProcess) CloseHandle(hProcess);
	return true;
}
DWORD MemoryInject::Inject::GetReflectiveLoaderOffset(PVOID lpReflectiveDllBuffer)
{
	UINT_PTR uiBaseAddress = 0;
	UINT_PTR uiExportDir = 0;
	UINT_PTR uiNameArray = 0;
	UINT_PTR uiAddressArray = 0;
	UINT_PTR uiNameOrdinals = 0;
	DWORD dwCounter = 0;
#ifdef _WIN64
	DWORD dwCompiledArch = 2;
#else
	//这里捕获Win32和WinRT
	DWORD dwCompiledArch = 1;
#endif
	uiBaseAddress = (UINT_PTR)lpReflectiveDllBuffer;
	//获取模块NT Header的文件偏移量
	uiExportDir = uiBaseAddress + ((PIMAGE_DOS_HEADER)uiBaseAddress)->e_lfanew;
	//目前，我们只能处理与此函数类型相同的PE文件
	//由于在编译时定义了PE结构中的各种偏移
	if (((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.Magic == 0x010B) // PE32
	{
		if (dwCompiledArch != 1)
			return 0;
	}
	else if (((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.Magic == 0x020B) // PE64
	{
		if (dwCompiledArch != 2)
			return 0;
	}
	else
	{
		return 0;
	}
	//uiNameArray=模块导出目录条目的地址
	uiNameArray = (UINT_PTR) & ((PIMAGE_NT_HEADERS)uiExportDir)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	//获取导出目录的文件偏移量
	uiExportDir = uiBaseAddress + Rva2Offset(((PIMAGE_DATA_DIRECTORY)uiNameArray)->VirtualAddress, uiBaseAddress);
	//获取名称指针数组的文件偏移量
	uiNameArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNames, uiBaseAddress);
	//获取地址数组的文件偏移量
	uiAddressArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
	//获取名称序号数组的文件偏移量
	uiNameOrdinals = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfNameOrdinals, uiBaseAddress);
	//获取导出函数数量的计数器
	dwCounter = ((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->NumberOfNames;
	//遍历所有导出的函数以找到ReflectiveLoader
	while (dwCounter--)
	{
		char* cpExportedFunctionName = (char*)(uiBaseAddress + Rva2Offset(DEREF_32(uiNameArray), uiBaseAddress));
		if (strstr(cpExportedFunctionName, "ReflectiveLoader") != NULL)
		{
			//获取地址数组的文件偏移量
			uiAddressArray = uiBaseAddress + Rva2Offset(((PIMAGE_EXPORT_DIRECTORY)uiExportDir)->AddressOfFunctions, uiBaseAddress);
			//使用函数name ordinal作为名称指针数组的索引
			uiAddressArray += (DEREF_16(uiNameOrdinals) * sizeof(DWORD));
			//将文件偏移量返回给ReflectiveLoader（）函数代码
			return Rva2Offset(DEREF_32(uiAddressArray), uiBaseAddress);
		}
		//获取下一个导出的函数名
		uiNameArray += sizeof(DWORD);
		//获取下一个导出的函数名序号
		uiNameOrdinals += sizeof(WORD);
	}
	return 0;
}

HMODULE __stdcall MemoryInject::Inject::LoadLibraryR(LPVOID lpBuffer, DWORD dwLength)
{
	HMODULE hResult = NULL;
	DWORD dwReflectiveLoaderOffset = 0;
	DWORD dwOldProtect1 = 0;
	DWORD dwOldProtect2 = 0;
	REFLECTIVELOADER pReflectiveLoader = NULL;
	DLLMAIN pDllMain = NULL;
	if (lpBuffer == NULL || dwLength == 0)
		return NULL;
	__try
	{
		//检查库是否有ReflectiveLoader
		dwReflectiveLoaderOffset = GetReflectiveLoaderOffset(lpBuffer);
		if (dwReflectiveLoaderOffset != 0)
		{
			pReflectiveLoader = (REFLECTIVELOADER)((UINT_PTR)lpBuffer + dwReflectiveLoaderOffset);
			//我们必须对RWX的缓冲区进行虚拟保护，以便执行ReflectiveLoader
			//这假设lpBuffer是页面区域的基址，dwLength是区域的大小
			if (VirtualProtect(lpBuffer, dwLength, PAGE_EXECUTE_READWRITE, &dwOldProtect1))
			{
				//调用库的ReflectiveLoader
				pDllMain = (DLLMAIN)pReflectiveLoader();
				if (pDllMain != NULL)
				{
					//调用加载的库DllMain以获取其HMODULE
					if (!pDllMain(NULL, DLL_QUERY_HMODULE, &hResult))
						hResult = NULL;
				}
				//恢复到以前的保护标志
				VirtualProtect(lpBuffer, dwLength, dwOldProtect1, &dwOldProtect2);
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		hResult = NULL;
	}
	return hResult;
}

FARPROC __stdcall MemoryInject::Inject::GetProcAddressR(HANDLE hModule, LPCSTR lpProcName)
{
	UINT_PTR uiLibraryAddress = 0;
	FARPROC fpResult = NULL;
	if (hModule == NULL)
		return NULL;
	//模块句柄实际上是它的基址
	uiLibraryAddress = (UINT_PTR)hModule;
	__try
	{
		UINT_PTR uiAddressArray = 0;
		UINT_PTR uiNameArray = 0;
		UINT_PTR uiNameOrdinals = 0;
		PIMAGE_NT_HEADERS pNtHeaders = NULL;
		PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
		PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
		//获取模块NT Header的VA
		pNtHeaders = (PIMAGE_NT_HEADERS)(uiLibraryAddress + ((PIMAGE_DOS_HEADER)uiLibraryAddress)->e_lfanew);
		pDataDirectory = (PIMAGE_DATA_DIRECTORY)&pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		//获取导出目录的VA
		pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(uiLibraryAddress + pDataDirectory->VirtualAddress);
		//获取地址数组的VA
		uiAddressArray = (uiLibraryAddress + pExportDirectory->AddressOfFunctions);
		//获取名称指针数组的VA
		uiNameArray = (uiLibraryAddress + pExportDirectory->AddressOfNames);
		//获取名称序号数组的VA
		uiNameOrdinals = (uiLibraryAddress + pExportDirectory->AddressOfNameOrdinals);
		//测试我们是按名称还是按序号导入
		if (((DWORD)lpProcName & 0xFFFF0000) == 0x00000000)
		{
			//按顺序导入
			//使用导入序号（-export序号基）作为地址数组的索引
			uiAddressArray += ((IMAGE_ORDINAL((DWORD)lpProcName) - pExportDirectory->Base) * sizeof(DWORD));
			//解析此导入函数的地址
			fpResult = (FARPROC)(uiLibraryAddress + DEREF_32(uiAddressArray));
		}
		else
		{
			//按名称导入
			DWORD dwCounter = pExportDirectory->NumberOfNames;
			while (dwCounter--)
			{
				char* cpExportedFunctionName = (char*)(uiLibraryAddress + DEREF_32(uiNameArray));
				if (strcmp(cpExportedFunctionName, lpProcName) == 0)
				{
					//使用函数name ordinal作为名称指针数组的索引
					uiAddressArray += (DEREF_16(uiNameOrdinals) * sizeof(DWORD));
					//计算函数的虚拟地址
					fpResult = (FARPROC)(uiLibraryAddress + DEREF_32(uiAddressArray));
					//完成
					break;
				}
				//获取下一个导出的函数名
				uiNameArray += sizeof(DWORD);
				//获取下一个导出的函数名序号
				uiNameOrdinals += sizeof(WORD);
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		fpResult = NULL;
	}
	return fpResult;
}

HANDLE __stdcall MemoryInject::Inject::LoadRemoteLibraryR(HANDLE hProcess, LPVOID lpBuffer, DWORD dwLength, LPVOID lpParameter)
{
	BOOL bSuccess = FALSE;
	LPVOID lpRemoteLibraryBuffer = NULL;
	LPTHREAD_START_ROUTINE lpReflectiveLoader = NULL;
	HANDLE hThread = NULL;
	DWORD dwReflectiveLoaderOffset = 0;
	DWORD dwThreadId = 0;
	__try
	{
		do
		{
			if (!hProcess || !lpBuffer || !dwLength)
				break;
			//检查库是否有ReflectiveLoader
			dwReflectiveLoaderOffset = GetReflectiveLoaderOffset(lpBuffer);
			if (!dwReflectiveLoaderOffset)
				break;
			//在主机进程中为图像分配内存（RWX）。。。
			lpRemoteLibraryBuffer = VirtualAllocEx(hProcess, NULL, dwLength, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (!lpRemoteLibraryBuffer)
				break;
			//将映像写入主机进程。。。
			if (!WriteProcessMemory(hProcess, lpRemoteLibraryBuffer, lpBuffer, dwLength, NULL))
				break;
			//将ReflectiveLoader()的偏移量添加到远程库地址。。。
			lpReflectiveLoader = (LPTHREAD_START_ROUTINE)((ULONG_PTR)lpRemoteLibraryBuffer + dwReflectiveLoaderOffset);
			//在主机进程中创建一个远程线程来调用ReflectiveLoader！
			hThread = CreateRemoteThread(hProcess, NULL, 1024 * 1024, lpReflectiveLoader, lpParameter, (DWORD)NULL, &dwThreadId);

		} while (0);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		hThread = NULL;
	}

	return hThread;
}
