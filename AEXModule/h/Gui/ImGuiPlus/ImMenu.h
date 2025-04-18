#pragma once
#include "ImWindow.h"
#include "config.h"
#include <string>
namespace ImGuiPlusMenu {
    class Menu {
    public:
        typedef std::function<void()> Callback;
    public:
        Menu() = default;
        Menu(ImGuiPlusWindow::window* win, std::string title, Callback callback = nullptr, ImGuiWindowFlags style = 0);
        ~Menu();
        /*
        * @brief 创建菜单
        * @param win 父窗口
        * @param title 菜单标题
        * @param callback 点击回调函数
        * @return 创建成功返回true，否则返回false
        */
        bool create(ImGuiPlusWindow::window* win, std::string title, Callback callback = nullptr, ImGuiWindowFlags style = 0);
    };

};
