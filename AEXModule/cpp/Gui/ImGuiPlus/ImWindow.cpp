#include "../../../h/Gui/ImguiPlus/ImWindow.h"
#include <dxgi.h>
#include <d3d11.h>
#include <VersionHelpers.h>
#include <string>
#include <vector>

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
static bool g_bTearingSupported = false;

// 前置声明窗口消息处理函数
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 创建渲染目标视图的函数
static bool CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    if (FAILED(g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) 
        return false;

    HRESULT hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
    return SUCCEEDED(hr);
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

// 现代风格配置（支持亮色/暗色主题切换）
static void SetupImGuiStyle(bool bStyleDark = true, float alpha = 0.9f) {
    ImGuiStyle& style = ImGui::GetStyle();
    style.AntiAliasedLines = true; // 设置抗锯齿
    style.AntiAliasedFill = true;  // 设置抗锯齿
    style.CurveTessellationTol = 1.25f; // 设置曲线细分误差

    // 通用样式参数
    style.Alpha = 1.0f;
    style.FrameRounding = 4.0f;       // 控件圆角
    style.ScrollbarRounding = 4.0f;   // 滚动条圆角
    style.GrabRounding = 4.0f;        // 拖动条圆角
    style.TabRounding = 4.0f;         // Tab页圆角
    style.WindowBorderSize = 1.0f;    // 窗口边框
    style.ChildBorderSize = 1.0f;     // 子窗口边框
    style.PopupBorderSize = 1.0f;     // 弹出窗口边框
    style.FrameBorderSize = 0.0f;     // 控件边框
    style.ScrollbarSize = 16.0f;      // 滚动条宽度
    style.ItemSpacing = ImVec2(8, 6); // 控件间距
    style.WindowMenuButtonPosition = ImGuiDir_Right; // 菜单按钮位置

    // 完整颜色配置（支持透明度）
    ImVec4* colors = style.Colors;
    if (bStyleDark) {
        // 暗色主题
        colors[ImGuiCol_Text]               = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled]       = ImVec4(0.50f, 0.51f, 0.53f, 1.00f);
        colors[ImGuiCol_WindowBg]           = ImVec4(0.12f, 0.14f, 0.17f, alpha);
        colors[ImGuiCol_ChildBg]            = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_PopupBg]            = ImVec4(0.12f, 0.14f, 0.17f, 0.99f);
        // 交互控件
        colors[ImGuiCol_Border]             = ImVec4(0.20f, 0.22f, 0.27f, 0.50f);
        colors[ImGuiCol_FrameBg]            = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]     = ImVec4(0.25f, 0.27f, 0.33f, 1.00f);
        colors[ImGuiCol_FrameBgActive]      = ImVec4(0.30f, 0.32f, 0.38f, 1.00f);
        // 标题栏
        colors[ImGuiCol_TitleBg]            = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBgActive]      = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.10f, 0.11f, 0.14f, 0.75f);
        // 按钮
        colors[ImGuiCol_Button]             = ImVec4(0.25f, 0.55f, 0.95f, 1.00f);
        colors[ImGuiCol_ButtonHovered]      = ImVec4(0.30f, 0.60f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive]       = ImVec4(0.20f, 0.50f, 0.90f, 1.00f);
        // 高级组件
        colors[ImGuiCol_Header]             = ImVec4(0.30f, 0.32f, 0.38f, 1.00f);
        colors[ImGuiCol_HeaderHovered]      = ImVec4(0.35f, 0.37f, 0.43f, 1.00f);
        colors[ImGuiCol_HeaderActive]       = ImVec4(0.25f, 0.27f, 0.33f, 1.00f);
        colors[ImGuiCol_Separator]          = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_CheckMark]          = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_SliderGrab]         = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]   = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        colors[ImGuiCol_ResizeGrip]         = ImVec4(0.40f, 0.40f, 0.40f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]  = ImVec4(0.60f, 0.60f, 0.60f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]   = ImVec4(0.90f, 0.90f, 0.90f, 0.95f);
        // 导航组件
        colors[ImGuiCol_Tab]                = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_TabHovered]         = ImVec4(0.25f, 0.55f, 0.95f, 0.80f);
        colors[ImGuiCol_TabActive]          = ImVec4(0.20f, 0.50f, 0.90f, 1.00f);
        colors[ImGuiCol_TabUnfocused]       = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.50f, 0.90f, 1.00f);
        // 滚动条
        colors[ImGuiCol_ScrollbarBg]        = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]      = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        // 菜单栏
        colors[ImGuiCol_MenuBarBg]          = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
    }
}

