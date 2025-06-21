#pragma once
#include <Windows.h>
#include <functional>
#include "Thread.h"
class HotKeys {
public:
    typedef std::function<VOID(DWORD key)> HOTKEY_CALLBACK;
    typedef std::function<VOID()> JUDGE_CALLBACK;
    HotKeys();
    ~HotKeys();
    VOID listen(HOTKEY_CALLBACK callback);
    /*
    * 判断虚拟键码
    *@params key 原操作码
    *@params key 指定操作码
    * @params callback 符合指定操作码后调用此方法
    */
    VOID judgeVk(DWORD key, DWORD forKey, JUDGE_CALLBACK callback);
    /*
    * 判断修饰键
    *@params key 原操作码
    *@params key 指定操作码
    * @params callback 符合指定操作码后调用此方法
    */
    VOID judgeMod(DWORD key, DWORD forKey, JUDGE_CALLBACK callback);
private:
    Thread thread;
    static HOTKEY_CALLBACK s_callback;    // 静态回调函数指针
    static HHOOK s_hook;                 // 钩子句柄
    static bool s_ctrl;                   // Ctrl键状态
    static bool s_alt;                    // Alt键状态
    static bool s_shift;                  // Shift键状态
    // 键盘钩子处理函数
    static LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam);
    // 判断是否为修饰键
    static bool IsModifierKey(DWORD vkCode);
};
