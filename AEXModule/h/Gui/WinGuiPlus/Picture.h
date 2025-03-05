#pragma once
#include <string>
#include "Label.h"
#include <map>
namespace WinGuiPlusPicture {
	class Picture {
	public:
		//样式设置
		struct STYLE {
			float radius = 0.0f; //圆角半径
			Gdiplus::Color radiusColor[4] = { Gdiplus::Color(255, 255, 255)/*左上*/, Gdiplus::Color(255, 255, 255)/*右上*/, Gdiplus::Color(255, 255, 255)/*左下*/, Gdiplus::Color(255, 255, 255)/*右下*/ }; //圆角空白补色
			Gdiplus::Color bkcolor = Gdiplus::Color(0, 0, 0); //背景颜色
		};
		Picture() = default;
		~Picture();
		/*
		* 创建编辑框
		* @param parent 父窗口句柄
		* @param hInstance 实例句柄
		* @param top 顶部坐标
		* @param left 左边坐标
		* @param width 宽度
		* @param height 高度
		* @param Id 图片ID（可选，默认值0）,可用于C++资源文件中的id来加载图片
		* @param suffix 图片类型后缀 （可选，默认值PNG）
		* @param isLayered 是否层叠窗口
		* @param visible 是否可见
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, DWORD Id,std::string suffix = "PNG", bool isLayered = false, bool visible = true);
		/*
		* 创建编辑框
		* @param parent 父窗口句柄
		* @param hInstance 实例句柄
		* @param top 顶部坐标
		* @param left 左边坐标
		* @param width 宽度
		* @param height 高度
		* @param imagePath 图片路径（可选，默认值空）
		* @param isLayered 是否层叠窗口
		* @param visible 是否可见
		*/
		void create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string imagePath, bool isLayered = false, bool visible = true);
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
		/*
		* 设置新图片ID
		* @param Id 图片ID
		* @param suffix 图片类型后缀
		*/
		void SetImage(DWORD Id, std::string suffix = "PNG");
		/*
		* 设置新图片路径
		* @param imagePath 图片路径
		*/
		void SetImage(std::string imagePath);
		/*
		* 设置图片事件
		*/
		PWINGUIPLUS_WINEVENTPROC Event();
	public:
		STYLE style;//样式设置
		WinGuiPlusPanel::Panel panel;
		DWORD Id = NULL;// 图片ID
		std::string suffix = "PNG";//图片类型后缀
		std::string imagePath; //图片路径
	};
};