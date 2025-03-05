#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include "data.h"
//�¼�: ���ڴ���
int wm_create(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����
int wm_destroy(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����ƶ�
int wm_mousemove(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���ƴ���
int wm_paint(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �������
int wm_lbuttondown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����ͷ�
int wm_lbuttonup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �Ҽ�����
int wm_rbuttondown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: �Ҽ��ͷ�
int wm_rbuttonup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���̰�������
int wm_keydown(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���̰����ͷ�
int wm_keyup(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ���ڴ�С�ı�
int wm_size(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);
//�¼�: ����λ���ƶ�
int wm_move(std::vector<WINGUIPLUS_TEMPLATE>& tempList, HWND& hwnd, UINT& uMsg, WPARAM& wParam, LPARAM& lParam);