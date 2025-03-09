#pragma once
/*
* CODE: GB2312
* author: <QX>
* createBy: 2024-08-5
* updateBy: 2025-03-9
*/
#ifndef NDEBUG
	#ifdef _DLL
		#define AEX_LIB_TYPE "MDd"
	#else
		#define AEX_LIB_TYPE "MTd"
	#endif
#else
	#ifdef _DLL
		#define AEX_LIB_TYPE "MD"
	#else
		#define AEX_LIB_TYPE "MT"
	#endif
#endif
#ifdef _WIN64
	#define AEX_LIB_PATH_END "_64.lib"
#else
	#define AEX_LIB_PATH_END ".lib"
#endif
#ifndef AEX_LIB_PATH_TOP
	#define AEX_LIB_PATH_TOP "D:/Project/C/AEXModule/AEXModule/libs/AEXModule_"// 相对路径(根据你下载的AEXModule的路径去改)
#endif
#pragma comment(lib, AEX_LIB_PATH_TOP AEX_LIB_TYPE AEX_LIB_PATH_END)//如果lib加载失败就改相对路径
#include "AEXModule/h/Process.h"								//进程方法集合					<namespace>					updateBy: 2024-08-9
#include "AEXModule/h/Text.h"										//文本方法集合					<namespace>					updateBy: 2025-01-8
#include "AEXModule/h/File.h"										//文件方法对象					<class>							updateBy: 2025-01-21
#include "AEXModule/h/Byteset.h"								//字节集对象						<class>							updateBy: 2024-09-14
#include "AEXModule/h/Memory.h"								//内存方法集合					<namespace>					updateBy: 2025-01-20
#include "AEXModule/h/Network.h"								//网络方法集合					<namespace>					updateBy: 2024-09-15
#include "AEXModule/h/Encryption.h"							//加密解密集合					<namespace>					updateBy: 2024-08-12
#include "AEXModule/h/System.h"									//系统方法集合					<namespace>					updateBy: 2024-09-14
#include "AEXModule/h/AEXStore.h"								//AEX官方数据仓库			<class>							updateBy: 2024-09-14
#include "AEXModule/h/Gui.h"										//Gui库集合						<namespace>					updateBy: 2025-03-9
#include "AEXModule/h/Thread.h"									//线程类							<class>							updateBy: 2024-12-10
#include "AEXModule/h/Sqlite.h"									//sqlite3数据库类				<class>							updateBy: 2025-03-6
#include "AEXModule/h/PortableExecutable.h"				//进程文件PE解析类			<namespace>					updateBy: 2025-01-21
#include "AEXModule/h/Doubles.h"								//超级浮点类型集合			<class>							updateBy: 2025-03-5


