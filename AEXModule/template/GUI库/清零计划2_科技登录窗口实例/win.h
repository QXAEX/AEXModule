#pragma once
#include "fnc.h"
#include "resource.h"
extern Gui::WinGui gui;
static class win
{
public:
	static void win_main();//������
	static void AEX_Main_LOGIN(Gui::PWinGui ui, HWND hwnd, BOOL flag);//�����ڻص�
	static void FNC_Main(HWND hwnd);//���ܴ���
	static void AEX_Main(Gui::PWinGui ui, HWND hwnd, BOOL flag);//���ܴ��ڻص�
};

