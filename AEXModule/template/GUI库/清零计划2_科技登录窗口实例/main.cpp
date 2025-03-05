#include <iostream>
#include <Windows.h>
#include "win.h"

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PWSTR lpCmdLine,int nCmdShow) {
    win::win_main();
    gui.Main();
    return 0;
}
