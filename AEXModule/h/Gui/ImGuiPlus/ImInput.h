#pragma once
#include "ImWindow.h"
#include "config.h"
#include <string>
namespace ImGuiPlusInput {
    struct INPUT_STYLE {
        ImFont* font = nullptr;              // 字体对象指针
        ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // 正常文本颜色
        ImVec4 bgColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);        // 输入框背景颜色
        ImVec4 borderColor = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);    // 边框颜色
        ImVec4 hoveredBgColor = ImVec4(0.95f, 0.95f, 0.95f, 1.0f); // 悬停状态背景色
        ImVec4 activeBgColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);   // 激活状态背景色
        ImVec4 cursorColor = ImVec4(0.1f, 0.5f, 1.0f, 1.0f);     // 输入光标颜色
        ImVec4 selectionBgColor = ImVec4(0.26f, 0.59f, 0.98f, 0.35f); // 文本选中背景色
        float borderSize = 1.0f;            // 边框粗细
        ImVec2 padding = ImVec2(8, 6);      // 内边距（需与FramePadding匹配）
        float fontSize = 16.0f;             // 字体大小
        float borderRadius = 4.0f;          // 圆角半径
        float borderShadowStrength = 0.1f;  // 边框阴影强度（需通过ImDrawList自定义绘制）

        void Apply() const {
            // 设置颜色属性（InputText特有参数）
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);               // 文本颜色
            ImGui::PushStyleColor(ImGuiCol_FrameBg, bgColor);              // 背景色
            ImGui::PushStyleColor(ImGuiCol_Border, borderColor);          // 边框色
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, hoveredBgColor); // 悬停背景
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, activeBgColor);  // 激活背景
            ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, selectionBgColor); // 选中背景

            // 设置样式变量
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, borderRadius); // 圆角
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, borderSize);   // 边框粗细
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);        // 内边距

            // 应用自定义字体
            if (font) ImGui::PushFont(font);
        }

        void Unapply() const {
            if (font) ImGui::PopFont();
            ImGui::PopStyleColor(6);  // 弹出6个颜色设置
            ImGui::PopStyleVar(3);     // 弹出3个样式变量
        }
    };
    class Input {
    public:
        typedef std::function<void(WINGUIPLUS_STATUS)> Callback;
    public:
        Input() = default;
        Input(ImGuiPlusWindow::window* win, std::string* content, std::string tooltip = "", ImVec2 size = ImVec2(-1, 0), INPUT_STYLE style = INPUT_STYLE(), Callback callback = nullptr);
        ~Input();
        /*
        * @brief 创建 编辑框
        * @param win 父窗口
        * @param content 编辑框内容
        * @param tooltip  编辑框提示
        * @param size  编辑框尺寸, 默认值为(-1,0)即根据内容自动调整
        * @param callback 点击回调函数, 默认值为nullptr
        *   @param WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN : 点击事件
        *   @param WINGUIPLUS_STATUS::MOUSE_ENTER : 鼠标进入事件
        * @return 创建成功返回true，否则返回false
        */
        bool create(ImGuiPlusWindow::window* win, std::string* content, std::string tooltip = "", ImVec2 size = ImVec2(-1, 0), INPUT_STYLE style = INPUT_STYLE(), Callback callback = nullptr);
    };

};
