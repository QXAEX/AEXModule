#include "../../../h/Gui/WinGuiPlus/Label.h"
#include <string>
#include <sstream>
static void LabelDraw(WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics) {
    WinGuiPlusLabel::Label* label = reinterpret_cast<WinGuiPlusLabel::Label*>(panelData->params1);
    if (!label) return;
    // ��ʼ��������ʽ
    int fontStyle = Gdiplus::FontStyleRegular;
    if (label->style.bold) fontStyle |= Gdiplus::FontStyleBold;
    if (label->style.italic) fontStyle |= Gdiplus::FontStyleItalic;
    if (label->style.underline) fontStyle |= Gdiplus::FontStyleUnderline;
    if (label->style.strikeout) fontStyle |= Gdiplus::FontStyleStrikeout;
    Gdiplus::Font font(label->style.name.c_str(), static_cast<float>(label->style.size), fontStyle, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush textBrush(label->style.color);
    // ���Ʊ���
    if (label->style.bkcolor.GetValue() != 0) {
        Gdiplus::SolidBrush bkBrush(label->style.bkcolor);

        // �Ȼ����ĸ������С����
        int radius = label->style.radius;
        if (label->style.radius) {
            // �������Ͻ�
            Gdiplus::SolidBrush radiusColor1(label->style.radiusColor[0]);
            graphics->FillRectangle(&radiusColor1, -1, -1, radius, radius);
            // �������Ͻ�
            Gdiplus::SolidBrush radiusColor2(label->style.radiusColor[1]);
            graphics->FillRectangle(&radiusColor2, panelData->width - radius, 0, radius, radius);
            // �������½�
            Gdiplus::SolidBrush radiusColor3(label->style.radiusColor[2]);
            graphics->FillRectangle(&radiusColor3, panelData->width - radius, panelData->height - radius, radius, radius);
            // �������½�
            Gdiplus::SolidBrush radiusColor4(label->style.radiusColor[3]);
            graphics->FillRectangle(&radiusColor4, 0, panelData->height - radius, radius, radius);
            // Ȼ�����Բ�Ǿ���
            Gdiplus::GraphicsPath bgPath;
            bgPath.AddArc(-1, -1, radius * 2, radius * 2, 180, 90); // ���Ͻ�
            bgPath.AddArc(panelData->width - radius * 2, -1, radius * 2, radius * 2, 270, 90); // ���Ͻ�
            bgPath.AddArc(panelData->width - radius * 2, panelData->height + 1 - radius * 2, radius * 2, radius * 2, 0, 90); // ���½�
            bgPath.AddArc(-1, panelData->height - radius * 2, radius * 2, radius * 2, 90, 90); // ���½�
            bgPath.CloseFigure();
            graphics->FillPath(&bkBrush, &bgPath);
        }
        else {
            graphics->FillRectangle(&bkBrush, -1, -1, panelData->width + 1, panelData->height + 1);
        }
    }
    std::wstring text = Text::text_to_wstr(label->GetText());
    float maxWidth = panelData->width - 2 * label->style.padding; 
    std::vector<std::wstring> words;
    std::wstring currentLine;
    std::wistringstream iss(text);
    std::wstring word;
    if (label->style.wrap) {
        while (iss >> word) {
            std::wstring newLine = currentLine.empty() ? word : currentLine + L" " + word;
            Gdiplus::RectF layoutRect;
            graphics->MeasureString(newLine.c_str(), -1, &font, Gdiplus::PointF(0, 0), &layoutRect);
            if (layoutRect.Width <= maxWidth) {
                currentLine = newLine;
            }
            else {
                if (!currentLine.empty()) words.push_back(currentLine);
                currentLine = word;
            }
        }
        if (!currentLine.empty()) words.push_back(currentLine);
    }
    else words.push_back(text);
    float yOffset = label->style.padding; 
    for (const auto& line : words) {
        Gdiplus::RectF layoutRect;
        graphics->MeasureString(line.c_str(), -1, &font, Gdiplus::PointF(0, 0), &layoutRect);
        float x = 0.0f;
        switch (label->style.align) {
            case ALIGN::TOP_LEFT: x = label->style.padding; break;
            case ALIGN::TOP_CENTER: x = (panelData->width - layoutRect.Width) / 2; break;
            case ALIGN::TOP_RIGHT: x = panelData->width - layoutRect.Width - label->style.padding; break;
            case ALIGN::CENTER_LEFT: x = label->style.padding; break;
            case ALIGN::CENTER: x = (panelData->width - layoutRect.Width) / 2; break;
            case ALIGN::CENTER_RIGHT: x = panelData->width - layoutRect.Width - label->style.padding; break;
            case ALIGN::BOTTOM_LEFT: x = label->style.padding; break;
            case ALIGN::BOTTOM_CENTER: x = (panelData->width - layoutRect.Width) / 2; break;
            case ALIGN::BOTTOM_RIGHT: x = panelData->width - layoutRect.Width - label->style.padding; break;
        }
        // ȷ�� Y ��ƫ����
        if (label->style.align == ALIGN::CENTER_LEFT || label->style.align == ALIGN::CENTER || label->style.align == ALIGN::CENTER_RIGHT) {
            if (&line == &words[0]) yOffset = (panelData->height - (layoutRect.Height * words.size())) / 2; // ��ֱ����
        } else if (label->style.align == ALIGN::BOTTOM_LEFT || label->style.align == ALIGN::BOTTOM_CENTER || label->style.align == ALIGN::BOTTOM_RIGHT) {
            if (&line == &words[0]) yOffset = panelData->height - (layoutRect.Height * words.size()) - label->style.padding; // �ײ�����
        }
        if (label->style.angle != 0.0f) {
            Gdiplus::Matrix originalTransform;
            graphics->GetTransform(&originalTransform);
            // ������ת����
            Gdiplus::PointF rotateCenter(x + layoutRect.Width / 2, yOffset + layoutRect.Height / 2);
            graphics->TranslateTransform(rotateCenter.X, rotateCenter.Y);
            graphics->RotateTransform(label->style.angle, Gdiplus::MatrixOrderPrepend);
            graphics->TranslateTransform(-rotateCenter.X, -rotateCenter.Y);
            graphics->DrawString(line.c_str(), -1, &font, Gdiplus::PointF(x, yOffset), &textBrush);
            graphics->SetTransform(&originalTransform);
        } else {
            graphics->DrawString(line.c_str(), -1, &font, Gdiplus::PointF(x, yOffset), &textBrush);
        }

        yOffset += layoutRect.Height + (label->style.size * 0.1f); // �м��
    }
}

WinGuiPlusLabel::Label::Label(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool isLayered, bool visible)
{
    this->create(parent, hInstance, top, left, width, height, text, isLayered, visible);
}

WinGuiPlusLabel::Label::~Label()
{
}

void WinGuiPlusLabel::Label::create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool isLayered, bool visible)
{
    this->panel.callback = LabelDraw;
    this ->panel.panelData.componentType = COMPONENT_TYPE::LABEL;
    this->panel.panelData.params1 = this;
    this->text = text;
    this->panel.create(parent, "AEXLabel", hInstance, top, left, width, height, isLayered, visible);
}

void WinGuiPlusLabel::Label::SetText(std::string text)
{
    this->text = text;
    InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
}

std::string WinGuiPlusLabel::Label::GetText() const
{
	return this->text;
}

void WinGuiPlusLabel::Label::SetVisible(bool visible)
{
	this->panel.SetVisible(visible);
}

bool WinGuiPlusLabel::Label::GetVisible() const
{
	return this->panel.panelData.visible;
}

PWINGUIPLUS_WINEVENTPROC WinGuiPlusLabel::Label::Event()
{
    return &this->panel.Event()->event;
}
