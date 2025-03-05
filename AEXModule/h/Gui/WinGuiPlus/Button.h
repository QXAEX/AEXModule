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
		* 创建按钮
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
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool isLayered = false, bool visible = true);
		/*
		* 设置按钮是否可见
		* @param visible 是否可见
		*/
		void SetVisible(bool visible);
		/*
		* 获取按钮是否可见
		* @return 是否可见
		*/
		bool GetVisible() const;
		/*
		* 设置按钮事件
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
		/*
		* 设置按钮文本
		* @param text 文本内容
		*/
		void SetText(std::string text);
		/*
		* 获取按钮文本
		* @return 文本内容
		*/
		std::string GetText() const;
		/*
		* 设置按钮样式
		*/
		WinGuiPlusLabel::Label::STYLE* Style();
	public:
		WinGuiPlusLabel::Label label;
		std::string text;
	};
};