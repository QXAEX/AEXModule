#include "../../../../h/Gui/WinGuiPlus/panel/panel.h"
#include "../../../../h/Gui/WinGuiPlus/panel/panel_msg.h"
#include <dwmapi.h>
#include <windowsx.h>
#pragma comment(lib, "dwmapi.lib")
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: return wm_panel_create(hwnd, uMsg, wParam, lParam); // ���ڴ���
	case WM_DESTROY: return wm_panel_destroy(hwnd, uMsg, wParam, lParam);// ��������
	case WM_MOUSELEAVE: return wm_panel_mouseleave(hwnd, uMsg, wParam, lParam);// ����뿪
	case WM_MOUSEMOVE: return wm_panel_mousemove(hwnd, uMsg, wParam, lParam);// ����ƶ�
	case WM_PAINT: return wm_panel_paint(hwnd, uMsg, wParam, lParam); // ���ƴ���
	case WM_LBUTTONDOWN: return wm_panel_lbuttondown(hwnd, uMsg, wParam, lParam); // �������
	case WM_LBUTTONUP: return wm_panel_lbuttonup(hwnd, uMsg, wParam, lParam); // ����ͷ�
	case WM_RBUTTONDOWN: return wm_panel_rbuttondown(hwnd, uMsg, wParam, lParam); // �Ҽ�����
	case WM_RBUTTONUP: return wm_panel_rbuttonup(hwnd, uMsg, wParam, lParam); // �Ҽ��ͷ�
	case WM_KEYDOWN: return wm_panel_keydown(hwnd, uMsg, wParam, lParam); // ���̰�������
	case WM_KEYUP: return wm_panel_keyup(hwnd, uMsg, wParam, lParam); // ���̰����ͷ�
	case WM_SIZE: return wm_panel_size(hwnd, uMsg, wParam, lParam); // ���ڴ�С�ı�
	case WM_MOVE: return wm_panel_move(hwnd, uMsg, wParam, lParam); // ����λ���ƶ�
	case WM_LBUTTONDBLCLK: return wm_panel_lbuttondblclk(hwnd, uMsg, wParam, lParam);//���˫��
	case WM_RBUTTONDBLCLK: return wm_panel_rbuttondblclk(hwnd, uMsg, wParam, lParam);//�Ҽ�˫��
	case WM_MOUSEWHEEL: return wm_panel_mousewheel(hwnd, uMsg, wParam, lParam); // �����ֹ���
	case WM_SETCURSOR: return wm_panel_setcursor(hwnd, uMsg, wParam, lParam); // ���������
	case WM_TIMER: return wm_panel_timer(hwnd, uMsg, wParam, lParam); // ��ʱ��
	case WM_KILLFOCUS: return wm_panel_killfocus(hwnd, uMsg, wParam, lParam); //����ʧȥ����
	case WM_NCCREATE: {
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		if (pCreate) {
			WinGuiPlusPanel::Panel* panel = reinterpret_cast<WinGuiPlusPanel::Panel*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(panel));
		}
		return TRUE;
	}
	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

WinGuiPlusPanel::Panel::Panel(HWND parent, LPCSTR className, HINSTANCE hInstance, int top, int left, int width, int height, bool isLayered, bool visible)
{
	this->create(parent, className, hInstance, top, left, width, height, isLayered, visible);
}

void WinGuiPlusPanel::Panel::create(HWND parent, LPCSTR className, HINSTANCE hInstance, int top, int left, int width, int height, bool isLayered, bool visible)
{
	this->event.token = Text::text_random_alnum(6);
	this->panelData.hInstance = hInstance;
	WNDCLASSEXA wc = { 0 };
	wc.hInstance = this->panelData.hInstance;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC | CS_SAVEBITS | CS_IME | CS_DROPSHADOW;
	wc.lpfnWndProc = WindowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = className;
	RegisterClassExA(&wc);
	this->panelData.parent = parent;
	this->panelData.isLayered = isLayered;
	this->panelData.top = top;
	this->panelData.left = left;
	this->panelData.width = width;
	this->panelData.height = height;
	this->panelData.visible = visible;
	//WS_OVERLAPPEDWINDOW      ���ô���
	this->panelData.hwnd = CreateWindowExA(NULL, className, "", (isLayered ? WS_POPUP : WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_BITMAP), left, top, width, height, isLayered ? NULL : parent, NULL, this->panelData.hInstance, this);
	if (!this->panelData.hwnd) {
		throw std::exception("����ʼ��ʧ��!");
		return;
	}
	SetWindowLong(this->panelData.hwnd, GWL_EXSTYLE, GetWindowLong(this->panelData.hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_TABSTOP);
	panelList.push_back(this);
	this->SetBasicInfo(top, left, width, height);
	this->SetVisible(visible);
}

void WinGuiPlusPanel::Panel::SetVisible(bool visible)
{
	ShowWindow(this->panelData.hwnd, visible ? SW_SHOW : SW_HIDE);
	this->panelData.visible = visible;
}

void WinGuiPlusPanel::Panel::SetBasicInfo(int top, int left, int width, int height)
{
	SetWindowPos(this->panelData.hwnd, NULL, left, top, width, height, SWP_NOACTIVATE);
	this->panelData.top = top;
	this->panelData.left = left;
	this->panelData.width = width;
	this->panelData.height = height;
}


void WinGuiPlusPanel::Panel::Draw(WinGuiPlusPanel::Panel::DrawFunc callback)
{
	this->callback = callback;
	InvalidateRect(this->panelData.hwnd, NULL, TRUE);
}

bool WinGuiPlusPanel::Panel::windowMove(bool isMove)
{
	return this->panelData.dragEnabled = isMove;
}

PWINGUIPLUS_TEMPLATE WinGuiPlusPanel::Panel::Event(PWINGUIPLUS_TEMPLATE event)
{
	if (event) {
		this->event = *event;
	}
	return &this->event;
}
