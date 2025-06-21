#include "../h/Hotkeys.h"

// 静态成员初始化
HHOOK HotKeys::s_hook = nullptr;
HotKeys::HOTKEY_CALLBACK HotKeys::s_callback = nullptr;
bool HotKeys::s_ctrl = false;
bool HotKeys::s_alt = false;
bool HotKeys::s_shift = false;

HotKeys::HotKeys() {}

HotKeys::~HotKeys() {
    if (s_hook) {
        UnhookWindowsHookEx(s_hook);
        s_hook = nullptr;
    }
    thread.stop(0);
}

void HotKeys::listen(HOTKEY_CALLBACK callback) {
    s_callback = callback;
    thread.add(0,[&](PThread pthread) {
        // 安装低级键盘钩子
        s_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        });
}

VOID HotKeys::judgeVk(DWORD key, DWORD forKey, JUDGE_CALLBACK callback)
{
    UINT vk = key & 0xFFFF;
    if (vk & forKey) callback();
}

VOID HotKeys::judgeMod(DWORD key, DWORD forKey, JUDGE_CALLBACK callback)
{
    UINT mod = (key >> 16) & 0xFFFF;
    if (mod & forKey) callback();
}

bool HotKeys::IsModifierKey(DWORD vkCode) {
    // 判断是否为控制键（Ctrl/Alt/Shift及其左右变体）
    return vkCode == VK_CONTROL || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL ||
        vkCode == VK_MENU || vkCode == VK_LMENU || vkCode == VK_RMENU ||
        vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT;
}

LRESULT CALLBACK HotKeys::LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) { // 有效事件
        KBDLLHOOKSTRUCT* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        DWORD vkCode = p->vkCode;
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        // 更新修饰键状态
        switch (vkCode) {
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            if (isKeyDown) s_ctrl = true;
            else if (isKeyUp) s_ctrl = false;
            break;
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            if (isKeyDown) s_alt = true;
            else if (isKeyUp) s_alt = false;
            break;
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
            if (isKeyDown) s_shift = true;
            else if (isKeyUp) s_shift = false;
            break;
        }

        // 触发回调：仅处理非修饰键的按下事件
        if (isKeyDown && !IsModifierKey(vkCode)) {
            DWORD mod = 0;
            mod |= s_ctrl ? MOD_CONTROL : 0;
            mod |= s_alt ? MOD_ALT : 0;
            mod |= s_shift ? MOD_SHIFT : 0;

            if (s_callback) {
                // 组合键信息：高16位为修饰键，低16位为虚拟键码
                s_callback((mod << 16) | vkCode);
            }
        }
    }
    return CallNextHookEx(s_hook, code, wParam, lParam);
}
