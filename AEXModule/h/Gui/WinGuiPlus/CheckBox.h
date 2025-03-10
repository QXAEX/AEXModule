#pragma once
#include <string>
#include "./panel/panel.h"
namespace WinGuiPlusCheckBox {
	//复选框
	class CheckBox {
	public:
		//样式设置
		struct STYLE {
			ALIGN align = ALIGN::CENTER_LEFT; //对齐方式, 默认居中左对齐
			bool wrap = false;//是否换行
			int size = 12; //字体大小
			std::wstring name = L"微软雅黑"; //字体名称
			bool bold = false; //是否粗体
			bool italic = false; //是否斜体
			bool underline = false; //是否下划线
			bool strikeout = false; //是否删除线
			float angle = 0.0f; //旋转角度
			float radius = 0.0f; //圆角半径
			Gdiplus::Color radiusColor[4] = { Gdiplus::Color(255, 255, 255)/*左上*/, Gdiplus::Color(255, 255, 255)/*右上*/, Gdiplus::Color(255, 255, 255)/*左下*/, Gdiplus::Color(255, 255, 255)/*右下*/ }; //圆角空白补色
			float padding = 5.0f; //内边距
			Gdiplus::Color color = Gdiplus::Color(0, 0, 0); //字体颜色,默认黑色
			Gdiplus::Color bkcolor = 0; //背景颜色, 如果为0则表示透明
		};
	public:
		CheckBox() = default;
		CheckBox(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text = "test", bool* checked = nullptr, bool isLayered = false, bool visible = true);
		~CheckBox();
		/*
		* 复选框
		* @param parent 父窗口句柄
		* @param hInstance 程序实例句柄
		* @param top 复选框的上边距
		* @param left 复选框的左边距
		* @param width 复选框的宽度
		* @param height 复选框的高度
		* @param text 复选框的文本
		* @param checked 是否选中, 默认为false
		* @param isLayered 是否层叠窗口
		* @param visible 是否可见
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text = "test", bool* checked = nullptr, bool isLayered = false, bool visible = true);
		/*
		* 设置复选框的文本
		* @param text 复选框的文本
		*/
		void SetText(std::string text);
		/*
		* 获取复选框的文本
		* @return 复选框的文本
		*/
		std::string GetText() const;
		/*
		* 设置复选框是否选中
		* @param checked 是否选中
		*/
		void SetChecked(bool checked);	
		/*
		* 获取复选框是否选中
		* @return 是否选中
		*/
		bool GetChecked() const;
		/*
		* 设置复选框是否可见
		* @param visible 是否可见
		*/
		void SetVisible(bool visible);
		/*
		* 获取复选框是否可见
		* @return 是否可见
		*/
		bool GetVisible() const;
		/*
		* 设置复选框事件
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
	public:
		WinGuiPlusCheckBox::CheckBox::STYLE style;
		std::string text;
		bool* checked;
		WinGuiPlusPanel::Panel panel;
	};
};