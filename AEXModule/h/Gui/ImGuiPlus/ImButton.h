#pragma once
#include "ImWindow.h"
#include "config.h"
#include <string>
namespace ImGuiPlusButton {
    struct BUTTON_STYLE {
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
    class Button {
    public:
        typedef std::function<void(WINGUIPLUS_STATUS)> Callback;
    public:
        Button() = default;
        Button(ImGuiPlusWindow::window* win, std::string title,std::string tooltip = "", ImVec2 size = ImVec2(-1, 0), BUTTON_STYLE style = BUTTON_STYLE(), Callback callback = nullptr);
        ~Button();
        /*
        * @brief 创建按钮
        * @param win 父窗口
        * @param title 按钮标题
        * @param tooltip 按钮提示
        * @param size 按钮尺寸, 默认值为(-1,0)即根据内容自动调整
        * @param style 按钮样式, 默认值为BUTTON_STYLE()
        * @param callback 点击回调函数, 默认值为nullptr
        *   @param WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN : 点击事件
        *   @param WINGUIPLUS_STATUS::MOUSE_ENTER : 鼠标进入事件
        * @return 创建成功返回true，否则返回false
        */
        bool create(ImGuiPlusWindow::window* win, std::string title, std::string tooltip = "", ImVec2 size = ImVec2(-1, 0), BUTTON_STYLE style = BUTTON_STYLE(), Callback callback = nullptr);
    };

};
