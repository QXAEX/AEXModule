#include "../../../../h/Gui/WinGuiPlus/panel/panel.h"
#include "../../../../h/Gui/WinGuiPlus/panel/panel_msg.h"
#include <unordered_map>
#include <windowsx.h>
#include "../../../../h/Gui/WinGuiPlus/CheckBox.h"
std::vector<WinGuiPlusPanel::Panel*> panelList;
static void wm_panel_iniiMsg(HWND hwnd) {
	TRACKMOUSEEVENT tme{};
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE | TME_HOVER; // 请求离开和悬停事件
	tme.hwndTrack = hwnd;
	tme.dwHoverTime = HOVER_DEFAULT; // 默认悬停时间（400毫秒）
	TrackMouseEvent(&tme);
	//设置光标为默认
	SetCursor(LoadCursor(NULL, IDC_ARROW));
};
void panelListDelete(HWND hwnd) {
	for (size_t i = 0; i < panelList.size(); i++) {
		if (panelList[i]->panelData.hwnd == hwnd) {
			panelList.erase(panelList.begin() + i);
			break;
		}
	}
}
WinGuiPlusPanel::Panel* panelListFind(HWND hwnd) {
	for (size_t i = 0; i < panelList.size(); i++) {
		if (panelList[i]->panelData.hwnd == hwnd) {
			return panelList[i];
		}
	}
	return nullptr;
}
int wm_panel_create(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	wm_panel_iniiMsg(hwnd);
	LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
	WinGuiPlusPanel::Panel* panel = reinterpret_cast<WinGuiPlusPanel::Panel*>(pCreateStruct->lpCreateParams);
	if (panel) {
		if (panel->Event()->callBack) panel->Event()->callBack(hwnd, panel->panelData.hInstance, WINGUIPLUS_STATUS::CREATE);
	}
	return TRUE;
}
int wm_panel_destroy(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel) {
		if (panel->Event()->callBack) panel->Event()->callBack(hwnd, panel->panelData.hInstance, WINGUIPLUS_STATUS::DESTROY);
		panelListDelete(hwnd);
	}
	return 0;
}
int wm_panel_mouseleave(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel) {
		if (panel->Event()->event.mouse)panel->Event()->event.mouse(WINGUIPLUS_STATUS::MOUSE_LEAVE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		panel->panelData.Cursor = false;
	};
	return TRUE;
}
int wm_panel_mousemove(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel) {
		int state = WINGUIPLUS_STATUS::MOUSE_MOVE;
		if (!panel->panelData.Cursor) state = WINGUIPLUS_STATUS::MOUSE_ENTER, panel->panelData.Cursor = true, wm_panel_iniiMsg(hwnd);
		else if (wParam == MK_LBUTTON) state = WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN_MOVE;
		else if (wParam == MK_RBUTTON) state = WINGUIPLUS_STATUS::MOUSE_RIGHT_DOWN_MOVE;
		if (panel->panelData.isDragging && panel->panelData.dragEnabled) {
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(hwnd, &pt);
			SetWindowPos(hwnd, NULL, pt.x - panel->panelData.dragOffset.x, pt.y - panel->panelData.dragOffset.y, panel->panelData.width, panel->panelData.height, SWP_NOZORDER);
		}
		else if (panel->Event()->event.mouse) if (panel->Event()->event.mouse)panel->Event()->event.mouse(state, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	
	return 0;
}
int wm_panel_paint(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	if (panel && !panel->Event()->token.empty() && panel->callback) {
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBitmap = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
		FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
		Gdiplus::Graphics graphics(memDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		((WinGuiPlusPanel::Panel::DrawFunc)panel->callback)(&panel->panelData, &graphics);
		BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, memDC, 0, 0, SRCCOPY);
		SelectObject(memDC, oldBitmap);
		DeleteObject(memBitmap);
		DeleteDC(memDC);

		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	EndPaint(hwnd, &ps);
	return 0;
}

int wm_panel_lbuttondown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->panelData.dragEnabled && panel->panelData.isLayered) {
		panel->panelData.dragOffset.x = GET_X_LPARAM(lParam) - panel->panelData.left;
		panel->panelData.dragOffset.y = GET_Y_LPARAM(lParam) - panel->panelData.top;
		panel->panelData.isDragging = true;
		SetCapture(hwnd);
	}
	if (panel && panel->Event()->event.mouseLeft) {
		panel->Event()->event.mouseLeft(WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	return 0;
}
int wm_panel_lbuttonup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel) {
		if (panel->panelData.isDragging && panel->panelData.dragEnabled) {
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			panel->panelData.top = pt.y - panel->panelData.dragOffset.y;
			panel->panelData.left = pt.x - panel->panelData.dragOffset.x;
			panel->panelData.isDragging = false;
			ReleaseCapture();
		}
		switch (panel->panelData.componentType)
		{
		case COMPONENT_TYPE::CHECKBOX://复选框
			WinGuiPlusCheckBox::CheckBox* checkBox = (WinGuiPlusCheckBox::CheckBox*)panel->panelData.params1;
			if (checkBox->checked)*checkBox->checked = !(*checkBox->checked);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		if (panel->Event()->event.mouseLeft) panel->Event()->event.mouseLeft(WINGUIPLUS_STATUS::MOUSE_LEFT_UP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	SetFocus(hwnd); // 强制获取焦点
	return 0;
}
int wm_panel_rbuttondown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.mouseRight) panel->Event()->event.mouseRight(WINGUIPLUS_STATUS::MOUSE_RIGHT_DOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
}
int wm_panel_rbuttonup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.mouseRight) panel->Event()->event.mouseRight(WINGUIPLUS_STATUS::MOUSE_RIGHT_UP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
}
int wm_panel_keydown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.key) panel->Event()->event.key(WINGUIPLUS_STATUS::KEY_DOWN, wParam, lParam);
	return 0;
}
int wm_panel_keyup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.key) panel->Event()->event.key(WINGUIPLUS_STATUS::KEY_UP, wParam, lParam);
	return 0;
}
int wm_panel_size(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.winSize) panel->Event()->event.winSize(LOWORD(lParam), HIWORD(lParam));
	return 0;
}
int wm_panel_move(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.winMove) panel->Event()->event.winMove(LOWORD(lParam), HIWORD(lParam));
	return 0;
}
int wm_panel_lbuttondblclk(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.mouseLeft) panel->Event()->event.mouseLeft(WINGUIPLUS_STATUS::MOUSE_LEFT_DOUBLE_CLICK, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
}
int wm_panel_rbuttondblclk(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel && panel->Event()->event.mouseRight) panel->Event()->event.mouseRight(WINGUIPLUS_STATUS::MOUSE_RIGHT_DOUBLE_CLICK, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
}
int wm_panel_mousewheel(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	int direction = (delta > 0) ? 1 : -1;
	if (panel && panel->Event()->event.mouseWheel) panel->Event()->event.mouseWheel(direction, delta, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
}
int wm_panel_setcursor(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel) {
		if (panel->Event()->event.setCursor && (HWND)wParam == hwnd) panel->Event()->event.setCursor();
	}
	return TRUE;
}

int wm_panel_timer(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	return 0;
}

int wm_panel_killfocus(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam)
{
	WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
	if (panel) {
		if (panel->Event()->event.killFocus) panel->Event()->event.killFocus();
	}
	return 0;
}
