#include "AEXModule.h"

int main() {
    {
        static ImGuiPlus::window win;
        WinGuiPlus::window::INFO info;
        //全屏
        //info.width = GetSystemMetrics(SM_CXSCREEN);
        //info.height = GetSystemMetrics(SM_CYSCREEN);
        info.width = 500;
        info.height = 400;
        info.noBorder = true;
        info.topMost = true;

        win.create(true, true, NULL, L"高级组件演示", L"DEMO", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
            if (WINGUIPLUS_STATUS::DRAW == status) {
                ImGui::MenuItem(u8"测试1");
                ImGui::MenuItem(u8"测试2");
                ImGui::MenuItem(u8"测试3");

                // 添加帧数显示
                ImGui::Separator(); // 在菜单底部分隔
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %.1f", ImGui::GetIO().Framerate);
                //ImGuiPlus::Menu(&win, "测试", [&]() {
                //    ImGui::MenuItem("测试1");
                //    ImGui::MenuItem("测试2");
                //    ImGui::MenuItem("测试3");
                //    // 添加帧数显示（在菜单底部）
                //    ImGui::Separator();
                //    ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %.1f", ImGui::GetIO().Framerate);
                //});
            }
         });

        ImGuiPlus::run();
    }
    return 0;
}