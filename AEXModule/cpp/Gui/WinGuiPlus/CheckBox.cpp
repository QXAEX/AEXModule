#include "../../../h/Gui/WinGuiPlus/CheckBox.h"
#include <string>
#include <sstream>
static void CheckBoxDraw(WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics) {
    WinGuiPlusCheckBox::CheckBox* checkBox = reinterpret_cast<WinGuiPlusCheckBox::CheckBox*>(panelData->params1);
    if (!checkBox) return;
    //��ѡ����ƿ��
    int checkBoxUiWidth = checkBox->style.size + 10, checkBoxUiHeight = checkBox->style.size;
    // ��ʼ��������ʽ
    int fontStyle = Gdiplus::FontStyleRegular;
    if (checkBox->style.bold) fontStyle |= Gdiplus::FontStyleBold;
    if (checkBox->style.italic) fontStyle |= Gdiplus::FontStyleItalic;
    if (checkBox->style.underline) fontStyle |= Gdiplus::FontStyleUnderline;
    if (checkBox->style.strikeout) fontStyle |= Gdiplus::FontStyleStrikeout;
    Gdiplus::Font font(checkBox->style.name.c_str(), static_cast<float>(checkBox->style.size), fontStyle, Gdiplus::UnitPixel);
    Gdiplus::SolidBrush textBrush(checkBox->style.color);
    // ���Ʊ���
    if (checkBox->style.bkcolor.GetValue() != 0) {
        Gdiplus::SolidBrush bkBrush(checkBox->style.bkcolor);

        // �Ȼ����ĸ������С����
        int radius = checkBox->style.radius;
        if (checkBox->style.radius) {
            // �������Ͻ�
            Gdiplus::SolidBrush radiusColor1(checkBox->style.radiusColor[0]);
            graphics->FillRectangle(&radiusColor1, -1, -1, radius, radius);
            // �������Ͻ�
            Gdiplus::SolidBrush radiusColor2(checkBox->style.radiusColor[1]);
            graphics->FillRectangle(&radiusColor2, panelData->width - radius, 0, radius, radius);
            // �������½�
            Gdiplus::SolidBrush radiusColor3(checkBox->style.radiusColor[2]);
            graphics->FillRectangle(&radiusColor3, panelData->width - radius, panelData->height - radius, radius, radius);
            // �������½�
            Gdiplus::SolidBrush radiusColor4(checkBox->style.radiusColor[3]);
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
    std::wstring text = Text::text_to_wstr(checkBox->GetText());
    float maxWidth = panelData->width - 2 * checkBox->style.padding;
    float yOffset = checkBox->style.padding;
    Gdiplus::RectF layoutRect;
    graphics->MeasureString(text.c_str(), -1, &font, Gdiplus::PointF(0, 0), &layoutRect);
    float x = 0.0f;
    switch (checkBox->style.align) {
    case ALIGN::TOP_LEFT: x = checkBox->style.padding + checkBoxUiWidth; break;
    case ALIGN::TOP_CENTER: x = (panelData->width - layoutRect.Width) / 2 + checkBoxUiWidth; break;
    case ALIGN::TOP_RIGHT: x = panelData->width - layoutRect.Width - checkBox->style.padding; break;
    case ALIGN::CENTER_LEFT: x = checkBox->style.padding + checkBoxUiWidth; break;
    case ALIGN::CENTER: x = (panelData->width - layoutRect.Width) / 2 + checkBoxUiWidth; break;
    case ALIGN::CENTER_RIGHT: x = panelData->width - layoutRect.Width - checkBox->style.padding; break;
    case ALIGN::BOTTOM_LEFT: x = checkBox->style.padding + checkBoxUiWidth; break;
    case ALIGN::BOTTOM_CENTER: x = (panelData->width - layoutRect.Width) / 2 + checkBoxUiWidth; break;
    case ALIGN::BOTTOM_RIGHT: x = panelData->width - layoutRect.Width - checkBox->style.padding; break;
    }
    // ȷ�� Y ��ƫ����
    if (checkBox->style.align == ALIGN::CENTER_LEFT || checkBox->style.align == ALIGN::CENTER || checkBox->style.align == ALIGN::CENTER_RIGHT) {
         yOffset = (panelData->height - layoutRect.Height) / 2; // ��ֱ����
    }
    else if (checkBox->style.align == ALIGN::BOTTOM_LEFT || checkBox->style.align == ALIGN::BOTTOM_CENTER || checkBox->style.align == ALIGN::BOTTOM_RIGHT) {
        yOffset = panelData->height - layoutRect.Height - checkBox->style.padding; // �ײ�����
    }
    if (checkBox->style.angle != 0.0f) {
        Gdiplus::Matrix originalTransform;
        graphics->GetTransform(&originalTransform);
        // ������ת����
        Gdiplus::PointF rotateCenter(x + layoutRect.Width / 2, yOffset + layoutRect.Height / 2);
        graphics->TranslateTransform(rotateCenter.X, rotateCenter.Y);
        graphics->RotateTransform(checkBox->style.angle, Gdiplus::MatrixOrderPrepend);
        graphics->TranslateTransform(-rotateCenter.X, -rotateCenter.Y);
        graphics->DrawString(text.c_str(), -1, &font, Gdiplus::PointF(x, yOffset), &textBrush);
        graphics->SetTransform(&originalTransform);
    }
    else {
        graphics->DrawString(text.c_str(), -1, &font, Gdiplus::PointF(x, yOffset), &textBrush);
    }
    //������
    // ����һ��Pen��ʹ��textBrush����ɫ
    Gdiplus::Pen pen(&textBrush);
    checkBoxUiWidth -= 10;
    // ������ε�λ�úʹ�С
    Gdiplus::RectF checkBoxRect(x - checkBoxUiWidth - 5,
        (Gdiplus::REAL)(yOffset * 1.1),
        (Gdiplus::REAL)checkBoxUiWidth,
        (Gdiplus::REAL)checkBoxUiHeight);

    // ���ƾ��α߿�
    graphics->DrawRectangle(&pen, checkBoxRect.X, checkBoxRect.Y, checkBoxRect.Width, checkBoxRect.Height);
    if (*checkBox->checked) {
        graphics->DrawString(L"��", -1, &font, Gdiplus::PointF(x - checkBoxUiWidth - 5, (Gdiplus::REAL)(yOffset * 1.05)), &textBrush);
    }
}

WinGuiPlusCheckBox::CheckBox::CheckBox(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool* checked, bool isLayered, bool visible)
{
    this->create(parent, hInstance, top, left, width, height, text, checked, isLayered, visible);
}

WinGuiPlusCheckBox::CheckBox::~CheckBox()
{
}

void WinGuiPlusCheckBox::CheckBox::create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string text, bool* checked, bool isLayered, bool visible)
{
    this->panel.callback = CheckBoxDraw;
    this->panel.panelData.componentType = COMPONENT_TYPE::CHECKBOX;
    this->panel.panelData.params1 = this;
    this->text = text;
    this->checked = checked;
    this->panel.create(parent, "AEXCheckBox", hInstance, top, left, width, height, isLayered, visible);
}


void WinGuiPlusCheckBox::CheckBox::SetText(std::string text)
{
    this->text = text;
    InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
}

std::string WinGuiPlusCheckBox::CheckBox::GetText() const
{
    return this->text;
}

void WinGuiPlusCheckBox::CheckBox::SetChecked(bool checked)
{
    *this->checked = checked;
    InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
}

bool WinGuiPlusCheckBox::CheckBox::GetChecked() const
{
    return *this->checked;
}

void WinGuiPlusCheckBox::CheckBox::SetVisible(bool visible)
{
    this->panel.SetVisible(visible);
}

bool WinGuiPlusCheckBox::CheckBox::GetVisible() const
{
    return this->panel.panelData.visible;
}

PWINGUIPLUS_WINEVENTPROC WinGuiPlusCheckBox::CheckBox::Event()
{
    return &this->panel.Event()->event;
}
