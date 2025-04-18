#include "../../../h/Gui/ImguiPlus/ImInput.h"

ImGuiPlusInput::Input::Input(ImGuiPlusWindow::window* win, std::string* content, std::string tooltip, ImVec2 size, INPUT_STYLE style, Callback callback)
{
	this->create(win, content, tooltip, size, style, callback);
}

ImGuiPlusInput::Input::~Input()
{
}

bool ImGuiPlusInput::Input::create(ImGuiPlusWindow::window* win, std::string* content, std::string tooltip, ImVec2 size, INPUT_STYLE style, Callback callback)
{
	style.Apply();
	if (content->size() == 0) content->resize(100);
	bool flag = ImGui::InputText("##hidden_label", content->data(), content->size());
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