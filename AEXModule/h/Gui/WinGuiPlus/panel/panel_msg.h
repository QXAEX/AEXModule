#pragma once
#include <windows.h>
extern std::vector<WinGuiPlusPanel::Panel*> panelList;
void panelListDelete(HWND hwnd);
WinGuiPlusPanel::Panel* panelListFind(HWND hwnd);
//事件: 窗口创建
int wm_panel_create(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 销毁
int wm_panel_destroy(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 鼠标离开
int wm_panel_mouseleave(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 鼠标移动
int wm_panel_mousemove(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 绘制窗口
int wm_panel_paint(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 左键按下
int wm_panel_lbuttondown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 左键释放
int wm_panel_lbuttonup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 右键按下
int wm_panel_rbuttondown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 右键释放
int wm_panel_rbuttonup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 键盘按键按下
int wm_panel_keydown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 键盘按键释放
int wm_panel_keyup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 窗口大小改变
int wm_panel_size(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 窗口位置移动
int wm_panel_move(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 左键双击
int wm_panel_lbuttondblclk(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 右键双击
int wm_panel_rbuttondblclk(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 鼠标滚轮滚动
int wm_panel_mousewheel(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 光标处理
int wm_panel_setcursor(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 定时器
int wm_panel_timer(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 窗口失去焦点
int wm_panel_killfocus(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);