#pragma once
#include "config.h"

namespace ImguiPlusWindow {
    int WINAPI run(WinGuiPlus::customMsg msgLoop = nullptr);
    class window {
    public:
        ~window();
        /*
        * 创建窗口
        * @param drawAll 是否绘制整个内容区
        * @param useFrame 是否使用基础无边框框架
        * @param parent 父窗口句柄
        * @param title 窗口标题
        * @param className 窗口类名
        * @param winInfo 窗口信息, 由createWinInfo函数创建
        * @param callBackFunc 窗口回调函数, 默认值为空
        *       @param WINGUIPLUS_STATUS::CREATE 创建时，回调函数会被调用一次，可用来初始IMGUI窗口样式
        *       @param WINGUIPLUS_STATUS::DRAW 绘制时，回调函数会被调用多次，可用来绘制IMGUI窗口内容
        * @return 是否创建成功
        */
        HWND create(bool drawAll, bool useFrame, HWND parent, LPCWSTR title, LPCWSTR className, WinGuiPlus::window::INFO winInfo, WinGuiPlus::window::callBack callBackFunc = nullptr);
        /*
        * 获取win对象
        * @return win对象
        */
        WinGuiPlus::window* getWin() const;
    private:
        DWORD code;
    };
}