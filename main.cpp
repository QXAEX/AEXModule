#include "AEXModule.h"
#include "AEXModule/imgui/imgui.h"
#include "AEXModule/imgui/backends/imgui_impl_win32.h"
#include "AEXModule/imgui/backends/imgui_impl_dx11.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
static std::string UT8(std::string str) {
    return Text::text_unicode_to_utf8(Text::text_ascii_to_unicode(str));
}

// 自定义样式初始化函数
void SetupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    // 现代色彩方案
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.14f, 0.17f, 0.90f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.14f, 0.17f, 0.99f);
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.22f, 0.27f, 0.50f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.27f, 0.33f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.32f, 0.38f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.55f, 0.95f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.60f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.50f, 0.90f, 1.00f);
}

int main()
{
    static ImguiPlus::window win;
    WinGuiPlus::window::INFO info;
    info.width = 400;
    info.height = 600;
    static float progress = 0.0f;
    static char inputText[128] = "";
    static bool checkState = false;
    static int radioSelect = 0;

    win.create(NULL, L"高级组件演示", L"DEMO", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
        win.getWin()->setDragable();
        SetupImGuiStyle();
        ImGui::BeginChild("InputGroup", ImVec2(0, 150), true);
        {
            ImGui::TextColored(ImVec4(0.25f, 0.55f, 0.95f, 1.00f), UT8("AEX 控制面板").c_str());
            ImGui::Separator();
            ImGui::Text(UT8("文本输入:").c_str());
            ImGui::PushItemWidth(-1);
            ImGui::InputText("##input", inputText, IM_ARRAYSIZE(inputText));

            ImGui::Spacing();

            ImGui::Text(UT8("进度条控制:").c_str());
            ImGui::ProgressBar(progress, ImVec2(-1, 20), "");
            if (ImGui::Button(UT8("增加进度").c_str(), ImVec2(-1, 0))) {
                progress += 0.1f;
                if (progress > 1.0f) progress = 1.0f;
            }
            if (ImGui::IsItemHovered()) {
                win.getWin()->cancelDragable();
            }
        }
        ImGui::EndChild();

        // 第二组：选项控制
        ImGui::BeginChild("OptionGroup", ImVec2(0, 180), true);
        {
            ImGui::Text(UT8("开关设置:").c_str());
            ImGui::Checkbox(UT8("启用特效").c_str(), &checkState);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text(UT8("单选选项:").c_str());
            ImGui::RadioButton(UT8("选项 1").c_str(), &radioSelect, 0);
            ImGui::SameLine();
            ImGui::RadioButton(UT8("选项 2").c_str(), &radioSelect, 1);
            ImGui::SameLine();
            ImGui::RadioButton(UT8("选项 3").c_str(), &radioSelect, 2);

            ImGui::Spacing();

            static int comboSelect = 0;
            const char* items[] = { UT8("选项 A").c_str(), UT8("选项 B").c_str(), UT8("选项 C").c_str() };
            ImGui::Combo(UT8("下拉选择").c_str(), &comboSelect, items, IM_ARRAYSIZE(items));
        }
        ImGui::EndChild();

        // 第三组：图形显示
        ImGui::BeginChild("VisualGroup", ImVec2(0, 0), true);
        {
            // 自定义绘制区域
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();

            // 绘制渐变背景
            draw_list->AddRectFilledMultiColor(
                p,
                ImVec2(p.x + ImGui::GetContentRegionAvail().x, p.y + 80),
                IM_COL32(20, 25, 35, 255),
                IM_COL32(40, 45, 60, 255),
                IM_COL32(40, 45, 60, 255),
                IM_COL32(20, 25, 35, 255)
            );

            // 绘制动态圆
            static float angle = 0.0f;
            angle += 0.02f;
            ImVec2 center = ImVec2(p.x + 50, p.y + 40);
            draw_list->AddCircle(
                center,
                30.0f,
                IM_COL32(255, 255, 255, 100),
                32,
                2.0f
            );
            draw_list->AddLine(
                center,
                ImVec2(
                    center.x + cosf(angle) * 25.0f,
                    center.y + sinf(angle) * 25.0f
                ),
                IM_COL32(255, 100, 100, 255),
                3.0f
            );

            ImGui::Dummy(ImVec2(0, 80));
        }
        ImGui::EndChild();
        });

    ImguiPlus::run();
    return 0;
}