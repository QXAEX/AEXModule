#include "../../h/Gui/ImguiPlus.h"
#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_win32.h"
#include "../../imgui/backends/imgui_impl_dx11.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
struct IMGUIWINS {
    DWORD code;
    WinGuiPlus::window* win;
};
static std::vector<IMGUIWINS> windows;
// 全局DX11资源声明
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// 前置声明窗口消息处理函数
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 创建渲染目标视图的函数
static bool CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (!pBackBuffer) return false;

    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
    return true;
}

// 清理资源的函数
static void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

static void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}

static void Init(HWND hwnd, WinGuiPlus::window::INFO info, HDC hdc, WinGuiPlus::window& win)
{
    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);

    // 创建DX11设备
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = info.width;
    sd.BufferDesc.Height = info.height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const D3D_FEATURE_LEVEL featureLevelArray[] = { D3D_FEATURE_LEVEL_11_0 };
    if (D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevelArray,
        1,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        nullptr,
        &g_pd3dDeviceContext) != S_OK)
    {
        MessageBox(hwnd, L"DX11初始化失败", L"错误", MB_ICONERROR);
        return;
    }

    if (!CreateRenderTarget()) {
        MessageBox(hwnd, L"渲染目标创建失败", L"错误", MB_ICONERROR);
        return;
    }

    // 初始化ImGui DX11后端
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    // 如果框架需要显式设置像素格式
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32, // color depth
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0,
        24, // depth buffer
        8,  // stencil buffer
        0,  // auxiliary buffers
        PFD_MAIN_PLANE,
        0, 0, 0, 0
    };
    int format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, format, &pfd);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    io.ConfigFlags |= ImGuiConfigFlags_None; // 禁用全局默认行为
    win.redraw();
}
ImguiPlus::window::~window() {
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i].code == this->code) {
            windows[i].win->close();
            delete windows[i].win;
            windows.erase(windows.begin() + i);
        }
    }
}
HWND ImguiPlus::window::create(HWND parent, LPCWSTR title, LPCWSTR className, WinGuiPlus::window::INFO winInfo, WinGuiPlus::window::callBack callBackFunc) {
    this->code = std::stoi(Text::text_random_num(8));
    windows.push_back({ this->code,new WinGuiPlus::window() });
    WinGuiPlus::window* win = this->getWin();
    HWND hwnd = win->create(parent, title, className, winInfo, [&, winInfo, title, win, callBackFunc](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
        switch (status) {
        case WINGUIPLUS_STATUS::CREATE:
            Init(hwnd, winInfo, hdc, *win);
            break;
        case WINGUIPLUS_STATUS::SIZE_CHANGED:
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, winInfo.width - 16, winInfo.height - 39, DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
            break;
        case WINGUIPLUS_STATUS::DRAW: {
            // 开始新帧前清除目标
            const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
            // ImGui绘制流程
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::Begin(Text::text_to_str(title).c_str(), nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoSavedSettings);
            if (callBackFunc) callBackFunc(hwnd, hInstance, status, hdc);
            ImGui::End();
            ImGui::Render();
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            g_pSwapChain->Present(1, 0);
            win->redraw();
            break;
            }
        case WINGUIPLUS_STATUS::DESTROY:
            CleanupDeviceD3D();
            break;
        }
        });
    return hwnd;
}

WinGuiPlus::window* ImguiPlus::window::getWin() const
{
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i].code == this->code) {
            return windows[i].win;
        }
    }
    return nullptr;
}

int __stdcall ImguiPlus::run(WinGuiPlus::customMsg msgLoop)
{
    return WinGuiPlus::run([&](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
        if(msgLoop) msgLoop(hwnd, msg, wParam, lParam);
        });
}
