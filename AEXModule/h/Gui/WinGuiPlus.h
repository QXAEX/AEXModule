#pragma once
#include "../Text.h"
#include <windows.h>
#include <string>
#include "WinGuiPlus/data.h"
#include "./WinGuiPlus/Label.h"
#include "./WinGuiPlus/Edit.h"
#include "./WinGuiPlus/Picture.h"
#include "./WinGuiPlus/Button.h"
#include "./WinGuiPlus/CheckBox.h"
namespace WinGuiPlus {
    // 消息循环
    int WINAPI run();
    // 窗口类
    class window {
    public:
        //窗口位置
        typedef struct INFO {
            ALIGN align = CENTER;//将屏幕判定为九宫格,窗口在九宫格内的位置，默认居中
            ALIGN alignType = CENTER;//在格子内的位置，默认居中
            int top = NULL;//窗口左上角坐标x,设置了align则基于align的九宫格坐标进行计算
            int left = NULL;//窗口左上角坐标y,设置了align则基于align的九宫格坐标进行计算
            int width = 400;//宽度
            int height = 300;//高度
            bool disableResize = true;//禁用窗口大小变化,默认禁用
            bool disableMaximize = true;//禁用窗口最大化
            bool disableMinimize = false;//禁用窗口最小化
            bool topMost = false;//窗口置顶
            BYTE alpha = 255;//窗口透明度
            DWORD iconId = NULL;// 图标ID（可选，默认值0）,可用于C++资源文件中的id来加载图标
            DWORD style = WS_OVERLAPPEDWINDOW;//窗口样式，默认,表示窗口可以拖动,缩放,最大化,最小化
            COLORREF backgroundColor = RGB(255, 255, 255);//背景颜色
            WINGUIPLUS_WINEVENTPROC event;//事件回调中心

        } *PINFO;
        /*
        * 窗口回调函数
        * @param hwnd 窗口句柄
        * @param status 状态码,对应 WINGUIPLUS_STATUS
        * @return void
        */
        typedef std::function<void WINAPI (HWND hwnd, HINSTANCE hInstance, int status)> callBack;
    public:
        window();
        ~window();
        /*
        * 创建窗口
        * @param parent 父窗口句柄
        * @param title 窗口标题
        * @param className 窗口类名
        * @param winInfo 窗口信息, 由createWinInfo函数创建
        * @return 是否创建成功
        */
        HWND create(HWND parent, LPCWSTR title, LPCWSTR className, INFO winInfo, callBack callBackFunc = nullptr);
        /*
        * 获取INFO结构体
        */
        WINGUIPLUS_TEMPLATE getInfo(HWND hwnd);
        /*
        * 设置INFO结构体
        */
        bool setInfo(WINGUIPLUS_TEMPLATE winInfo);
    private:
        HINSTANCE hInstance = NULL;
        std::string token = "";
        DWORD setWindowStyle(INFO& winInfo);
        DWORD setWindowExStyle(INFO& winInfo);
    };
    // 标签
    using namespace WinGuiPlusLabel;
    // 编辑框
    using namespace WinGuiPlusEdit;
    // 图片
    using namespace WinGuiPlusPicture;
    // 按钮
    using namespace WinGuiPlusButton;
    // 复选框
    using namespace WinGuiPlusCheckBox;
}
