#include "../../../h/Gui/ImguiPlus/ImSliderFloat.h"

ImGuiPlusSliderFloat::SliderFloat::SliderFloat(ImGuiPlusWindow::window* win, PFLOAT value, ImVec2 range, std::string title, std::string tooltip, SLIDERFLOAT_STYLE style, Callback callback)
{
	this->create(win, value, range, title, tooltip, style, callback);
}

ImGuiPlusSliderFloat::SliderFloat::~SliderFloat()
{
}

bool ImGuiPlusSliderFloat::SliderFloat::create(ImGuiPlusWindow::window* win, PFLOAT value, ImVec2 range, std::string title, std::string tooltip, SLIDERFLOAT_STYLE style, Callback callback)
{
	style.Apply();
	ImGui::PushItemWidth(-1);
	bool flag = ImGui::SliderFloat(std::string("##").append(title).c_str(), value, range.x, range.y, title.append("%.0f").c_str());
	ImGui::PopItemWidth();
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
