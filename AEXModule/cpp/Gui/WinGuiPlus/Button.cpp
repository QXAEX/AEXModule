#include "../../../h/Gui/WinGuiPlus/Button.h"

WinGuiPlusButton::Button::Button(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool isLayered, bool visible)
{
    this->create(parent, hInstance, top, left, width, height, text, isLayered, visible);
}

WinGuiPlusButton::Button::~Button()
{
}

void WinGuiPlusButton::Button::create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool isLayered, bool visible)
{
    this->label.style.align = ALIGN::CENTER;
    this->text = text;
    this->label.create(parent,hInstance, top, left, width, height, text, isLayered, visible);
    this->label.panel.Event()->event.setCursor = [&]() {
        //设置光标为点击图标的光标
        SetCursor(LoadCursor(NULL, IDC_HAND));
        };
}

void WinGuiPlusButton::Button::SetVisible(bool visible)
{
    this->label.panel.SetVisible(visible);
}

bool WinGuiPlusButton::Button::GetVisible() const
{
    return this->label.panel.panelData.visible;
}

PWINGUIPLUS_WINEVENTPROC WinGuiPlusButton::Button::Event()
{
	return &this->label.panel.Event()->event;
}

WinGuiPlusLabel::Label::STYLE* WinGuiPlusButton::Button::Style()
{
    return &this->label.style;
}
