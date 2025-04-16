#pragma once
#include "../WinGuiPlus.h"
#include "../../../imgui/imgui.h"
#include "../../../imgui/backends/imgui_impl_win32.h"
#include "../../../imgui/backends/imgui_impl_dx11.h"
#include "../../../imgui/imgui_internal.h"
#include <functional>
#include <d3d11.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <dxgi1_5.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

static std::string UTF8(std::string str) {
    return Text::text_unicode_to_utf8(Text::text_ascii_to_unicode(str));
}
