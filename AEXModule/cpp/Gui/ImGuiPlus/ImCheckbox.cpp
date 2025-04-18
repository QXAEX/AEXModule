#include "../../../h/Gui/ImguiPlus/ImCheckbox.h"

ImGuiPlusCheckbox::Checkbox::Checkbox(ImGuiPlusWindow::window* win, std::string title, std::string tooltip, bool* checked, CHECKBOX_STYLE style, Callback callback)
{
	this->create(win, title, tooltip, checked, style, callback);
}

ImGuiPlusCheckbox::Checkbox::~Checkbox()
{
}

bool ImGuiPlusCheckbox::Checkbox::create(ImGuiPlusWindow::window* win, std::string title, std::string tooltip, bool* checked, CHECKBOX_STYLE style, Callback callback)
{
	style.Apply();
	bool flag = ImGui::Checkbox(title.c_str(), checked);
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
