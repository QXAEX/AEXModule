#include "../../../h/Gui/ImguiPlus/ImMenu.h"

ImguiPlusMenu::Menu::Menu(ImguiPlusWindow::window* win, std::string title, ClickCallback callback, ImGuiWindowFlags style)
{
    this->create(win, title, callback, style);
}

ImguiPlusMenu::Menu::~Menu()
{
}
bool ImguiPlusMenu::Menu::create(ImguiPlusWindow::window* win, std::string title, ClickCallback callback, ImGuiWindowFlags style)
{
    if (style == 0) {
        style = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize 
              | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
    }
    // 使用默认配色方案
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);  // 保留圆角
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f); // 保留细边框
    bool flag = ImGui::Begin(UTF8(title).c_str(), nullptr, style);
    if(flag) 
    {
        // 获取当前主题颜色
        const ImVec4& text_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
        const ImVec4& bg_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        
        // 自适应阴影颜色
        const ImU32 shadow_base = ImGui::GetColorU32(ImGuiCol_Border);
        
        // 绘制阴影
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 window_pos = ImGui::GetWindowPos();
        const ImVec2 window_size = ImGui::GetWindowSize();
        
        for (int i = 0; i < 3; i++) {
            ImVec2 rect_min(window_pos.x - i, window_pos.y - i);
            ImVec2 rect_max(window_pos.x + window_size.x + i, 
                          window_pos.y + window_size.y + i);
            
            draw_list->AddRect(
                rect_min, rect_max,
                (shadow_base & 0x00FFFFFF) | ((64 - i*20) << 24), // 保持阴影色相
                8.0f + i*2,
                0,
                1.0f + i*0.5f);
        }

        // 自动适配的标题栏
        ImGui::BeginChild("##titlebar", ImVec2(0, 40));
        ImGui::SetCursorPosY(8);
        // 使用当前文本颜色
        ImGui::Text("%s", UTF8(title).c_str()); 
        ImGui::EndChild();

        if(callback) callback();
    }
    ImGui::End();
    
    ImGui::PopStyleVar(2);
    return flag;
}

