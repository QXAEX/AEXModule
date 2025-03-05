#pragma once
#include "Text.h"
#include <windows.h>
#include <string>
namespace System {
	/*
	* ��ȡϵͳʱ��
	* @param format �Ƿ��ʽ��
	*/
	std::string WINAPI GetTime(bool format = false);
	/*
	* ȡ����Ŀ¼
	* @param pid ����ID,Ĭ��Ϊ��ǰ����ID
	* @return ����Ŀ¼
	*/
	std::string WINAPI GetRunPath(DWORD pid = NULL);
	/*
	* ȡ����ʱ��
	* @return ����ʱ��(����)
	*/
	__int64 WINAPI GetRunTime();
	/*
	* ���ݴ�С����(1:BIT -> 2:KB -> 3:MB -> 4:GB)
	* @param size ��С(λ)
	* @param type ת������,��ַ(1:BIT -> 2:KB -> 3:MB -> 4:GB)
	* @param time ʱ�����,�ɿ�(����ÿ��������1������ݴ�С)
	* @return ת������ڴ��С
	*/
	double WINAPI DataSizeConversion(double size, int& type, __int64 time = -1);
	/*
	* ����Ŀ¼
	* @param path Ŀ¼·��
	* @return �Ƿ�ɹ�
	*/
	bool WINAPI CreateDir(std::string path);
	/*
	* ɾ��Ŀ¼
	* @param path Ŀ¼·��
	* @return �Ƿ�ɹ�
	*/
	bool WINAPI RemoveDir(std::string path);
};