#pragma once
#include <windows.h>
extern std::vector<WinGuiPlusPanel::Panel*> panelList;
void panelListDelete(HWND hwnd);
WinGuiPlusPanel::Panel* panelListFind(HWND hwnd);
//�¼�: ���ڴ���
int wm_panel_create(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����
int wm_panel_destroy(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����뿪
int wm_panel_mouseleave(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����ƶ�
int wm_panel_mousemove(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���ƴ���
int wm_panel_paint(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �������
int wm_panel_lbuttondown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����ͷ�
int wm_panel_lbuttonup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �Ҽ�����
int wm_panel_rbuttondown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �Ҽ��ͷ�
int wm_panel_rbuttonup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���̰�������
int wm_panel_keydown(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���̰����ͷ�
int wm_panel_keyup(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���ڴ�С�ı�
int wm_panel_size(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����λ���ƶ�
int wm_panel_move(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���˫��
int wm_panel_lbuttondblclk(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �Ҽ�˫��
int wm_panel_rbuttondblclk(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �����ֹ���
int wm_panel_mousewheel(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ��괦��
int wm_panel_setcursor(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ��ʱ��
int wm_panel_timer(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����ʧȥ����
int wm_panel_killfocus(HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);