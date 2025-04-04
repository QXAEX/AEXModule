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

void main() {
	return;
	{
		//小作文
		Byteset data = R"(
		在探索未知的路上，每个人都是一部独一无二的机器，驱动着我们前行的动力是内心深处的梦想和对知识的渴望。在这篇文章中，我想与大家分享一个关于追求梦想的故事，以及在这个过程中我们可能会遇到的挑战和如何克服它们。

		故事的主人公是一个名叫李明的年轻人。在李明还小的时候，他就对编程产生了浓厚的兴趣。每当看到电视上那些酷炫的游戏和应用程序，他总是忍不住想要知道，这些是如何制作出来的。然而，对于那时的他而言，编程似乎遥不可及，就像天上的星星，虽然美丽却难以触及。但是，李明并没有放弃，他相信，只要自己努力，总有一天可以实现心中的梦想。

		随着年龄的增长，李明开始利用课余时间自学编程。他订阅了一些编程相关的视频课程，每天都会花费几个小时的时间来学习新知识，尝试编写小程序，解决各种编程问题。每当攻克一个难题时，他都会感到无比的满足和喜悦，这种感觉就像是在黑暗的夜晚找到了一片光明，让他更加坚定了要成为程序员的决心。然而，光靠自学是远远不够的，没有系统的知识，没有实践经验，很难成为一名优秀的程序员。于是，李明决定去上大学，选择计算机科学作为自己的专业。大学里，他遇到了许多志同道合的朋友，他们一起讨论编程问题，一起参加各种编程竞赛，互相鼓励，共同成长。这些经历不仅让他学到了许多编程技巧，也让他结交了人生中的许多宝贵的朋友。

		毕业后，李明进入了一家知名的游戏公司工作。在这里，他遇到了许多优秀的同事，也学到了许多以前从未接触过的知识。他开始负责一些小型项目，然后逐渐接手一些稍大的项目。在工作中，他遇到了各种各样的挑战，有时候甚至会感到有些绝望。但是，每当这个时候，他都会想起自己曾经的梦想，想起那些让自己坚持下去的人，想起那些曾经帮助过自己的书籍和文章，这让他重新找回了信心和勇气。他相信，只要自己不断学习，不断积累经验，总有一天可以成为一名优秀的程序员。

		在这家公司的几年里，李明从一个什么都不懂的新人逐渐成长为一个可以独当一面的程序员。他参与了许多项目的开发，其中有些项目甚至获得了市场的认可和用户的喜爱。每当这个时候，他都会感到无比的自豪和欣慰，因为这些都是自己努力的结果。然而，李明并没有满足于此。他认为，作为一名程序员，自己的责任不仅仅是写出代码，更重要的是能够用代码去创造价值，去改变世界。于是，他开始尝试进行一些创新，希望能够开发出一些新的东西。虽然有时候会失败，但是李明并没有放弃。他相信，失败是成功之母，每一次失败都是对自己的提升。在这样的坚持下，李明最终开发出了一款全新的游戏，这款游戏不仅获得了市场的认可，也获得了用户的喜爱。这不仅让他实现了自己的梦想，也让他找到了自己的价值。

		如今，李明已经成为了一名成功的程序员。他不仅有着稳定的收入，而且还有着自己的作品，这些作品都让他感到非常满足。然而，李明并没有忘记那些曾经帮助过自己的人，那些曾经支持过自己的书籍和文章。他经常参与各种编程社区的讨论，帮助那些遇到困难的人，分享自己的经验和知识。他相信，知识只有被分享，才能发挥最大的价值。在这样的分享中，李明也结交了更多志同道合的朋友，这些朋友都是他的精神支柱，让他们共同成长，共同进步。

		在追求梦想的路上，每个人都可能会遇到各种各样的挑战和困难。但是，只要我们有梦想，有坚持，有勇气，就一定能够克服这些困难，实现自己的梦想。李明的故事告诉我们，编程并不是一件遥不可及的事情，只要我们愿意投入时间和精力，就一定能够成为一名优秀的程序员。同时，他也告诉我们，作为一名程序员，我们的责任不仅仅是写出代码，更重要的是能够用代码去创造价值，去改变世界。只有这样，我们的梦想才能真正地实现，我们的生活才能更加丰富多彩。

		编程的世界是一个充满挑战的世界，但是也是一个充满机遇的世界。在这个世界里，每个人都有机会去实现自己的梦想，去创造属于自己的价值。就像李明一样，他用自己的努力去证明了，只要有梦想，有坚持，有勇气，就一定能够实现自己的梦想。让我们一起加油吧，为了我们的梦想，为了更好的自己。
		)";
		data.append('\0');
		Byteset data1 = data;
		//data = "";
		//data.append("我").append('\0').append("爱").append('\0').append("你").append('\0').append("！").append({ 255 }).append('\0');
		__int64 time = System::GetRunTime();
		Byteset dataBase = Encryption::BaseEncrypt(data);
		std::cout << "加密后：" << dataBase.data() << "，长度：" << dataBase.size() << std::endl;
		printf("加密:%lld毫秒\n", System::GetRunTime() - time);
		time = System::GetRunTime();
		Byteset dataDecrypt = Encryption::BaseDecrypt(dataBase);
		std::cout << "解密后：" << dataDecrypt.data() << "，长度：" << dataDecrypt.size() << std::endl;
		printf("解密:%lld毫秒\n", System::GetRunTime() - time);
		printf("是否相等:%d\n", data1 == data);
	}
	return;
	{
		Memory::R3 r3;
		if (!r3.Open(Process::ProcessGetID("清零计划2_科技登录窗口实例.exe"))) {
			printf("未找到目标程序\n");
			return;
		}
		{
			__int64 time = System::GetRunTime();
			std::vector<PVOID> addrList = r3.Search("24 08 48 83 EC 28 E8 52 FD ?? FF 48 83 C4 28 C3 CC CC CC CC CC CC CC CC CC");
			for (auto addr : addrList) {
				printf("地址：%p\n", addr);
			}
			printf("普通查询耗时:%lld毫秒\n", System::GetRunTime() - time);
		}
		{
			__int64 time = System::GetRunTime();
			std::vector<PVOID> addrList = r3.Search("24 08 48 83 EC 28 E8 52 FD ?? FF 48 83 C4 28 C3 CC CC CC CC CC CC CC CC CC", true);
			for (auto addr : addrList) {
				printf("地址：%p\n", addr);
			}
			printf("【VIP】查询耗时:%lld毫秒\n", System::GetRunTime() - time);
		}
	}

}