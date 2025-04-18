#include "../../../h/Gui/ImguiPlus/ImButton.h"

ImGuiPlusButton::Button::Button(ImGuiPlusWindow::window* win, std::string title, std::string tooltip, ImVec2 size, BUTTON_STYLE style, Callback callback)
{
	this->create(win, title, tooltip, size, style, callback);
}

ImGuiPlusButton::Button::~Button()
{
}

bool ImGuiPlusButton::Button::create(ImGuiPlusWindow::window* win, std::string title, std::string tooltip, ImVec2 size, BUTTON_STYLE style, Callback callback)
{
	style.Apply();
	bool flag = ImGui::Button(title.c_str(), size);
	style.Unapply();
	if (flag)
	{
		if (callback) callback(WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN);
	}
	if (ImGui::IsItemHovered()) {
		win->getWin()->cancelDragable();
		if (tooltip.size() > 0) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(tooltip.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		if (callback) callback(WINGUIPLUS_STATUS::MOUSE_ENTER);
	}

	return flag;
}