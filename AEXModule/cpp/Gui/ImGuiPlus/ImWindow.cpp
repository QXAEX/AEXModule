#include "../../../h/Gui/ImguiPlus/ImWindow.h"


typedef struct IMGUIWINS {
    DWORD code;
    WinGuiPlus::window* win;
    ID3D11Device* pd3dDevice;          // 每个窗口独立设备
    ID3D11DeviceContext* pd3dContext;  // 独立设备上下文
    IDXGISwapChain* pSwapChain;        // 独立交换链
    ID3D11RenderTargetView* pRenderTarget; // 独立渲染目标
    ImGuiContext* imguiContext;  // 新增ImGui上下文指针
} *PIMGUIWINS;
static std::vector<IMGUIWINS> windows;
static PIMGUIWINS FindIMGUIWINS(DWORD code) {
    for (auto& w : windows) {
        if (w.code == code)
            return &w;
    }
    return nullptr;
}
static PIMGUIWINS FindIMGUIWINS(HWND hwnd) {
    for (auto& w : windows) {
        if (w.win->hwnd == hwnd)
            return &w;
    }
    return nullptr;
}
// 前置声明窗口消息处理函数
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 创建渲染目标视图的函数
static bool CreateRenderTarget(DWORD code) {
    PIMGUIWINS w = FindIMGUIWINS(code);
    if (w == nullptr) return false;
    // 创建渲染目标视图
    ID3D11Texture2D* pBackBuffer;
    if (FAILED(w->pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
        return false;

    HRESULT hr = w->pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &w->pRenderTarget);
    pBackBuffer->Release();
    return SUCCEEDED(hr);
}

// 清理资源的函数
static void CleanupRenderTarget(DWORD code) {
    PIMGUIWINS w = FindIMGUIWINS(code);
    if (w == nullptr) return;
    if (w->pRenderTarget) {
        w->pRenderTarget->Release();
        w->pRenderTarget = nullptr;
    }
}

static void CleanupDeviceD3D(bool drawAll, DWORD code) {
    PIMGUIWINS w = FindIMGUIWINS(code);
    if (w == nullptr) return;
    ImGui::SetCurrentContext(w->imguiContext);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(w->imguiContext);
    CleanupRenderTarget(code);
    if (w->pSwapChain) {
        w->pSwapChain->Release();
        w->pSwapChain = nullptr;
    }
    if (w->pd3dContext) {
        w->pd3dContext->Release();
        w->pd3dContext = nullptr;
    }
    if (w->pd3dDevice) {
        w->pd3dDevice->Release();
        w->pd3dDevice = nullptr;
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

static void Init(bool drawAll,DWORD code, HWND hwnd, WinGuiPlus::window::INFO info, HDC hdc, WinGuiPlus::window& win, std::string fontPath) {
    // 创建DX11设备
    PIMGUIWINS w = FindIMGUIWINS(code);
    if (w == nullptr) return;
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &w->pd3dDevice, nullptr, &w->pd3dContext);
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
            sd.Width = info.width - (info.noBorder ? 0 : 16);
            sd.Height = info.height - (info.noBorder ? 0 : 39);
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 2;
            sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 使用翻转丢弃模式
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

            IDXGISwapChain1* pSwapChain1 = nullptr;
            if (SUCCEEDED(pFactory2->CreateSwapChainForHwnd(w->pd3dDevice, hwnd, &sd, nullptr, nullptr, &pSwapChain1)))
            {
                pSwapChain1->QueryInterface(IID_PPV_ARGS(&w->pSwapChain));
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
            pFactory->CreateSwapChain(w->pd3dDevice, &sd, &w->pSwapChain);
        }
        pFactory->Release();
    }

    if (!w->pSwapChain || !CreateRenderTarget(code)) {
        MessageBox(hwnd, L"渲染目标创建失败", L"错误", MB_ICONERROR);
        return;
    }

    // 初始化ImGui
    IMGUI_CHECKVERSION();
    w->imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(w->imguiContext); // 设置当前上下文
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(w->pd3dDevice, w->pd3dContext);

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

ImGuiPlusWindow::window::~window() {
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i].code == this->code) {
            windows[i].win->close();
            delete windows[i].win;
            windows.erase(windows.begin() + i);
        }
    }
}

