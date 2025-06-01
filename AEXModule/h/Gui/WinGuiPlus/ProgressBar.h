#pragma once
#include "./panel/panel.h"
#include <string>
#include <map>
namespace WinGuiPlusProgressBar {
	class ProgressBar {
	public:
		//样式设置
		struct STYLE {
			float radius = 0.0f; //圆角半径, 如果高度为20，则设置12的圆角最合适
			Gdiplus::Color radiusColor[4] = { Gdiplus::Color(255, 255, 255)/*左上*/, Gdiplus::Color(255, 255, 255)/*右上*/, Gdiplus::Color(255, 255, 255)/*左下*/, Gdiplus::Color(255, 255, 255)/*右下*/ }; //圆角空白补色
			Gdiplus::Color bkcolor = Gdiplus::Color(255, 255, 255); //背景颜色
			Gdiplus::Color pbcolor = Gdiplus::Color(0, 144, 0); //进度条颜色
			Gdiplus::Color textcolor = Gdiplus::Color(168, 41, 42); //文字颜色
			Gdiplus::Color textcolorf = Gdiplus::Color(150, 255, 255, 200); // 文字描边发光色（带透明度）
			float maxGlowSize = 3.0f; // 文字描边最大发光半径
		};
		ProgressBar() = default;
		ProgressBar(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, float* progress, bool isLayered = false, bool visible = true);
		~ProgressBar();
		/*
		* 创建进度条
		* @param parent 父窗口句柄
		* @param hInstance 实例句柄
		* @param top 顶部坐标
		* @param left 左边坐标
		* @param width 宽度
		* @param height 高度
		* @param progress 进度
		* @param isLayered 是否层叠窗口
		* @param visible 是否可见
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, float* progress, bool isLayered = false, bool visible = true);
		/*
		* 设置进度条是否可见
		* @param visible 是否可见
		*/
		void SetVisible(bool visible);
		/*
		* 获取进度条是否可见
		* @return 是否可见
		*/
		bool GetVisible() const;
		/*
		* 设置进度条进度
		* @param progress 进度
		*/	
		void SetProgress(float progress);
		/*
		* 获取进度条进度
		* @return 进度
		*/
		float GetProgress() const;
		/*
		* 设置进度条事件
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
	public:
		STYLE style;//样式设置
		WinGuiPlusPanel::Panel panel;
		PFLOAT progress;
	};
};