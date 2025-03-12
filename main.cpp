//// AEXModule.cpp : 锟斤拷锟侥硷拷锟斤拷锟斤拷 "main" 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷执锟叫斤拷锟节此达拷锟斤拷始锟斤拷锟斤拷锟斤拷锟斤拷
////

#include "AEXModule.h" 
#include <iostream>
#include <thread>
#include <windows.h>
#include <string>
static std::string key = "Please enter the card password你好";
static Gdiplus::Color bgColor = Gdiplus::Color(237, 237, 237);
static Byteset mkey = 3220896020;
static LPCWSTR fontName = L"黑体";// 可用字体：微软雅黑、宋体、黑体、Arial、Verdana、Georgia、Tahoma、Times New Roman
/*
* 窗口_登录
* @param title 标题
* @param name 软件名
* @param notice 公告内容
* @param key 卡密
*/
static WinGuiPlus::window loginWin(LPCWSTR title, LPCSTR name, LPCSTR notice, std::string* key) {
	WinGuiPlus::window win;
	WinGuiPlus::window::INFO info;
	info.width = 350;
	info.height = 440;
	info.alpha = 200;
	info.backgroundColor = RGB(237, 237, 237);
	win.create(NULL, title, L"LOGIN", info, [&, info](HWND hwnd, HINSTANCE hInstance, int status) {
		static WinGuiPlus::Label lb_title(hwnd, hInstance, 10, 0, info.width, 30, name);
		lb_title.style.align = ALIGN::CENTER;
		lb_title.style.size = 20;
		lb_title.style.name = fontName;
		lb_title.style.bkcolor = bgColor;
		static WinGuiPlus::Label lb_notice(hwnd, hInstance, 50, 0, info.width, 200, notice);
		lb_notice.style.align = ALIGN::TOP_LEFT;
		lb_notice.style.name = fontName;
		lb_notice.style.wrap = true;
		lb_notice.style.bkcolor = bgColor;
		static WinGuiPlus::Edit ed_key(hwnd, hInstance, 260, 20, info.width - 55, 30, key);
		ed_key.style.borderWidth = 1;
		static WinGuiPlus::Button btn_login(hwnd, hInstance, 300, 20, info.width - 55, 50, "Login");
		btn_login.Style()->size = 20;
		btn_login.Style()->name = L"Times New Roman";
		btn_login.Style()->bkcolor = Gdiplus::Color(168, 41, 42);
		btn_login.Style()->color = Gdiplus::Color(255, 255, 255);
		btn_login.Style()->radius = 10;
		btn_login.Event()->mouseLeft = [hwnd, key](int state, int x, int y) {
			if (state == WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN) {
				try {
					if (mkey == Byteset(std::stoll(*key))) {
						MessageBox(hwnd, L"登录成功", L"提示", MB_OK);
						return;
					}
				}
				catch (...) {
					MessageBox(hwnd, L"卡密格式错误", L"提示", MB_OK);
					return;
				}
				MessageBox(hwnd, L"卡密错误", L"提示", MB_OK);
			}
			};
		for (auto& c : btn_login.Style()->radiusColor) c = bgColor;
		static WinGuiPlus::Label lb_ChangeBind(hwnd, hInstance, 360, 20, 50, 30, "换绑");
		lb_ChangeBind.style.align = ALIGN::CENTER;
		lb_ChangeBind.style.underline = true;
		lb_ChangeBind.style.bkcolor = bgColor;
		lb_ChangeBind.Event()->mouseLeft = [hwnd](int state, int x, int y) {
			if (state == WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN) {
				MessageBox(hwnd, L"功能暂未开放", L"提示", MB_OK);
			}
			};
		static WinGuiPlus::Label lb_Close(hwnd, hInstance, 360, info.width - 35 - 50, 50, 30, "关闭");
		lb_Close.style.align = ALIGN::CENTER;
		lb_Close.style.underline = true;
		lb_Close.style.bkcolor = bgColor;
		lb_Close.Event()->mouseLeft = [](int state, int x, int y) {
			if (state == WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN) {
				exit(0);
			}
			};
		});
	return win;
}
static PE pe;
static void 输出区段() {
	std::vector<PE_SECTION_HEADER> table = pe.GetSectionHeaders();
	for (auto& dir : table) {
		printf("节名称：%s，起始地址：0x%08X，大小：0x%08X\n", dir.Name, dir.VirtualAddress, dir.SizeOfRawData);
	}
}
static void 输出导入表() {
	std::vector<PE_IMPORT_DESCRIPTOR> table = pe.GetImportTable();
	for (auto& dir : table) {
		printf("DLL名：%s，方法数量：%d\n",dir.DLLName.c_str(), dir.Functions.size());
		if (dir.DLLName.find("Network.dll") != std::string::npos) {
			for (PE_IMPORT_FUNCTION&  data :dir.Functions) {
				printf("-------> 函数名：%s，序号：%d\n", data.Name.c_str(), data.Ordinal);
			}
		}
	}
}
void main() {
	//WinGuiPlus::window login = loginWin(L"AEX登录器1.0", "AEX：登录", "当前版本1.0\nQ群: 165820307", &key);
	//WinGuiPlus::run();
}