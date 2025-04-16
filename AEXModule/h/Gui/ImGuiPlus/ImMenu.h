#pragma once
#include "ImWindow.h"
#include "config.h"
#include <string>
namespace ImguiPlusMenu {
    struct MENU_STYLE {
        std::string title;// 改为值类型
        ImVec2 size = ImVec2(-1, 0);// 合并width/height
        ImFont* font = nullptr; // 字体指针
        ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);// 正常文本颜色
        ImVec4 bgColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);// 正常背景颜色
        ImVec4 borderColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);// 正常边框颜色
        ImVec4 hoverBgColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);// 悬停背景颜色
        ImVec4 activeColor = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);// 按下文本颜色
        ImVec4 activeBgColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);// 按下背景颜色
        ImVec4 activeBorderColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);// 按下边框颜色
        float borderSize = 1.0f;// 边框大小
        ImVec2 padding = ImVec2(5, 5);// 内边距
        float fontSize = 16.0f;// 字体大小
        float borderRadius = 5.0f;// 圆角半径
        void Apply() const {
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
            ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverBgColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeBgColor);
            ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, activeBorderColor);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, borderRadius);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, borderSize);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
            if (font) ImGui::PushFont(font);
        }
        void Unapply() const {
            if (font) ImGui::PopFont();
            ImGui::PopStyleColor(7);
            ImGui::PopStyleVar(3);
        }
    };
    typedef std::function<void()> ClickCallback;
    class Menu {
    public:
        Menu() = default;
        Menu(ImguiPlusWindow::window* win, std::string title, ClickCallback callback = nullptr, ImGuiWindowFlags style = 0);
        ~Menu();
        /*
        * @brief 创建按钮
        * @param win 父窗口
        * @param title 按钮标题
        * @param style 按钮样式
        * @param callback 按钮点击回调函数
        *   @param 【鼠标移动】WINGUIPLUS_STATUS::MOUSE_MOVE：用于判断鼠标是否在此组件绘制范围内
        *   @param 【鼠标左键按下】WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN：用于触发按钮点击事件
        * @return 创建成功返回true，否则返回false
        */
        bool create(ImguiPlusWindow::window* win, std::string title,ClickCallback callback = nullptr, ImGuiWindowFlags style = 0);
    };

};
