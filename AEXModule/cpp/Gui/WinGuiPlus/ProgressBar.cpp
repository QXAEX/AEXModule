#include "../../../h/Gui/WinGuiPlus/ProgressBar.h"
static void ProgressBarDraw(WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics) {
    WinGuiPlusProgressBar::ProgressBar* progressbar = reinterpret_cast<WinGuiPlusProgressBar::ProgressBar*>(panelData->params1);
    if (!progressbar) return;
    // 绘制背景
    if (progressbar->style.bkcolor.GetValue() != 0) {
        Gdiplus::SolidBrush bkBrush(progressbar->style.bkcolor);
        Gdiplus::SolidBrush pbBrush(progressbar->style.pbcolor);
        // 先绘制四个角落的小矩形
        int radius = progressbar->style.radius;
        if (progressbar->style.radius) {
            {//bkBrush
                // 绘制左上角
                Gdiplus::SolidBrush radiusColor1(progressbar->style.radiusColor[0]);
                graphics->FillRectangle(&radiusColor1, -1, -1, radius, radius);
                // 绘制右上角
                Gdiplus::SolidBrush radiusColor2(progressbar->style.radiusColor[1]);
                graphics->FillRectangle(&radiusColor2, panelData->width - radius, 0, radius, radius);
                // 绘制右下角
                Gdiplus::SolidBrush radiusColor3(progressbar->style.radiusColor[2]);
                graphics->FillRectangle(&radiusColor3, panelData->width - radius, panelData->height - radius, radius, radius);
                // 绘制左下角
                Gdiplus::SolidBrush radiusColor4(progressbar->style.radiusColor[3]);
                graphics->FillRectangle(&radiusColor4, 0, panelData->height - radius, radius, radius);
                // 然后绘制圆角矩形
                Gdiplus::GraphicsPath bgPath;
                bgPath.AddArc(-1, -1, radius * 2, radius * 2, 180, 90); // 左上角
                bgPath.AddArc(panelData->width - radius * 2, -1, radius * 2, radius * 2, 270, 90); // 右上角
                bgPath.AddArc(panelData->width - radius * 2, panelData->height + 1 - radius * 2, radius * 2, radius * 2, 0, 90); // 右下角
                bgPath.AddArc(-1, panelData->height - radius * 2, radius * 2, radius * 2, 90, 90); // 左下角
                bgPath.CloseFigure();
                graphics->FillPath(&bkBrush, &bgPath);
            }
            {//pbBrush
                Gdiplus::GraphicsPath bgPath;
                bgPath.AddArc(-1, -1, radius * 2, radius * 2, 180, 90); // 左上角
                bgPath.AddArc((int)((panelData->width + 1) * (*progressbar->progress / 100)) - radius * 2, -1, radius * 2, radius * 2, 270, 90); // 右上角
                bgPath.AddArc((int)((panelData->width + 1) * (*progressbar->progress / 100)) - radius * 2, panelData->height + 1 - radius * 2, radius * 2, radius * 2, 0, 90); // 右下角
                bgPath.AddArc(-1, panelData->height - radius * 2, radius * 2, radius * 2, 90, 90); // 左下角
                bgPath.CloseFigure();
                graphics->FillPath(&pbBrush, &bgPath);
            }
        }
        else {
            graphics->FillRectangle(&bkBrush, -1, -1, panelData->width + 1, panelData->height + 1);
            graphics->FillRectangle(&pbBrush, -1, -1, (int)((panelData->width + 1) * (*progressbar->progress / 100)), panelData->height + 1);
        }
        Gdiplus::FontFamily fontFamily(L"微软雅黑");
        Gdiplus::SolidBrush fontBrush(progressbar->style.textcolor);
        Gdiplus::StringFormat format;
        std::string progresss = std::to_string(progressbar->GetProgress());
        std::wstring wprogresss = Text::text_to_wstr(Text::text_right_del(progresss, progresss.size() - Text::text_find(progresss, ".") - 4) + "%");
        // 创建字体（根据容器高度动态计算字号）
        float fontSize = panelData->height * 0.5f; // 高度60%作为字号
        Gdiplus::Font font(&fontFamily, fontSize, Gdiplus::FontStyleRegular);
        // 左侧文字参数设置
        Gdiplus::StringFormat formatLeft;
        formatLeft.SetAlignment(Gdiplus::StringAlignmentNear);    // 水平靠左
        formatLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter); // 垂直居中
        const float margin = 5.0f;
        Gdiplus::RectF rectLeft(0, 0, panelData->width * 1, panelData->height);
        // 创建文本路径
        Gdiplus::GraphicsPath path;
        path.AddString(wprogresss.c_str(), -1, &fontFamily, Gdiplus::FontStyleRegular, fontSize, rectLeft, &formatLeft);
        // 发光参数设置
        const int glowLayers = 1;
        const float maxGlowSize = 3.0f;
        Gdiplus::Color glowColor = progressbar->style.textcolorf;
        // 多层绘制实现发光效果
        for (int i = glowLayers; i > 0; --i)
        {
            float penWidth = progressbar->style.maxGlowSize * (i / (float)glowLayers);
            Gdiplus::Pen glowPen(glowColor, penWidth);
            glowPen.SetLineJoin(Gdiplus::LineJoinRound);
            // 调整颜色透明度
            glowColor = Gdiplus::Color(150 - (i * 20), glowColor.GetRed(), glowColor.GetGreen(), glowColor.GetBlue());
            graphics->DrawPath(&glowPen, &path);
        }
        // 最后绘制原始文本
        graphics->FillPath(&fontBrush, &path);

    }
}
WinGuiPlusProgressBar::ProgressBar::ProgressBar(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, float* progress, bool isLayered, bool visible)
{
    this->create(parent, hInstance, top, left, width, height, progress, isLayered, visible);
}

WinGuiPlusProgressBar::ProgressBar::~ProgressBar()
{
}

void WinGuiPlusProgressBar::ProgressBar::create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, float* progress, bool isLayered, bool visible)
{
    this->panel.callback = ProgressBarDraw;
    this->panel.panelData.componentType = COMPONENT_TYPE::PROGRESSBAR;
    this->panel.panelData.params1 = this;
    this->panel.create(parent, "AEXProgressBar", hInstance, top, left, width, height, isLayered, visible);
    this->progress = progress;
}

void WinGuiPlusProgressBar::ProgressBar::SetVisible(bool visible)
{
    this->panel.SetVisible(visible);
}

bool WinGuiPlusProgressBar::ProgressBar::GetVisible() const
{
    return this->panel.panelData.visible;
}

void WinGuiPlusProgressBar::ProgressBar::SetProgress(float progress)
{
    *this->progress = progress;
    this->panel.Redraw();
}

float WinGuiPlusProgressBar::ProgressBar::GetProgress() const
{
    return *this->progress;
}

PWINGUIPLUS_WINEVENTPROC WinGuiPlusProgressBar::ProgressBar::Event()
{
    return &this->panel.Event()->event;
}
