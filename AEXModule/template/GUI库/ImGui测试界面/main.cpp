#include "../../../../AEXModule.h"
#include <iostream>

int main()
{
    static ImGuiPlus::window win;
    static ImGuiPlus::window win1;
    static ImGuiPlus::window win2;
    WinGuiPlus::window::INFO info;
    //info.width = GetSystemMetrics(SM_CXSCREEN);
    //info.height = GetSystemMetrics(SM_CYSCREEN);
    info.width = 500;
    info.height = 400;
    info.noBorder = false;
    info.topMost = false;
    static std::string buf;

    win.create(false, true, NULL, L"高级组件演示", L"DEMO", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
        if (WINGUIPLUS_STATUS::DRAW == status) {
            ImGui::MenuItem(u8"测试1");
            ImGui::MenuItem(u8"测试2");
            ImGui::MenuItem(u8"测试3");
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %.1f", ImGui::GetIO().Framerate);
            ImGuiPlus::Button(&win, u8"测试按钮", u8"测试提示", ImVec2(100, 30), ImGuiPlus::BUTTON_STYLE(), [hwnd](WINGUIPLUS_STATUS status) {
                if (WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN == status) {
                    Thread thread;
                    thread.add(0, [&](PThread pthread) {
                        MessageBoxA(nullptr, "测试按钮被点击了", "提示", MB_OK);
                        });
                }
                });
            ImGuiPlus::Input(&win, &buf);
        }
        });
    win1.create(true, true, NULL, L"高级组件演示2", L"DEMO1", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
        if (WINGUIPLUS_STATUS::DRAW == status) {
            win1.getWin()->setDragable();
            ImGuiPlus::Menu(&win1, "测试", [&]() {
                ImGui::MenuItem("测试1");
                ImGui::MenuItem("测试2");
                ImGui::MenuItem("测试3");
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %.1f", ImGui::GetIO().Framerate);
                });
        }
        });
    info.noBorder = true;
    win2.create(true, true, NULL, L"高级组件演示3", L"DEMO2", info, [&, info](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
        if (WINGUIPLUS_STATUS::DRAW == status) {
            ImGui::MenuItem(u8"测试1");
            ImGui::MenuItem(u8"测试2");
            ImGui::MenuItem(u8"测试3");
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %.1f", ImGui::GetIO().Framerate);
            ImGuiPlus::Button(&win2, u8"测试按钮", u8"测试提示", ImVec2(100, 30), ImGuiPlus::BUTTON_STYLE(), [hwnd](WINGUIPLUS_STATUS status) {
                if (WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN == status) {
                    Thread thread;
                    thread.add(0, [&](PThread pthread) {
                        MessageBoxA(nullptr, "测试按钮被点击了", "提示", MB_OK);
                        });
                }
                });
            ImGuiPlus::Input(&win2, &buf);
        }
        });
    ImGuiPlus::run();
}