HWND ImGuiPlusWindow::window::create(bool drawAll, bool useFrame, HWND parent, LPCWSTR title, LPCWSTR className, WinGuiPlus::window::INFO winInfo, WinGuiPlus::window::callBack callBackFunc) {
    this->code = std::stoi(Text::text_random_num(8));

    windows.push_back({ this->code, new WinGuiPlus::window() });
    PIMGUIWINS w = FindIMGUIWINS(this->code);
    WinGuiPlus::window* win = this->getWin();
    HWND hwnd = win->create(parent, title, className, winInfo, [&, winInfo, useFrame, title, win, callBackFunc, drawAll](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status) {
            PIMGUIWINS w = FindIMGUIWINS(this->code);
            if (w == nullptr) {
                if (WINGUIPLUS_STATUS::DRAW == status) {
                    win->redraw();
                }
                return;
            }
            int newWidth = max(1, winInfo.width - 16);
            int newHeight = max(1, winInfo.height - 39);
            HDC hdc; 
            PAINTSTRUCT ps;
            switch (status) {
            case WINGUIPLUS_STATUS::CREATE:
                 hdc = BeginPaint(hwnd, &ps);
                Init(drawAll, this->code, hwnd, winInfo, hdc, *win, "c:\\Windows\\Fonts\\msyh.ttc");
                EndPaint(hwnd, &ps);
                if (callBackFunc) callBackFunc(hwnd, hInstance, status);
                break;
            case WINGUIPLUS_STATUS::SIZE_CHANGED:
                CleanupRenderTarget(this->code);
                w->pSwapChain->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget(this->code);
                break;
            case WINGUIPLUS_STATUS::DRAW: {
                if (!w->pd3dContext || !w->pRenderTarget || !w->pSwapChain) {
                    win->redraw();
                    break;
                }
                if (IsIconic(hwnd)) {
                    win->redraw();
                    break;
                }
                ImGui::SetCurrentContext(w->imguiContext);
                const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                w->pd3dContext->ClearRenderTargetView(w->pRenderTarget, clear_color);
                ImGui_ImplDX11_NewFrame();
                ImGui_ImplWin32_NewFrame();
                w->pd3dContext->OMSetRenderTargets(1, &w->pRenderTarget, nullptr);
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
                                if (callBackFunc) callBackFunc(hwnd, hInstance, status);
                            }
                            ImGui::EndChild();
                        }
                        ImGui::EndChild();
                    }
                    else if (callBackFunc) {
                        callBackFunc(hwnd, hInstance, status);
                    }
                    ImGui::PopStyleVar(3);
                    ImGui::End();
                }
                else {
                    if (callBackFunc) callBackFunc(hwnd, hInstance, status);
                }
                ImGui::Render();
                w->pd3dContext->OMSetRenderTargets(1, &w->pRenderTarget, nullptr);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                w->pSwapChain->Present(0, 0); // 允许撕裂或设置为0避免VSync
                win->redraw();
                break;
            }
            case WINGUIPLUS_STATUS::DESTROY:
                CleanupDeviceD3D(drawAll, this->code);
                break;
            }
        });
    return hwnd;
}

WinGuiPlus::window* ImGuiPlusWindow::window::getWin() const {
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i].code == this->code) {
            return windows[i].win;
        }
    }
    return nullptr;
}

int __stdcall ImGuiPlusWindow::run(WinGuiPlus::customMsg msgLoop) {
    return WinGuiPlus::run([msgLoop](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (auto w = FindIMGUIWINS(hwnd)) {
            ImGuiContext* backup_context = ImGui::GetCurrentContext();
            ImGui::SetCurrentContext(w->imguiContext);
            ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
            ImGui::SetCurrentContext(backup_context);
            if (msg == WM_SIZE && wParam != SIZE_MINIMIZED) {
                if (w->pSwapChain && w->pd3dDevice) {
                    CleanupRenderTarget(w->code);
                    w->pSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                    CreateRenderTarget(w->code);
                }
            }
        }
        if (msgLoop) msgLoop(hwnd, msg, wParam, lParam);
        return 0;
        });
}