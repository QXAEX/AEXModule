#include "../../../../AEXModule.h"
#include "resource.h"
static std::string key = "Please enter the card password你好";
static Gdiplus::Color bgColor = Gdiplus::Color(255, 237, 237, 237);
static Byteset mkey = 3220896020;
static LPCWSTR fontName = L"黑体";// 可用字体：微软雅黑、宋体、黑体、Arial、Verdana、Georgia、Tahoma、Times New Roman
static void loginWin(WinGuiPlus::window* win, LPCWSTR title, LPCSTR name, LPCSTR notice, std::string* key);// 登录窗口
static void ChangeBind(WinGuiPlus::window* win, HWND parent);// 换绑窗口
static void FuncWin(WinGuiPlus::window* win);// 功能窗口
WinGuiPlus::window login;
WinGuiPlus::window changeBind;
WinGuiPlus::window funcWin;
int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	loginWin(&login, L"AEX测试", "测试软件", "测试软件公告", &key);
	login.setTopMost();
	WinGuiPlus::run(); 
	return 0;
}

void loginWin(WinGuiPlus::window* win, LPCWSTR title, LPCSTR name, LPCSTR notice, std::string* key) {
	WinGuiPlus::window::INFO info;
	info.width = 350;
	info.height = 440;
	info.alpha = 200;
	info.backgroundColor = RGB(237, 237, 237);
	info.iconId = IDI_ICON1;
	win->create(NULL, title, L"LOGIN", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status) {
		static WinGuiPlus::Label lb_log;//此页全局log标签
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
						FuncWin(&funcWin);
						return;
					}
				}
				catch (...) {
					lb_log.SetText("卡密格式错误");
					return;
				}
				lb_log.SetText("卡密错误");
			}
			};
		for (auto& c : btn_login.Style()->radiusColor) c = bgColor;
		static WinGuiPlus::Label lb_ChangeBind(hwnd, hInstance, 360, 20, 50, 30, "换绑");
		lb_ChangeBind.style.align = ALIGN::CENTER;
		lb_ChangeBind.style.underline = true;
		lb_ChangeBind.style.bkcolor = bgColor;
		lb_ChangeBind.Event()->mouseLeft = [&, hwnd](int state, int x, int y) {
			if (state == WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN) {
				ChangeBind(&changeBind, hwnd);
				changeBind.disableParentClick();
			}
			};
		lb_log.create(hwnd, hInstance, 360, 70, info.width - 35 - 100, 30, "等待操作...");
		lb_log.style.align = ALIGN::CENTER;
		lb_log.style.underline = true;
		lb_log.style.bkcolor = bgColor;
		static WinGuiPlus::Label lb_Close(hwnd, hInstance, 360, info.width - 35 - 50, 50, 30, "关闭");
		lb_Close.style.align = ALIGN::CENTER;
		lb_Close.style.underline = true;
		lb_Close.style.bkcolor = bgColor;
		lb_Close.Event()->mouseLeft = [&](int state, int x, int y) {
			if (state == WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN) {
				win->close();
			}
			};
		});
}

void ChangeBind(WinGuiPlus::window* win, HWND parent)
{
	WinGuiPlus::window::INFO info;
	info.width = 350;
	info.height = 440;
	info.alpha = 200;
	info.backgroundColor = RGB(237, 237, 237);
	info.iconId = IDI_ICON1;
	win->create(parent, L"换绑", L"CHANGEBIND", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status) {
		static WinGuiPlus::Label lb_title;
		lb_title.create(hwnd, hInstance, 10, 0, info.width, 50, "自己绘制此页功能");
		lb_title.style.align = ALIGN::CENTER;
		lb_title.style.size = 20;
		lb_title.style.name = fontName;
		lb_title.style.bkcolor = bgColor;
	});
}

void FuncWin(WinGuiPlus::window* win)
{
	WinGuiPlus::window::INFO info;
	info.width = 350;
	info.height = 440;
	info.alpha = 200;
	info.backgroundColor = RGB(237, 237, 237);
	info.iconId = IDI_ICON1;
	win->create(NULL, L"功能页", L"FUNCWIN", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status) {
		if (status == WINGUIPLUS_STATUS::CREATE) {
			static WinGuiPlus::Picture picture;
			picture.create(hwnd, hInstance, 0, 0, info.width, info.height, "D:\\IMG\\bg-1.png");
			static WinGuiPlus::CheckBox checkbox;
			static bool flag;
			checkbox.create(hwnd, hInstance, 10, 10, 100, 30, "选项1", &flag);
			login.hide();
		}
		else if (status == WINGUIPLUS_STATUS::DESTROY) {
			login.show();
		}
	});
}
