#pragma once
#include "ImWindow.h"
#include "config.h"
#include <string>
namespace ImGuiPlusSliderFloat {
    struct SLIDERFLOAT_STYLE {
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
            // 字体系统
            if (font) ImGui::PushFont(font);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, borderRadius);
            ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, borderSize * 2); // 抓取点尺寸
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, bgColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, hoverBgColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, activeBgColor);
            ImGui::PushStyleColor(ImGuiCol_SliderGrab, activeColor);       // 滑块常态
            ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, borderColor); // 滑块激活态
        }

        void Unapply() const {
            // 字体系统
            if (font) ImGui::PopFont();
            ImGui::PopStyleVar(3);  // FramePadding + FrameRounding + GrabMinSize
            ImGui::PopStyleColor(6); // 所有颜色设置
        }
    };
    class SliderFloat {
    public:
        typedef std::function<void(WINGUIPLUS_STATUS)> Callback;
    public:
        SliderFloat() = default;
        SliderFloat(ImGuiPlusWindow::window* win, PFLOAT value, ImVec2 range, std::string title, std::string tooltip = "", SLIDERFLOAT_STYLE style = SLIDERFLOAT_STYLE(), Callback callback = nullptr);
        ~SliderFloat();
        /*
        * @brief 创建刻度条
        * @param win 父窗口
        * @param value 刻度条初始值
        * @param range 范围
        * @param title 标题, 请不要出现重复的标题，如果出现重复标题会导致事件响应同步的情况
        * @param tooltip 刻度条提示, 默认为空
        * @param style 刻度条样式, 默认值为SliderFloat_STYLE()
        * @param callback 点击回调函数, 默认值为nullptr
        *   @param WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN : 点击事件
        *   @param WINGUIPLUS_STATUS::MOUSE_ENTER : 鼠标进入事件
        * @return 创建成功返回true，否则返回false
        */
        bool create(ImGuiPlusWindow::window* win, PFLOAT value, ImVec2 range, std::string title, std::string tooltip = "", SLIDERFLOAT_STYLE style = SLIDERFLOAT_STYLE(), Callback callback = nullptr);
    };

};
