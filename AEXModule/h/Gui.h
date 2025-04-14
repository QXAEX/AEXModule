#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "Text.h"
#include "Byteset.h"
#include "./Gui/WinGui.h"
#include "./Gui/WinGuiPlus.h"
#include "./Gui/ImguiPlus.h"

namespace Gui {
    //win原生gui(停止维护)
    namespace WinGui =  WinGui;
    //win原生gui+
    namespace WinGuiPlus =  WinGuiPlus;
    //imgui+(基于WinGuiPlus与Imgui结合)
    namespace ImguiPlus =  ImguiPlus;
}
