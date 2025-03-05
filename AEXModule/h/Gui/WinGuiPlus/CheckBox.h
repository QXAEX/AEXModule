#pragma once
#include <string>
#include "./panel/panel.h"
namespace WinGuiPlusCheckBox {
	//��ѡ��
	class CheckBox {
	public:
		//��ʽ����
		struct STYLE {
			ALIGN align = ALIGN::CENTER_LEFT; //���뷽ʽ, Ĭ�Ͼ��������
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
		CheckBox() = default;
		CheckBox(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text = "test", bool* checked = nullptr, bool isLayered = false, bool visible = true);
		~CheckBox();
		/*
		* ��ѡ��
		* @param parent �����ھ��
		* @param hInstance ����ʵ�����
		* @param top ��ѡ����ϱ߾�
		* @param left ��ѡ�����߾�
		* @param width ��ѡ��Ŀ��
		* @param height ��ѡ��ĸ߶�
		* @param text ��ѡ����ı�
		* @param checked �Ƿ�ѡ��, Ĭ��Ϊfalse
		* @param isLayered �Ƿ�������
		* @param visible �Ƿ�ɼ�
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text = "test", bool* checked = nullptr, bool isLayered = false, bool visible = true);
		/*
		* ���ø�ѡ����ı�
		* @param text ��ѡ����ı�
		*/
		void SetText(std::string text);
		/*
		* ��ȡ��ѡ����ı�
		* @return ��ѡ����ı�
		*/
		std::string GetText() const;
		/*
		* ���ø�ѡ���Ƿ�ѡ��
		* @param checked �Ƿ�ѡ��
		*/
		void SetChecked(bool checked);	
		/*
		* ��ȡ��ѡ���Ƿ�ѡ��
		* @return �Ƿ�ѡ��
		*/
		bool GetChecked() const;
		/*
		* ���ø�ѡ���Ƿ�ɼ�
		* @param visible �Ƿ�ɼ�
		*/
		void SetVisible(bool visible);
		/*
		* ��ȡ��ѡ���Ƿ�ɼ�
		* @return �Ƿ�ɼ�
		*/
		bool GetVisible() const;
		/*
		* ���ø�ѡ���¼�
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
	public:
		WinGuiPlusCheckBox::CheckBox::STYLE style;
		std::string text;
		bool* checked;
		WinGuiPlusPanel::Panel panel;
	};
};