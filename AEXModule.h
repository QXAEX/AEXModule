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
	#define AEX_LIB_PATH_TOP "D:/Project/C/AEXModule/AEXModule/libs/AEXModule_"// ���·��(���������ص�AEXModule��·��ȥ��)
#endif
#pragma comment(lib, AEX_LIB_PATH_TOP AEX_LIB_TYPE AEX_LIB_PATH_END)//���lib����ʧ�ܾ͸����·��
#include "AEXModule/h/Process.h"								//���̷�������					<namespace>					updateBy: 2024-08-9
#include "AEXModule/h/Text.h"										//�ı���������					<namespace>					updateBy: 2025-01-8
#include "AEXModule/h/File.h"										//�ļ���������					<class>							updateBy: 2025-01-21
#include "AEXModule/h/Byteset.h"								//�ֽڼ�����						<class>							updateBy: 2024-09-14
#include "AEXModule/h/Memory.h"								//�ڴ淽������					<namespace>					updateBy: 2025-01-20
#include "AEXModule/h/Network.h"								//���緽������					<namespace>					updateBy: 2024-09-15
#include "AEXModule/h/Encryption.h"							//���ܽ��ܼ���					<namespace>					updateBy: 2024-08-12
#include "AEXModule/h/System.h"									//ϵͳ��������					<namespace>					updateBy: 2024-09-14
#include "AEXModule/h/AEXStore.h"								//AEX�ٷ����ݲֿ�			<class>							updateBy: 2024-09-14
#include "AEXModule/h/Gui.h"										//Gui�⼯��						<namespace>					updateBy: 2025-03-9
#include "AEXModule/h/Thread.h"									//�߳���							<class>							updateBy: 2024-12-10
#include "AEXModule/h/Sqlite.h"									//sqlite3���ݿ���				<class>							updateBy: 2025-03-6
#include "AEXModule/h/PortableExecutable.h"				//�����ļ�PE������			<namespace>					updateBy: 2025-01-21
#include "AEXModule/h/Doubles.h"								//�����������ͼ���			<class>							updateBy: 2025-03-5


