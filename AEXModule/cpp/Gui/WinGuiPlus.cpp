#include "../../h/Gui/WinGuiPlus/_msg.h"
#include "../../h/Gui/WinGuiPlus.h"
#pragma comment(lib, "gdi32.lib")
static ULONG_PTR gdiplusToken = NULL;
static std::vector<WINGUIPLUS_TEMPLATE> tempList;
static WINGUIPLUS_TEMPLATE getTempInfo(HWND hwnd) {
    for (size_t i = 0; i < tempList.size(); i++) {
        if (tempList[i].hwnd == hwnd) {
            return tempList[i];
        }
    }
    return WINGUIPLUS_TEMPLATE();
}
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    WinGuiPlus::window::callBack customCallBackFunc{};
    switch (uMsg) {
    case WM_CREATE: return wm_create(tempList, hwnd, uMsg, wParam, lParam); // 窗口创建
    case WM_DESTROY: return wm_destroy(tempList, hwnd, uMsg, wParam, lParam);// 窗口销毁
    case WM_MOUSEMOVE: return wm_mousemove(tempList, hwnd, uMsg, wParam, lParam);// 鼠标移动
    case WM_PAINT: return wm_paint(tempList, hwnd, uMsg, wParam, lParam); // 绘制窗口
    case WM_LBUTTONDOWN: return wm_lbuttondown(tempList, hwnd, uMsg, wParam, lParam); // 左键按下
    case WM_LBUTTONUP: return wm_lbuttonup(tempList, hwnd, uMsg, wParam, lParam); // 左键释放
    case WM_RBUTTONDOWN: return wm_rbuttondown(tempList, hwnd, uMsg, wParam, lParam); // 右键按下
    case WM_RBUTTONUP: return wm_rbuttonup(tempList, hwnd, uMsg, wParam, lParam); // 右键释放
    case WM_KEYDOWN: return wm_keydown(tempList, hwnd, uMsg, wParam, lParam); // 键盘按键按下
    case WM_KEYUP: return wm_keyup(tempList, hwnd, uMsg, wParam, lParam); // 键盘按键释放
    case WM_SIZE: return wm_size(tempList, hwnd, uMsg, wParam, lParam); // 窗口大小改变
    case WM_MOVE: return wm_move(tempList, hwnd, uMsg, wParam, lParam); // 窗口位置移动
    default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinGuiPlus::run()
{
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return static_cast<int>(msg.wParam);
}

WinGuiPlus::window::window(): hInstance(NULL), token(Text::text_random_alnum(16)) {
    if (gdiplusToken == NULL) {
        Gdiplus::GdiplusStartupInput startupInput;
        Gdiplus::GdiplusStartup(&gdiplusToken, &startupInput, nullptr);
    }
}

WinGuiPlus::window::~window() {
    for (size_t i = 0; i < tempList.size(); i++) {
        if (tempList[i].token == this->token) {
            DestroyWindow(tempList[i].hwnd);
            tempList.erase(tempList.begin() + i);
        }
    }
}

DWORD WinGuiPlus::window::setWindowStyle(INFO& winInfo) {
    DWORD style = winInfo.style;
    if (winInfo.disableResize) style &= ~WS_THICKFRAME;  style &= ~WS_MAXIMIZEBOX;
    if (winInfo.disableMaximize) style &= ~WS_MAXIMIZEBOX;
    if (winInfo.disableMinimize) style &= ~WS_MINIMIZEBOX;
    return style;
}

DWORD WinGuiPlus::window::setWindowExStyle(INFO& winInfo) {
    DWORD exStyle = WS_EX_LAYERED;
    if (winInfo.topMost) exStyle |= WS_EX_TOPMOST;
    return exStyle;
}

HWND WinGuiPlus::window::create(HWND parent, LPCWSTR title, LPCWSTR className, INFO winInfo, callBack callBackFunc) {
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_IME;
    wcex.lpfnWndProc = WindowProc;
    wcex.hInstance = GetModuleHandle(nullptr);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = className;
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(winInfo.backgroundColor);
    if (!RegisterClassEx(&wcex)) return NULL;
    WINGUIPLUS_RECT Rect{
    winInfo.align,
    winInfo.alignType,
    winInfo.left,
    winInfo.top,
    winInfo.width,
    winInfo.height,
    };
    SetWinRectPosition(Rect, winInfo.left, winInfo.top);
    DWORD style = setWindowStyle(winInfo);
    DWORD exStyle = setWindowExStyle(winInfo);
    HWND hwnd = CreateWindowExW(exStyle, className, title, style, winInfo.left, winInfo.top, winInfo.width, winInfo.height, parent, nullptr, wcex.hInstance, &callBackFunc);
    if (!hwnd) return NULL;
    this->hInstance = wcex.hInstance;
    if (!winInfo.iconId == NULL) {
        HICON hIcon = LoadIconW(this->hInstance, MAKEINTRESOURCE(winInfo.iconId));
        if (hIcon) {
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        }
    }

    tempList.push_back({ this->token, hwnd, this->hInstance, callBackFunc, winInfo.event });
    if (winInfo.alpha != 255) {
        DWORD newExStyle = GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_TABSTOP;
        if (SetWindowLong(hwnd, GWL_EXSTYLE, newExStyle) == 0) {
            DWORD error = GetLastError();
            OutputDebugString(L"SetWindowLong failed with error:");
            OutputDebugString(std::to_wstring(error).c_str());
        }
        if (!SetLayeredWindowAttributes(hwnd, 0, winInfo.alpha, LWA_ALPHA)) {
            DWORD error = GetLastError();
            OutputDebugString(L"SetLayeredWindowAttributes failed with error:");
            OutputDebugString(std::to_wstring(error).c_str());
        }
    }
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    return hwnd;
}

WINGUIPLUS_TEMPLATE WinGuiPlus::window::getInfo(HWND hwnd)
{
    return getTempInfo(hwnd);
}

bool WinGuiPlus::window::setInfo(WINGUIPLUS_TEMPLATE winInfo)
{
    for (size_t i = 0; i < tempList.size(); i++) {
        if (tempList[i].hwnd == winInfo.hwnd) {
            tempList[i] = winInfo;
            return true;
        }
    }
    return false;
}