#pragma once
#include <string>
#include "Label.h"
namespace WinGuiPlusEdit {
	class Edit {
	public:
		//样式设置
		struct STYLE {
			int borderWidth = 2;//边框宽度
			std::wstring name = L"Courier New"; //字体名称
			int size = 12; //字体大小
			bool wrap = false;//是否换行
			float radius = 0.0f; //圆角半径
			Gdiplus::Color radiusColor[4] = { Gdiplus::Color(255, 255, 255)/*左上*/, Gdiplus::Color(255, 255, 255)/*右上*/, Gdiplus::Color(255, 255, 255)/*左下*/, Gdiplus::Color(255, 255, 255)/*右下*/ }; //圆角空白补色
			Gdiplus::Color color = Gdiplus::Color(0, 0, 0); //字体颜色,默认黑色
			Gdiplus::Color bkcolor = 0; //背景颜色, 如果为0则表示透明
			Gdiplus::Color bdcolor = Gdiplus::Color(155, 0, 0, 0); //边框颜色, 如果为0则表示透明
		};
		Edit() = default;
		Edit(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string* text, bool isLayered = false, bool visible = true);
		~Edit();
		/*
		* 创建编辑框
		* @param parent 父窗口句柄
		* @param hInstance 实例句柄
		* @param top 顶部坐标
		* @param left 左边坐标
		* @param width 宽度
		* @param height 高度
		* @param text 文本内容
		* @param isLayered 是否层叠窗口
		* @param visible 是否可见
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string* text, bool isLayered = false, bool visible = true);
		/*
		* 设置编辑框是否可见
		* @param visible 是否可见
		*/
		void SetVisible(bool visible);
		/*
		* 获取编辑框是否可见
		* @return 是否可见
		*/
		bool GetVisible() const;
	private:
		static void TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	public:
		WinGuiPlusEdit::Edit::STYLE style;
		WinGuiPlusPanel::Panel panel;
		WinGuiPlusLabel::Label lb_border;
		std::string* text = nullptr;
		size_t CursorPos[4] = { 0,0,0,0 };//光标位置,flag:是否自由位置,x,y,textWidth
		float textOffsetX = NULL; // 水平偏移量，用于文本滚动
	};
};