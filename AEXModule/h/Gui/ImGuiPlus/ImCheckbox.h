#pragma once
#include "ImWindow.h"
#include "config.h"
#include <string>
namespace ImGuiPlusCheckbox {
    struct CHECKBOX_STYLE {
        ImFont* font = nullptr; // 字体指针
        ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 0.9f);// 正常文本颜色
        ImVec4 bgColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);// 正常背景颜色
        ImVec4 borderColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);// 正常边框颜色
        ImVec4 hoverBgColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);// 悬停背景颜色
        ImVec4 activeBgColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);// 按下背景颜色
        ImVec4 activeBorderColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);// 按下边框颜色
        ImVec4 MarkColor = ImVec4(0.1f, 0.6f, 0.9f, 1.0f);// 选中状态对勾颜色
        ImVec2 padding = ImVec2(2.5, 2.5);// 内边距
        float borderRadius = 5.0f;// 圆角半径
        void Apply() const {
            if (font) ImGui::PushFont(font);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, borderRadius);
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, bgColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, hoverBgColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, activeBgColor);
            ImGui::PushStyleColor(ImGuiCol_CheckMark, MarkColor); // 激活状态对勾颜色
        }

        void Unapply() const {
            if (font) ImGui::PopFont();
            ImGui::PopStyleVar(2);  // FramePadding + FrameRounding
            ImGui::PopStyleColor(5); // 文本/背景/对勾颜色
        }
    };
    class Checkbox {
    public:
        typedef std::function<void(WINGUIPLUS_STATUS)> Callback;
    public:
        Checkbox() = default;
        Checkbox(ImGuiPlusWindow::window* win, std::string title, std::string tooltip = "", bool* checked = nullptr, CHECKBOX_STYLE style = CHECKBOX_STYLE(), Callback callback = nullptr);
        ~Checkbox();
        /*
        * @brief 创建复选框
        * @param win 父窗口
        * @param title 复选框标题
        * @param tooltip 复选框提示
        * @param checked 复选框状态，默认值为nullptr
        * @param style 复选框样式
        * @param callback 点击回调函数, 默认值为nullptr
        *   @param WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN : 鼠标点击事件
        *   @param WINGUIPLUS_STATUS::MOUSE_ENTER : 鼠标进入事件
        * @return 创建成功返回true，否则返回false
        */
        bool create(ImGuiPlusWindow::window* win, std::string title, std::string tooltip = "", bool* checked = nullptr, CHECKBOX_STYLE style = CHECKBOX_STYLE(), Callback callback = nullptr);
    };

};
