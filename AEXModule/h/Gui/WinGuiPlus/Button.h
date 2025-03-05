#pragma once
#include <string>
#include "Label.h"
namespace WinGuiPlusButton {
	class Button {
	public:
		Button() = default;
		Button(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool isLayered = false, bool visible = true);
		~Button();
		/*
		* ������ť
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
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool isLayered = false, bool visible = true);
		/*
		* ���ð�ť�Ƿ�ɼ�
		* @param visible �Ƿ�ɼ�
		*/
		void SetVisible(bool visible);
		/*
		* ��ȡ��ť�Ƿ�ɼ�
		* @return �Ƿ�ɼ�
		*/
		bool GetVisible() const;
		/*
		* ���ð�ť�¼�
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
		/*
		* ���ð�ť�ı�
		* @param text �ı�����
		*/
		void SetText(std::string text);
		/*
		* ��ȡ��ť�ı�
		* @return �ı�����
		*/
		std::string GetText() const;
		/*
		* ���ð�ť��ʽ
		*/
		WinGuiPlusLabel::Label::STYLE* Style();
	public:
		WinGuiPlusLabel::Label label;
		std::string text;
	};
};