#include "../../../h/Gui/ImguiPlus/ImMenu.h"

ImGuiPlusMenu::Menu::Menu(ImGuiPlusWindow::window* win, std::string title, Callback callback, ImGuiWindowFlags style)
{
    this->create(win, title, callback, style);
}

ImGuiPlusMenu::Menu::~Menu()
{
}
bool ImGuiPlusMenu::Menu::create(ImGuiPlusWindow::window* win, std::string title, Callback callback, ImGuiWindowFlags style)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);  // ±£ÁôÔ²½Ç
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f); // ±£ÁôÏ¸±ß¿ò
    bool flag = ImGui::Begin(UTF8(title).c_str(), nullptr, style);
    if (ImGui::IsWindowHovered()) {
        win->getWin()->cancelDragable();
    }
    if(flag) 
    {
        const ImVec4& text_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
        const ImVec4& bg_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        if(callback) callback();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    return flag;
}

