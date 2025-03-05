#pragma once
#include <string>
#include "./panel/panel.h"
namespace WinGuiPlusLabel {
	// ��ǩ
	class Label
	{
	public:
		//��ʽ����
		struct STYLE {
			ALIGN align = ALIGN::TOP_RIGHT; //���뷽ʽ, Ĭ�Ͼ��������
			bool wrap = false;//�Ƿ���
			int size = 12; //�����С
			std::wstring name = L"΢���ź�"; //��������
			bool bold = false; //�Ƿ����
			bool italic = false; //�Ƿ�б��
			bool underline = false; //�Ƿ��»���
			bool strikeout = false; //�Ƿ�ɾ����
			float angle = 0.0f; //��ת�Ƕ�
			float radius = 0.0f; //Բ�ǰ뾶
			Gdiplus::Color radiusColor[4] = { Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/ }; //Բ�ǿհײ�ɫ
			float padding = 5.0f; //�ڱ߾�
			Gdiplus::Color color = Gdiplus::Color(0, 0, 0); //������ɫ,Ĭ�Ϻ�ɫ
			Gdiplus::Color bkcolor = 0; //������ɫ, ���Ϊ0���ʾ͸��
		};
	public:
		Label() = default;
		Label(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text = "test", bool isLayered = false, bool visible = true);
		~Label();
		/*
		* ��ǩ
		* @param parent �����ھ��
		* @param hInstance ����ʵ�����
		* @param top ��ǩ���ϱ߾�
		* @param left ��ǩ����߾�
		* @param width ��ǩ�Ŀ��
		* @param height ��ǩ�ĸ߶�
		* @param text ��ǩ���ı�
		* @param isLayered �Ƿ�������
		* @param visible �Ƿ�ɼ�
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text = "test", bool isLayered = false, bool visible = true);
		/*
		* ���ñ�ǩ���ı�
		* @param text ��ǩ���ı�
		*/
		void SetText(std::string text);
		/*
		* ��ȡ��ǩ���ı�
		* @return ��ǩ���ı�
		*/
		std::string GetText() const;
		/*
		* ���ñ�ǩ�Ƿ�ɼ�
		* @param visible �Ƿ�ɼ�
		*/
		void SetVisible(bool visible);
		/*
		* ��ȡ��ǩ�Ƿ�ɼ�
		* @return �Ƿ�ɼ�
		*/
		bool GetVisible() const;
		/*
		* ���ñ�ǩ�¼�
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
	public:
		WinGuiPlusLabel::Label::STYLE style;
		std::string text;
		WinGuiPlusPanel::Panel panel;
	};
};