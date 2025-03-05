#pragma once
#include <string>
#include <vector>
#include <wtypes.h>
#include <algorithm>
#include <gdiplus.h>
#include "../../../Text.h"
#include "../data.h"
#pragma comment(lib, "gdiplus.lib")
namespace WinGuiPlusPanel {
	class Panel {
	public:
		//面板数据结构
		typedef struct PanelData {
			COMPONENT_TYPE componentType;//组件类型
			HINSTANCE hInstance;//模块句柄
			HWND parent;//父窗口句柄
			HWND hwnd;//窗口句柄
			int top, left, width, height;//位置、大小
			bool isLayered;//是否为层叠窗口
			bool visible;//是否可见
			bool dragEnabled;//是否允许拖动
			bool isDragging = false;//是否正在拖动(内部变动)
			POINT dragOffset = { 0,0 };
			bool Cursor = false;//鼠标是否在面板上(内部变动)
			PVOID params1 = nullptr;//自定义参数1
			size_t params[5] = { NULL };//(自定义参数，内部使用)
		}*PPanelData;
		/*
	* 绘制回调函数类型
	* @param panel 面板指针
	* @param hdc 设备上下文句柄
	*/
		typedef void (*DrawFunc)(WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics);
	public:
		Panel() = default;
		/*
		* 面板
		* @param parent 父窗口句柄
		* @param className 类名
		* @param top 左上角y坐标，默认0
		* @param left 左上角x坐标，默认0
		* @param width 面板宽度，默认0
		* @param height 面板高度，默认0
		* @param isLayered 是否为层叠窗口,默认false
		* @param visible 是否可见，默认true
		*/
		Panel(HWND parent, LPCSTR className, HINSTANCE hInstance, int top = 0, int left = 0, int width = 0, int height = 0, bool isLayered = false, bool visible = true);
		/*
		* 创建面板
		* @param parent 父窗口句柄
		* @param className 类名
		* @param top 左上角y坐标，默认0
		* @param left 左上角x坐标，默认0
		* @param width 面板宽度，默认0
		* @param height 面板高度，默认0
		* @param isLayered 是否为层叠窗口,默认false
		* @param visible 是否可见，默认true
		*/
		void create(HWND parent, LPCSTR className = "Panel", HINSTANCE hInstance = GetModuleHandle(NULL), int top = 0, int left = 0, int width = 0, int height = 0, bool isLayered = false, bool visible = true);
		/*
		* 设置可见
		* @param visible 是否可见，默认false
		*/
		void SetVisible(bool visible = false);
		/*
		* 设置基本信息
		* @param top 左上角y坐标，默认0
		* @param left 左上角x坐标，默认0
		* @param width 面板宽度，默认0
		* @param height 面板高度，默认0
		*/
		void SetBasicInfo(int top = 0, int left = 0, int width = 0, int height = 0);
		/*
		* 绘制事件
		* @param callback 绘制回调函数
		*/
		void Draw(WinGuiPlusPanel::Panel::DrawFunc callback);
		/*
		* 启动移动窗口模式(在层叠模式下有效)
		* @param isMove 是否启动，默认true
		*/
		bool windowMove(bool isMove = true);
		/*
		* 设置事件
		* @return 事件指针集合
		*/
		PWINGUIPLUS_TEMPLATE Event(PWINGUIPLUS_TEMPLATE event = nullptr);
	private:
		WINGUIPLUS_TEMPLATE event;
	public:
		PanelData panelData;
		WinGuiPlusPanel::Panel::DrawFunc callback = nullptr;
	};
};