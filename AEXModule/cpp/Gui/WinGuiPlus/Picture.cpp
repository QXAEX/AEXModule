#include "../../../h/Gui/WinGuiPlus/Picture.h"
#include <objidl.h>
#pragma comment(lib, "ole32.lib")
static void LoadImageFromResource(WinGuiPlusPicture::Picture* picture, Gdiplus::Graphics* graphics, WinGuiPlusPanel::Panel::PPanelData panelData) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (!picture->imagePath.empty()) {
        Gdiplus::Image image(Text::text_to_wstr(picture->imagePath).c_str());
        if (image.GetLastStatus() == Gdiplus::Ok) {
            graphics->DrawImage(&image, 0, 0, panelData->width, panelData->height);
        }
        else {
            printf("Image load failed: %d\n", image.GetLastStatus());
        }
    } else if (picture->Id) {
        HRSRC hResource = FindResource(panelData->hInstance, MAKEINTRESOURCE(picture->Id), Text::text_to_wstr(picture->suffix).c_str());
        if (hResource) {
            HGLOBAL hMemory = LoadResource(panelData->hInstance, hResource);
            if (hMemory) {
                DWORD size = SizeofResource(panelData->hInstance, hResource);
                void* pResourceData = LockResource(hMemory);

                // ´´½¨ÄÚ´æ¾ä±ú
                HGLOBAL hGlobal = GlobalAlloc(GHND, size);
                if (hGlobal) {
                    LPVOID pGlobal = GlobalLock(hGlobal);
                    if (pGlobal) {
                        memcpy(pGlobal, pResourceData, size);
                        GlobalUnlock(hGlobal);

                        IStream* pStream = NULL;
                        HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
                        if (SUCCEEDED(hr)) {
                            Gdiplus::Image image(pStream);
                            if (image.GetLastStatus() == Gdiplus::Ok) {
                                graphics->DrawImage(&image, 0, 0, panelData->width, panelData->height);
                            }
                            else {
                                printf("Image load failed: %d\n", image.GetLastStatus());
                            }
                            pStream->Release();
                        }
                        GlobalFree(hGlobal);
                    }
                }
            }
        }
    }
    CoUninitialize();
}
static void PictureDraw(WinGuiPlusPanel::Panel::PPanelData panelData, Gdiplus::Graphics* graphics) {
    WinGuiPlusPicture::Picture* picture = reinterpret_cast<WinGuiPlusPicture::Picture*>(panelData->params1);
    if (!picture) return;
    Gdiplus::SolidBrush brush(picture->style.bkcolor);
    graphics->FillRectangle(&brush, 0, 0, panelData->width, panelData->height);
    LoadImageFromResource(picture, graphics, panelData);
}
WinGuiPlusPicture::Picture::~Picture()
{
}
void WinGuiPlusPicture::Picture::create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, DWORD id, std::string suffix, bool isLayered, bool visible)
{
    this->panel.callback = PictureDraw;
    this->panel.panelData.componentType = COMPONENT_TYPE::LABEL;
    this->panel.panelData.params1 = this;
    this->Id = id;
    this->panel.create(parent, "AEXLabel", hInstance, top, left, width, height, isLayered, visible);

}
void WinGuiPlusPicture::Picture::create(HWND parent, HINSTANCE hInstance, int top, int left, int width, int height, std::string imagePath, bool isLayered, bool visible)
{
    this->panel.callback = PictureDraw;
    this->panel.panelData.componentType = COMPONENT_TYPE::LABEL;
    this->panel.panelData.params1 = this;
    this->imagePath = imagePath;
    this->panel.create(parent, "AEXLabel", hInstance, top, left, width, height, isLayered, visible);
}
void WinGuiPlusPicture::Picture::SetVisible(bool visible)
{
    this->panel.SetVisible(visible);
}
bool WinGuiPlusPicture::Picture::GetVisible() const
{
    return this->panel.panelData.visible;
}
void WinGuiPlusPicture::Picture::SetImage(DWORD Id, std::string suffix)
{
    this->Id = Id;
    this->suffix = suffix;
    InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
}
void WinGuiPlusPicture::Picture::SetImage(std::string imagePath)
{
    this->imagePath = imagePath;
    InvalidateRect(this->panel.panelData.hwnd, NULL, TRUE);
}
PWINGUIPLUS_WINEVENTPROC WinGuiPlusPicture::Picture::Event()
{
    return &this->panel.Event()->event;
}
