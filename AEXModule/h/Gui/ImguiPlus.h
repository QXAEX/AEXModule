#pragma once
#include "WinGuiPlus.h"
#include "ImguiPlus/Animation.h"
namespace ImguiPlus {
    int WINAPI run(WinGuiPlus::customMsg msgLoop = nullptr);
	class window {
	public:
        ~window();
        /*
        * 创建窗口
        * @param parent 父窗口句柄
        * @param title 窗口标题
        * @param className 窗口类名
        * @param winInfo 窗口信息, 由createWinInfo函数创建
        * @param callBackFunc 窗口回调函数, 默认值为空
        * @return 是否创建成功
        */
        HWND create(HWND parent, LPCWSTR title, LPCWSTR className, WinGuiPlus::window::INFO winInfo, WinGuiPlus::window::callBack callBackFunc = nullptr);
        /*
        * 获取win对象
        * @return win对象
        */
        WinGuiPlus::window* getWin() const;
    private:
        DWORD code;
    };
};