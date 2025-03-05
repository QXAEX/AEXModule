#pragma once
#include <string>
#include "Label.h"
namespace WinGuiPlusEdit {
	class Edit {
	public:
		//��ʽ����
		struct STYLE {
			int borderWidth = 2;//�߿���
			std::wstring name = L"Courier New"; //��������
			int size = 12; //�����С
			bool wrap = false;//�Ƿ���
			float radius = 0.0f; //Բ�ǰ뾶
			Gdiplus::Color radiusColor[4] = { Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/ }; //Բ�ǿհײ�ɫ
			Gdiplus::Color color = Gdiplus::Color(0, 0, 0); //������ɫ,Ĭ�Ϻ�ɫ
			Gdiplus::Color bkcolor = 0; //������ɫ, ���Ϊ0���ʾ͸��
			Gdiplus::Color bdcolor = Gdiplus::Color(155, 0, 0, 0); //�߿���ɫ, ���Ϊ0���ʾ͸��
		};
		Edit() = default;
		Edit(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string* text, bool isLayered = false, bool visible = true);
		~Edit();
		/*
		* �����༭��
		* @param parent �����ھ��
		* @param hInstance ʵ�����
		* @param top ��������
		* @param left �������
		* @param width ���
		* @param height �߶�
		* @param text �ı�����
		* @param isLayered �Ƿ�������
		* @param visible �Ƿ�ɼ�
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string* text, bool isLayered = false, bool visible = true);
		/*
		* ���ñ༭���Ƿ�ɼ�
		* @param visible �Ƿ�ɼ�
		*/
		void SetVisible(bool visible);
		/*
		* ��ȡ�༭���Ƿ�ɼ�
		* @return �Ƿ�ɼ�
		*/
		bool GetVisible() const;
	private:
		static void TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	public:
		WinGuiPlusEdit::Edit::STYLE style;
		WinGuiPlusPanel::Panel panel;
		WinGuiPlusLabel::Label lb_border;
		std::string* text = nullptr;
		size_t CursorPos[4] = { 0,0,0,0 };//���λ��,flag:�Ƿ�����λ��,x,y,textWidth
		float textOffsetX = NULL; // ˮƽƫ�����������ı�����
	};
};