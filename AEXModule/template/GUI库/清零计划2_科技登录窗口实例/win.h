#pragma once
#include "fnc.h"
#include "resource.h"
extern Gui::WinGui gui;
static class win
{
public:
	static void win_main();//主窗口
	static void AEX_Main_LOGIN(Gui::PWinGui ui, HWND hwnd, BOOL flag);//主窗口回调
	static void FNC_Main(HWND hwnd);//功能窗口
	static void AEX_Main(Gui::PWinGui ui, HWND hwnd, BOOL flag);//功能窗口回调
};

