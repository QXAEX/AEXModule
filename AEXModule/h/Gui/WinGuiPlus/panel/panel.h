#pragma once
#include <string>
#include <vector>
#include <wtypes.h>
#include <algorithm>
#include <gdiplus.h>
#include "../../../Text.h"
#include "../data.h"
#pragma comment(lib, "gdiplus.lib")
namespace WinGuiPlusPanel {
	class Panel {
	public:
		//������ݽṹ
		typedef struct PanelData {
			COMPONENT_TYPE componentType;//�������
			HINSTANCE hInstance;//ģ����
			HWND parent;//�����ھ��
			HWND hwnd;//���ھ��
			int top, left, width, height;//λ�á���С
			bool isLayered;//�Ƿ�Ϊ�������
			bool visible;//�Ƿ�ɼ�
			bool dragEnabled;//�Ƿ������϶�
			bool isDragging = false;//�Ƿ������϶�(�ڲ��䶯)
			POINT dragOffset = { 0,0 };
			bool Cursor = false;//����Ƿ��������(�ڲ��䶯)
			PVOID params1 = nullptr;//�Զ������1
			size_t params[5] = { NULL };//(�Զ���������ڲ�ʹ��)
		}*PPanelData;
		/*
	* ���ƻص���������
	* @param panel ���ָ��
	* @param hdc �豸�����ľ��
	*/
		typedef void (*DrawFunc)(WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics);
	public:
		Panel() = default;
		/*
		* ���
		* @param parent �����ھ��
		* @param className ����
		* @param top ���Ͻ�y���꣬Ĭ��0
		* @param left ���Ͻ�x���꣬Ĭ��0
		* @param width ����ȣ�Ĭ��0
		* @param height ���߶ȣ�Ĭ��0
		* @param isLayered �Ƿ�Ϊ�������,Ĭ��false
		* @param visible �Ƿ�ɼ���Ĭ��true
		*/
		Panel(HWND parent, LPCSTR className, HINSTANCE hInstance, int top = 0, int left = 0, int width = 0, int height = 0, bool isLayered = false, bool visible = true);
		/*
		* �������
		* @param parent �����ھ��
		* @param className ����
		* @param top ���Ͻ�y���꣬Ĭ��0
		* @param left ���Ͻ�x���꣬Ĭ��0
		* @param width ����ȣ�Ĭ��0
		* @param height ���߶ȣ�Ĭ��0
		* @param isLayered �Ƿ�Ϊ�������,Ĭ��false
		* @param visible �Ƿ�ɼ���Ĭ��true
		*/
		void create(HWND parent, LPCSTR className = "Panel", HINSTANCE hInstance = GetModuleHandle(NULL), int top = 0, int left = 0, int width = 0, int height = 0, bool isLayered = false, bool visible = true);
		/*
		* ���ÿɼ�
		* @param visible �Ƿ�ɼ���Ĭ��false
		*/
		void SetVisible(bool visible = false);
		/*
		* ���û�����Ϣ
		* @param top ���Ͻ�y���꣬Ĭ��0
		* @param left ���Ͻ�x���꣬Ĭ��0
		* @param width ����ȣ�Ĭ��0
		* @param height ���߶ȣ�Ĭ��0
		*/
		void SetBasicInfo(int top = 0, int left = 0, int width = 0, int height = 0);
		/*
		* �����¼�
		* @param callback ���ƻص�����
		*/
		void Draw(WinGuiPlusPanel::Panel::DrawFunc callback);
		/*
		* �����ƶ�����ģʽ(�ڲ��ģʽ����Ч)
		* @param isMove �Ƿ�������Ĭ��true
		*/
		bool windowMove(bool isMove = true);
		/*
		* �����¼�
		* @return �¼�ָ�뼯��
		*/
		PWINGUIPLUS_TEMPLATE Event(PWINGUIPLUS_TEMPLATE event = nullptr);
	private:
		WINGUIPLUS_TEMPLATE event;
	public:
		PanelData panelData;
		WinGuiPlusPanel::Panel::DrawFunc callback = nullptr;
	};
};