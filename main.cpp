#include "AEXModule.h"

int main() {
    HotKeys hotkeys;
    hotkeys.listen([&](DWORD key) {
        hotkeys.judgeMod(key, MOD_CONTROL, [&]() {
            hotkeys.judgeMod(key, MOD_SHIFT, [&]() {
                hotkeys.judgeVk(key, 'A', [&]() {
                    std::cout << "Ctrl+Shift+A 被按下" << std::endl;
                    });
                });
            });
        });

    // 保持程序运行
    while (true) {
        Sleep(1000);
    }
    return 0;
}