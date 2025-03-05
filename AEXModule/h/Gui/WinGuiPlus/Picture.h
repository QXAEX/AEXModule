#pragma once
#include <string>
#include "Label.h"
#include <map>
namespace WinGuiPlusPicture {
	class Picture {
	public:
		//��ʽ����
		struct STYLE {
			float radius = 0.0f; //Բ�ǰ뾶
			Gdiplus::Color radiusColor[4] = { Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/, Gdiplus::Color(255, 255, 255)/*����*/ }; //Բ�ǿհײ�ɫ
			Gdiplus::Color bkcolor = Gdiplus::Color(0, 0, 0); //������ɫ
		};
		Picture() = default;
		~Picture();
		/*
		* �����༭��
		* @param parent �����ھ��
		* @param hInstance ʵ�����
		* @param top ��������
		* @param left �������
		* @param width ���
		* @param height �߶�
		* @param Id ͼƬID����ѡ��Ĭ��ֵ0��,������C++��Դ�ļ��е�id������ͼƬ
		* @param suffix ͼƬ���ͺ�׺ ����ѡ��Ĭ��ֵPNG��
		* @param isLayered �Ƿ�������
		* @param visible �Ƿ�ɼ�
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, DWORD Id,std::string suffix = "PNG", bool isLayered = false, bool visible = true);
		/*
		* �����༭��
		* @param parent �����ھ��
		* @param hInstance ʵ�����
		* @param top ��������
		* @param left �������
		* @param width ���
		* @param height �߶�
		* @param imagePath ͼƬ·������ѡ��Ĭ��ֵ�գ�
		* @param isLayered �Ƿ�������
		* @param visible �Ƿ�ɼ�
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string imagePath, bool isLayered = false, bool visible = true);
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
		/*
		* ������ͼƬID
		* @param Id ͼƬID
		* @param suffix ͼƬ���ͺ�׺
		*/
		void SetImage(DWORD Id, std::string suffix = "PNG");
		/*
		* ������ͼƬ·��
		* @param imagePath ͼƬ·��
		*/
		void SetImage(std::string imagePath);
		/*
		* ����ͼƬ�¼�
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
	public:
		STYLE style;//��ʽ����
		WinGuiPlusPanel::Panel panel;
		DWORD Id = NULL;// ͼƬID
		std::string suffix = "PNG";//ͼƬ���ͺ�׺
		std::string imagePath; //ͼƬ·��
	};
};