static void Init(bool drawAll, HWND hwnd, WinGuiPlus::window::INFO info, HDC hdc, WinGuiPlus::window& win, std::string fontPath) {
    // 创建DX11设备
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &g_pd3dDevice,
        nullptr,
        &g_pd3dDeviceContext);

    if (FAILED(hr)) {
        MessageBox(hwnd, L"DX11初始化失败", L"错误", MB_ICONERROR);
        return;
    }

    // 创建交换链
    IDXGIFactory* pFactory = nullptr;
    if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&pFactory)))) {
        // 检测是否支持高级功能
        IDXGIFactory2* pFactory2 = nullptr;
        if (drawAll && SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&pFactory2)))) {
            DXGI_SWAP_CHAIN_DESC1 sd = { 0 };
            sd.Width = info.width;
            sd.Height = info.height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 2;
            sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 使用翻转丢弃模式
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

            IDXGISwapChain1* pSwapChain1 = nullptr;
            if (SUCCEEDED(pFactory2->CreateSwapChainForHwnd(
                g_pd3dDevice,
                hwnd,
                &sd,
                nullptr,
                nullptr,
                &pSwapChain1)))
            {
                pSwapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain));
                pSwapChain1->Release();
            }
            pFactory2->Release();
        }
        else {
            // 回退到传统创建方式
            DXGI_SWAP_CHAIN_DESC sd = { 0 };
            sd.BufferCount = 2;
            sd.BufferDesc.Width = info.width;
            sd.BufferDesc.Height = info.height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 0;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hwnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            // 使用传统的CreateSwapChain方法
            pFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
        }
        pFactory->Release();
    }

    if (!g_pSwapChain || !CreateRenderTarget()) {
        MessageBox(hwnd, L"渲染目标创建失败", L"错误", MB_ICONERROR);
        return;
    }

    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // 优化配置
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    // 加载字体
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->Build();

    SetupImGuiStyle();
    win.redraw();
}

ImguiPlusWindow::window::~window() {
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i].code == this->code) {
            windows[i].win->close();
            delete windows[i].win;
            windows.erase(windows.begin() + i);
        }
    }
}

HWND ImguiPlusWindow::window::create(bool drawAll, bool useFrame, HWND parent, LPCWSTR title, LPCWSTR className, WinGuiPlus::window::INFO winInfo, WinGuiPlus::window::callBack callBackFunc) {
    this->code = std::stoi(Text::text_random_num(8));
    windows.push_back({ this->code, new WinGuiPlus::window() });
    WinGuiPlus::window* win = this->getWin();
    HWND hwnd = win->create(parent, title, className, winInfo, 
        [&, winInfo, title, win, callBackFunc, drawAll](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
            switch (status) {
            case WINGUIPLUS_STATUS::CREATE:
                Init(drawAll, hwnd, winInfo, hdc, *win, "c:\\Windows\\Fonts\\msyh.ttc");
                if (callBackFunc) callBackFunc(hwnd, hInstance, status, hdc);
                break;
            case WINGUIPLUS_STATUS::SIZE_CHANGED:
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, winInfo.width - 16, winInfo.height - 39, DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
                break;
            case WINGUIPLUS_STATUS::DRAW: {
                const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
                ImGui_ImplDX11_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();
                if (drawAll) {
                    win->setDragable();
                    const ImGuiViewport* viewport = ImGui::GetMainViewport();
                    ImGui::SetNextWindowPos(viewport->WorkPos);
                    ImGui::SetNextWindowSize(viewport->WorkSize);
                    ImGui::Begin("-", nullptr,
                        ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_NoInputs |
                        ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoSavedSettings);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 3.0f);
                    if (useFrame) {
                        ImGui::BeginChild("AEXGUI", ImVec2(0, 0), true);
                        {
                            if (winInfo.noBorder) {
                                ImGui::Text("%s", UTF8(Text::text_to_str(title).c_str()).c_str());
                                ImGui::SameLine(ImGui::GetWindowWidth() - 95);
                                if (ImGui::Button(u8"—", ImVec2(40, 0))) {
                                    win->minimize();
                                }
                                if (ImGui::IsItemHovered()) win->cancelDragable();
                                ImGui::SameLine(ImGui::GetWindowWidth() - 50);
                                if (ImGui::Button(u8"X", ImVec2(40, 0))) {
                                    win->close();
                                    return;
                                }
                                if (ImGui::IsItemHovered()) win->cancelDragable();
                            }
                            ImGui::Separator();
                            ImGui::BeginChild("CONTENT", ImVec2(0, 0), true);
                            {
                                if (callBackFunc) callBackFunc(hwnd, hInstance, status, hdc);
                            }
                            ImGui::EndChild();
                        }
                        ImGui::EndChild();
                    }
                    else if (callBackFunc) {
                        callBackFunc(hwnd, hInstance, status, hdc); 
                    }
                    ImGui::PopStyleVar(3);
                    ImGui::End();
                } else {
                    if (callBackFunc) callBackFunc(hwnd, hInstance, status, hdc);
                }
                ImGui::Render();
                g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                g_pSwapChain->Present(0, 0); // 允许撕裂或设置为0避免VSync
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

WinGuiPlus::window* ImguiPlusWindow::window::getWin() const {
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i].code == this->code) {
            return windows[i].win;
        }
    }
    return nullptr;
}

int __stdcall ImguiPlusWindow::run(WinGuiPlus::customMsg msgLoop) {
    return WinGuiPlus::run([&](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
        if (msgLoop) msgLoop(hwnd, msg, wParam, lParam);
    });
}
