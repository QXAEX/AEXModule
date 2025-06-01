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
                WINGUIPLUS_TEMPLATE temp = *it;
                tempList.erase(it);
                bool flag = false;
                for (auto it2 = tempList.begin(); it2 != tempList.end(); ++it2) {
                    if (it2->parent == temp.parent) {
                        flag = true;
                        break;
                    }
                }
                if(!flag) EnableWindow(temp.parent, TRUE);
                return true;
            }
        }
        return false;
}
int wm_create(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
    WinGuiPlus::window::callBack* callBack = static_cast<WinGuiPlus::window::callBack*>(pCreateStruct->lpCreateParams);
    if (callBack) (*callBack)(hwnd, GetModuleHandle(NULL), WINGUIPLUS_STATUS::CREATE, NULL);
    return 0;
}

int wm_destroy(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.callBack) temp.callBack(temp.hwnd, temp.hInstance, WINGUIPLUS_STATUS::DESTROY, NULL);
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
    if (temp.event.mouse)temp.event.mouse((WINGUIPLUS_STATUS)state,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
}

int wm_nchittest(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
    if (((WinGuiPlus::window*)(temp.win))->nchittest && hit == HTCLIENT) // 当启用拖动且命中客户区时
    {
        // 转换坐标并检查子控件
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ::ScreenToClient(hwnd, &pt);
        HWND hChild = ::ChildWindowFromPoint(hwnd, pt);

        // 空白区域才可拖动（无子控件时）
        if (hChild == NULL || hChild == hwnd) {
            return HTCAPTION; // 伪装成标题栏点击
        }
    }
    return hit;
}

int wm_paint(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    PAINTSTRUCT ps;
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    HDC hdc = BeginPaint(hwnd, &ps);
    if (temp.callBack) temp.callBack(temp.hwnd, temp.hInstance, WINGUIPLUS_STATUS::DRAW, hdc);
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
    if (temp.callBack) temp.callBack(temp.hwnd, temp.hInstance, WINGUIPLUS_STATUS::SIZE_CHANGED, NULL);
    return 0;
}

int wm_move(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
    WINGUIPLUS_TEMPLATE temp = findTempInfo(tempList, hwnd);
    if (temp.event.winMove)temp.event.winMove(LOWORD(lParam), HIWORD(lParam));
    return 0;
}
