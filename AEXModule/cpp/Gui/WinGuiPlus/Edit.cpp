#include "../../../h/Gui/WinGuiPlus/Edit.h"
#include "../../../h/Gui/WinGuiPlus/panel/panel_msg.h"
#include <string>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
static float MeasureTextWidth(const std::string& text, const Gdiplus::Font& font, Gdiplus::Graphics* graphics) {
    std::wstring wtext(text.begin(), text.end());
    Gdiplus::RectF rect;
    graphics->MeasureString(wtext.c_str(), static_cast<int>(wtext.length()), &font, Gdiplus::PointF(0, 0), &rect);
    return rect.Width - 3;
}
static void ClampTextOffsetX(WinGuiPlusEdit::Edit* edit, float maxOffset) {
    if (edit->textOffsetX < 0) {
        edit->textOffsetX = 0;
    }
    else if (edit->textOffsetX > maxOffset) {
        edit->textOffsetX = maxOffset;
    }
}
static void EditCursorDraw(WinGuiPlusEdit::Edit* edit, WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics) {
    if (panelData->params[1]) {
        std::string* text = edit->text;
        float y; // 光标的 Y 位置
        float cursorHeight;
        float cursorWidth = 2.0f; // 默认宽度

        Gdiplus::Font font(edit->style.name.c_str(), static_cast<float>(edit->style.size), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

        float spaceWidth = MeasureTextWidth(" ", font, graphics);

        if (!text->empty() && edit->CursorPos[1] != NULL && edit->CursorPos[1] <= text->length()) {
            std::string subText = text->substr(0, edit->CursorPos[1]);
            float cursorX = MeasureTextWidth(subText, font, graphics);

            if (!subText.empty() && subText.back() == ' ') {
                cursorX += spaceWidth + edit->style.size / 2;
            }

            std::wstring wtext(text->begin(), text->end());
            Gdiplus::RectF fullTextRect;
            graphics->MeasureString(wtext.c_str(), static_cast<int>(wtext.length()), &font, Gdiplus::PointF(0, 0), &fullTextRect);

            if (cursorX - edit->textOffsetX < 0) {
                edit->textOffsetX = cursorX;
            }
            else if (cursorX - edit->textOffsetX > panelData->width) {
                edit->textOffsetX = cursorX - panelData->width;
            }

            float maxOffset = fullTextRect.Width - panelData->width;
            if (maxOffset < 0) maxOffset = 0;
            ClampTextOffsetX(edit, maxOffset);

            y = (panelData->height - fullTextRect.Height) / 2;
            cursorHeight = fullTextRect.Height;

            float cursorPos = cursorX - edit->textOffsetX;
            Gdiplus::SolidBrush brush(edit->style.bdcolor);
            graphics->FillRectangle(&brush, cursorPos, y, cursorWidth, cursorHeight);
        }
        else {
            cursorHeight = (float)(edit->style.size * 1.3);
            y = (panelData->height - cursorHeight) / 2;
            float cursorPos = 0;
            Gdiplus::SolidBrush brush(edit->style.bdcolor);
            graphics->FillRectangle(&brush, cursorPos, y, cursorWidth, cursorHeight);
        }
    }
}
static void EditDraw(WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics) {
    WinGuiPlusEdit::Edit* edit = reinterpret_cast<WinGuiPlusEdit::Edit*>(panelData->params1);
    if (!edit) return;

    Gdiplus::SolidBrush brush(edit->style.bkcolor);
    graphics->FillRectangle(&brush, 0, 0, panelData->width, panelData->height);

    EditCursorDraw(edit, panelData, graphics);

    Gdiplus::Font font(edit->style.name.c_str(), static_cast<float>(edit->style.size), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush textBrush(edit->style.color);

    std::string* text = edit->text;
    if (text) {
        std::wstring wtext(text->begin(), text->end());
        Gdiplus::RectF textRect;
        graphics->MeasureString(wtext.c_str(), static_cast<int>(wtext.length()), &font, Gdiplus::PointF(0, 0), &textRect);

        float x = -edit->textOffsetX;
        float y = (panelData->height - textRect.Height) / 2;
        graphics->DrawString(wtext.c_str(), static_cast<int>(wtext.length()), &font, Gdiplus::PointF(x, y), nullptr, &textBrush);
    }
}
WinGuiPlusEdit::Edit::Edit(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string* text, bool isLayered, bool visible)
{
    this->create(parent, hInstance, top, left, width, height, text, isLayered, visible);
}
WinGuiPlusEdit::Edit::~Edit()
{
}
void WinGuiPlusEdit::Edit::create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string* text, bool isLayered, bool visible)
{
    this->lb_border.style.radius = this->style.radius;
    this->lb_border.style.bkcolor = this->style.bdcolor;
    this->lb_border.style.padding = NULL;
    this->lb_border.create(parent, hInstance, top, left, width, height,"", isLayered, visible);
    this->panel.callback = EditDraw;
    this->panel.panelData.params1 = this;
    this->panel.panelData.componentType = COMPONENT_TYPE::EDIT;
    this->text = text;
    this->CursorPos[1] = text->length();
    this->panel.create(parent, "AEXEdit", hInstance, top + this->style.borderWidth, left + this->style.borderWidth, width - this->style.borderWidth * 2, height - this->style.borderWidth * 2, isLayered, visible);
    HIMC hImc = ImmGetContext(this->panel.panelData.hwnd);
    ImmSetOpenStatus(hImc, TRUE);
    ImmReleaseContext(this->panel.panelData.hwnd, hImc);
    this->panel.Event()->event.setCursor = [&]() {//设置鼠标光标
        SetCursor(LoadCursor(NULL, IDC_IBEAM));
        };
    this->panel.Event()->event.mouseLeft = [&](int state, int x, int y) {//鼠标左键按下
        if (state == WINGUIPLUS_STATUS::MOUSE_LEFT_UP) {
            this->panel.panelData.params[0] = TRUE;//定义鼠标光标焦点状态
            this->panel.panelData.params[1] = TRUE;//定义输入光标显示状态,FALSE为显示光标,TRUE为不显示光标
            SetTimer(this->panel.panelData.hwnd,0, 500, (TIMERPROC)this->TimerProc);//启动编辑光标定时器
            InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
        }
        };
    this->panel.Event()->event.killFocus = [&]() {//失去焦点
        this->panel.panelData.params[0] = FALSE;
        this->panel.panelData.params[1] = FALSE;
        KillTimer(this->panel.panelData.hwnd,NULL);
        InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
        };
    this->panel.Event()->event.key = [&](int state, WPARAM& key, LPARAM& lParam) {
        if (state == WINGUIPLUS_STATUS::KEY_DOWN) {
            switch (key) {
            case VK_BACK: // Backspace键
                if (this->text->length() > 0 && this->CursorPos[1] > 0) {
                    this->text->erase(this->CursorPos[1] - 1, 1);
                    this->CursorPos[1]--;
                }
                break;
            case VK_DELETE: // Delete键
                this->text->clear();
                this->CursorPos[1] = 0;
                break;
            case VK_LEFT: // 左箭头键
                this->CursorPos[0] = TRUE;
                if (this->CursorPos[1] > 0) {
                    this->CursorPos[1]--;
                }
                break;
            case VK_RIGHT: // 右箭头键
                if (this->CursorPos[1] < this->text->length()) {
                    this->CursorPos[1]++;
                    if (this->CursorPos[1] == this->text->length()) {
                        this->CursorPos[0] = FALSE;
                    }
                }
                break;
            case VK_HOME: // Home键
                this->CursorPos[1] = 0;
                break;
            case VK_END: // End键
                this->CursorPos[1] = this->text->length();
                break;
                case VK_CONTROL: // 控制键
                    break;
            default:
                bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                char outputChar{};
                switch (key) {
                case VK_OEM_4: outputChar = isShiftPressed ? '{' : '['; break;
                case VK_OEM_6: outputChar = isShiftPressed ? '}' : ']';break;
                case VK_OEM_1: outputChar = isShiftPressed ? ':' : ';';break;
                case VK_OEM_7: outputChar = isShiftPressed ? '"' : '\'';break;
                case VK_OEM_COMMA: outputChar = isShiftPressed ? '<' : ',';break;
                case VK_OEM_PERIOD: outputChar = isShiftPressed ? '>' : '.';break;
                case VK_OEM_MINUS: outputChar = isShiftPressed ? '_' : '-';break;
                case VK_OEM_PLUS: outputChar = isShiftPressed ? '+' : '=';break;
                case VK_OEM_2: outputChar = isShiftPressed ? '?' : '/';break;
                case VK_OEM_5: outputChar = isShiftPressed ? '|' : '\\';break;
                case VK_OEM_3: outputChar = isShiftPressed ? '~' : '`';break;
                default:
                    // 处理其他可打印字符
                    BYTE keyboardState[256];
                    if (GetKeyboardState(keyboardState)) {
                        WCHAR buffer[2];
                        int result = ToUnicode(key, lParam >> 16, keyboardState, buffer, 2, 0);
                        if (result == 1) {
                            char ansiChar[2];
                            WideCharToMultiByte(CP_ACP, 0, buffer, 1, ansiChar, 2, NULL, NULL);
                            outputChar = ansiChar[0];
                        }
                    }
                }
                this->text->insert(this->CursorPos[1], 1, outputChar);
                this->CursorPos[1]++;
                break;
            }
            if(!this->CursorPos[0]) this->CursorPos[1] = strlen(this->text->c_str());
            this->panel.panelData.params[1] = TRUE;
            InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
        }
        };

}
void WinGuiPlusEdit::Edit::SetVisible(bool visible)
{
    this->lb_border.SetVisible(visible);
    this->panel.SetVisible(visible);
}
bool WinGuiPlusEdit::Edit::GetVisible() const
{
    return this->panel.panelData.visible;
}
void WinGuiPlusEdit::Edit::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    WinGuiPlusPanel::Panel* panel = panelListFind(hwnd);
    if (panel) {
        if (panel->panelData.params[0]) {//焦点获取
            panel->panelData.params[1] = !(BOOL)panel->panelData.params[1];
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
}
