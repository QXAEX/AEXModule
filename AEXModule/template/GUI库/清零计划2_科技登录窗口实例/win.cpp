#include "win.h"
static Thread thread(200);
Gui::WinGui gui;
static Gui::WININFO win_info;//主窗口信息
static std::wstring username;

void win::win_main() {
    win_info.left = 100;
    win_info.top = 100;
    win_info.width = 320;
    win_info.height = 200;
    win_info.title = L"清零计划2:登录";
    win_info.className = L"AEX_LOGIN";
    win_info.iconId = IDI_ICON1;
    win_info.style.hasDlgFrame = true;
    win_info.style.hasMaximizeBox = false;
    win_info.callback = AEX_Main_LOGIN;
    gui.win_create(win_info);
}
void win::AEX_Main_LOGIN(Gui::PWinGui ui, HWND hwnd, BOOL flag) {
    if (flag) {//true表示初始化
        Gui::WININFO Label1;
        Label1.left = (320 - 80) / 2;
        Label1.top = 0;
        Label1.width = 80;
        Label1.height = 20;
        Label1.title = L"登录";
        ui->Label(hwnd, Label1, 1);
        Gui::WININFO Label2;
        Label2.left = (320 - 200) / 2;
        Label2.top = (200 - 50) / 2;
        Label2.width = 80;
        Label2.height = 20;
        Label2.title = L"账号";
        ui->Label(hwnd, Label2, 1);
        Gui::WININFO Input1;
        Input1.left = (320 - 80) / 2;
        Input1.top = (200 - 55) / 2;
        Input1.width = 120;
        Input1.height = 20;
        ui->InputBox(hwnd, Input1, &username);
        Gui::WININFO Button1;
        Button1.left = (320 - 80) / 2;
        Button1.top = (255) / 2;
        Button1.width = 80;
        Button1.height = 30;
        Button1.title = L"登录";
        Button1.event.onClick = FNC_Main;
        ui->Button(hwnd, Button1);
    }
}
void win::FNC_Main(HWND hwnd) {
    if (username.empty()) {
        MessageBox(hwnd, L"请输入账号", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    else if (username.length() > 16) {
        MessageBox(hwnd, L"账号过长", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    else if (username == L"admin") {
        Gui::WININFO win_info;
        win_info.left = 100;
        win_info.top = 100;
        win_info.width = 320;
        win_info.height = 30;
        win_info.title = L"清零计划2";
        win_info.className = L"AEX";
        win_info.style.hasDlgFrame = true;
        win_info.style.hasMaximizeBox = false;
        win_info.callback = AEX_Main;
        gui.win_create(win_info);
        gui.win_destroy(hwnd);
    }
    else {
        MessageBox(hwnd, L"账号错误", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
}
void win::AEX_Main(Gui::PWinGui ui, HWND hwnd, BOOL flag) {
    if (flag) {//true表示初始化
        Gui::WININFO CheckBox1;
        CheckBox1.left = 5;
        CheckBox1.top = 5;
        CheckBox1.width = 60;
        CheckBox1.height = 20;
        CheckBox1.title = L"锁血";
        ui->CheckBox(hwnd, CheckBox1, &game_value.blood_volume_flag);
        Gui::WININFO CheckBox2;
        CheckBox2.left = 70;
        CheckBox2.top = 5;
        CheckBox2.width = 70;
        CheckBox2.height = 20;
        CheckBox2.title = L"锁护盾";
        ui->CheckBox(hwnd, CheckBox2, &game_value.defense_flag);
        Gui::WININFO CheckBox3;
        CheckBox3.left = 145;
        CheckBox3.top = 5;
        CheckBox3.width = 70;
        CheckBox3.height = 20;
        CheckBox3.title = L"锁子弹";
        ui->CheckBox(hwnd, CheckBox3, &game_value.bullet_flag);
        Gui::WININFO CheckBox4;
        CheckBox4.left = 220;
        CheckBox4.top = 5;
        CheckBox4.width = 80;
        CheckBox4.height = 20;
        CheckBox4.title = L"无限大招";
        ui->CheckBox(hwnd, CheckBox4, &game_value.big_move);
        thread.add_thread(NULL, (Thread_callback)Fnc::startListenProcess, Thread_type::THREAD_TYPE_LOOP, 300);
    }
    else {//false表示窗口销毁
        win_main();
        thread.remove_thread(NULL);
    }
}
