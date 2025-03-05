#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include "data.h"
//事件: 窗口创建
int wm_create(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 销毁
int wm_destroy(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 鼠标移动
int wm_mousemove(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 绘制窗口
int wm_paint(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 左键按下
int wm_lbuttondown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 左键释放
int wm_lbuttonup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 右键按下
int wm_rbuttondown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 右键释放
int wm_rbuttonup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 键盘按键按下
int wm_keydown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 键盘按键释放
int wm_keyup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 窗口大小改变
int wm_size(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//事件: 窗口位置移动
int wm_move(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);