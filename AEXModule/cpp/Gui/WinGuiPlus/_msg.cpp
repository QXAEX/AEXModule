#include "../../../h/Gui/WinGuiPlus/_msg.h"
#include <windows.h>
#include <windowsx.h>
#include "../../../h/Gui/WinGuiPlus.h"

static WINGUIPLUS_TEMPLATE findTempInfo(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND hwnd) {
    for (auto it = tempList.begin(); it != tempList.end(); ++it) {
        if (it->hwnd == hwnd) {
            return  *it;
        }
    }
    return WINGUIPLUS_TEMPLATE();
}
static bool destroyTempInfo(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND hwnd) {
        for (auto it = tempList.begin(); it != tempList.end(); ++it) {
            if (it->hwnd == hwnd) {
                tempList.erase(it);
                return true;
            }
        }
        return false;
}
int wm_create(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
    WinGuiPlus::window::callBack* callBack = static_cast<WinGuiPlus::window::callBack*>(pCreateStruct->lpCreateParams);
    if (callBack) (*callBack)(hwnd, GetModuleHandle(NULL), WINGUIPLUS_STATUS::CREATE);
    return 0;
}

int wm_destroy(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.callBack) {
        temp.callBack(temp.hwnd,temp.hInstance, WINGUIPLUS_STATUS::DESTROY);
    }
    destroyTempInfo(tempList, hwnd);
    if (tempList.empty()) {
        PostQuitMessage(0);
    }
    return 0;
}

int wm_mousemove(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    int state = WINGUIPLUS_STATUS::MOUSE_MOVE;
    if (wParam == MK_LBUTTON) state = WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN_MOVE;
    else if(wParam == MK_RBUTTON) state = WINGUIPLUS_STATUS::MOUSE_RIGHT_DOWN_MOVE;
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.mouse)temp.event.mouse(state,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
}

int wm_paint(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
    return 0;
}

int wm_lbuttondown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.mouseLeft)temp.event.mouseLeft(WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
}

int wm_lbuttonup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.mouseLeft)temp.event.mouseLeft(WINGUIPLUS_STATUS::MOUSE_LEFT_UP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    SetFocus(hwnd); // 强制窗口获取焦点
    return 0;
}

int wm_rbuttondown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.mouseRight)temp.event.mouseRight(WINGUIPLUS_STATUS::MOUSE_RIGHT_DOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
}

int wm_rbuttonup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.mouseRight)temp.event.mouseRight(WINGUIPLUS_STATUS::MOUSE_RIGHT_UP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
}

int wm_keydown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.key)temp.event.key(WINGUIPLUS_STATUS::KEY_DOWN, wParam, lParam);
    return 0;
}

int wm_keyup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.key)temp.event.key(WINGUIPLUS_STATUS::KEY_UP, wParam, lParam);
    return 0;
}

int wm_size(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.winSize)temp.event.winSize(LOWORD(lParam), HIWORD(lParam));
    return 0;
}

int wm_move(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.winMove)temp.event.winMove(LOWORD(lParam), HIWORD(lParam));
    return 0;
}