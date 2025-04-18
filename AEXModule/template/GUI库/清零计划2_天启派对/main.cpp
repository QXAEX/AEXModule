#include "../../../../AEXModule.h"
#include <iostream>
struct FUNC_ADDR
{
    PVOID hp_addr; //生命地址
    PVOID hp_max_addr; //生命上限地址
    PVOID defense_addr; //防御地址
    PVOID defense_max_addr; //防御上限地址
    PVOID speed_addr; //移速地址
    PVOID attack_speed_addr;//攻速地址
    PVOID skill_addr;//大招地址
    PVOID bullet;//子弹地址
    PVOID bullet_max; //子弹上限地址
    PVOID exp_addr;//经验地址
    struct {
        PVOID gold_addr; //金币地址
    } unlimited;//无限地狱
    struct {
        DOUBLE hp;//生命值
        DOUBLE hp_max;//生命上限值
        DOUBLE defense;//防御值
        DOUBLE defense_max;//防御上限值
        DOUBLE speed;//移速
        DOUBLE attack_speed;//攻速
        DOUBLE exp;//经验
        struct {
            int gold;//金币
        } unlimited;//无限地狱
    } info;//个人信息
    struct {
        bool hp;//生命变更
        bool defense;//防御变更
        bool speed;//移速变更
        bool attack_speed;//攻速变更
        bool skill;//大招无限
        bool bullet;//子弹变更
        bool exp;//经验变更
        struct {
            bool gold;//金币变更
        } unlimited;//无限地狱
    } flag;
    struct {
        float hp = 200.0f;//生命值
        float hp_max = 200.0f;//生命上限值
        float defense = 100.0f;//防御值
        float defense_max = 100.0f;//防御上限值
        float speed = 800.0f;//移速
        float attack_speed = 1.0f;//攻速
        float bullet = 100.0f;//子弹
        float bullet_max = 100.0f;//子弹上限
        float exp = 1.0f;//经验
        struct {
            float gold = 1000.0f;//金币
        } unlimited;//无限地狱
    } value;
};
static FUNC_ADDR func;
static Thread thread;
static Memory::R3 r3;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    static ImGuiPlus::window win;
    WinGuiPlus::window::INFO info;
    info.width = 500;
    info.height = 800;
    info.noBorder = false;
    thread.add(0, [&](PThread pthread) {
        PVOID TheLastSurvivor = r3.GetModuleBase("TheLastSurvivor-Win64-Shipping.exe");
        if (TheLastSurvivor) {
            //防御系列
            {
                Byteset data = r3.pointerChain(TheLastSurvivor, { 0x062E11F8,0x10,0x300,0x140,0x318, 0x1E8, 0x8 });
                func.defense_addr = (PVOID)(*(__int64*)data.data() + 0xA10);//防御地址
                func.defense_max_addr = (PVOID)(*(__int64*)data.data() + 0xA08);//防御上限地址
                r3.Read<double>((PVOID)func.defense_addr, func.info.defense);
                r3.Read<double>((PVOID)func.defense_max_addr, func.info.defense_max);
                if (func.flag.defense) {
                    r3.Write(func.defense_addr, (double)func.value.defense);
                    r3.Write(func.defense_max_addr, (double)func.value.defense_max);
                }
            }
            //生命系列
            {
                Byteset data = r3.pointerChain(TheLastSurvivor, { 0x63698C0,0x30,0x328,0x388,0x9B8,0x2F0,0x20 });
                func.hp_addr = (PVOID)(*(__int64*)data.data() + 0x848);//血量地址
                func.hp_max_addr = (PVOID)(*(__int64*)data.data() + 0x828);//血量上限地址
                func.speed_addr = (PVOID)(*(__int64*)data.data() + 0x850);//移速地址
                func.attack_speed_addr = (PVOID)(*(__int64*)data.data() + 0x860);//攻速地址
                r3.Read<double>((PVOID)func.hp_addr, func.info.hp);
                r3.Read<double>((PVOID)func.hp_max_addr, func.info.hp_max);
                if (func.flag.hp) {
                    r3.Write(func.hp_addr, (double)func.value.hp);
                    r3.Write(func.hp_max_addr, (double)func.value.hp_max);
                }
                if (func.flag.speed) {
                    r3.Write(func.speed_addr, (double)func.value.speed);
                }
                if (func.flag.attack_speed) {
                    r3.Write(func.attack_speed_addr, (double)func.value.attack_speed);
                }
            }
            //大招系列
            {
                Byteset data = r3.pointerChain(TheLastSurvivor, { 0x6233E18,0xA90, 0xA0, 0x3B0, 0x20, 0x3A0, 0xE0 });
                func.skill_addr = (PVOID)(*(__int64*)data.data() + 0x940);//大招地址
                if (func.flag.skill) {
                    r3.Write(func.skill_addr, (double)1000);
                }
            }
            //子弹系列
            {
                Byteset data = r3.pointerChain(TheLastSurvivor, { 0x63698C0, 0x30, 0x328, 0x388, 0x9B8, 0x580, 0x40 });
                func.bullet = (PVOID)(*(__int64*)data.data() + 0x8C8);//子弹地址
                func.bullet_max = (PVOID)(*(__int64*)data.data() + 0x8D0);//子弹上限地址
                if (func.flag.bullet) {
                    r3.Write(func.bullet, (int)func.value.bullet);
                    r3.Write(func.bullet_max, (int)func.value.bullet_max);
                }
            }
            //经验系列
            {
                Byteset data = r3.pointerChain(TheLastSurvivor, { 0x60D1EA8, 0xA0, 0x710, 0xA8, 0x1B8, 0x10, 0x128 });
                func.exp_addr = (PVOID)(*(__int64*)data.data() + 0x478);//经验地址
                r3.Read<double>((PVOID)func.exp_addr, func.info.exp);
            }
            //无限地狱系列
            {
                Byteset data = r3.pointerChain(TheLastSurvivor, { 0x62301A8, 0x28, 0x6F0, 0x10, 0x9C8, 0x290, 0x20 });
                func.unlimited.gold_addr = (PVOID)(*(__int64*)data.data() + 0xA88);//金币地址
                r3.Read<int>((PVOID)func.unlimited.gold_addr, func.info.unlimited.gold);
            }
        }
        }, THREAD_TYPE::THREAD_TYPE_LOOP, 800);
    thread.start(0);
    win.create(true, true, NULL, L"清零计划2_天启派对", L"TIANQIPAIDUI", info, [](HWND hwnd, HINSTANCE hInstance, WINGUIPLUS_STATUS status, HDC hdc) {
        if (WINGUIPLUS_STATUS::DRAW == status) {
            ImGuiPlus::Button(&win, u8"锁定进程", std::string(u8"当前PID:  ").append(std::to_string(r3.PID)).c_str(), ImVec2(100, 30), ImGuiPlus::BUTTON_STYLE(), [hwnd](WINGUIPLUS_STATUS status) {
                if (WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN == status) {
                    r3.Open(Process::ProcessGetID("TheLastSurvivor-Win64-Shipping.exe"));
                }
                });
            {
                ImGui::BeginChild("INFO", ImVec2(-1, 60), true);
                ImGui::Text(std::string(u8"血量:  ").append(std::to_string(func.info.hp)).c_str());
                ImGui::SameLine(ImGui::GetWindowWidth() / 2);
                ImGui::Text(std::string(u8"血量上限:  ").append(std::to_string(func.info.hp_max)).c_str());
                ImGui::Text(std::string(u8"防御:  ").append(std::to_string(func.info.defense)).c_str());
                ImGui::SameLine(ImGui::GetWindowWidth() / 2);
                ImGui::Text(std::string(u8"防御上限:  ").append(std::to_string(func.info.defense_max)).c_str());
                ImGui::EndChild();
            }
            {
                ImGui::BeginChild("锁定功能", ImVec2(-1, 45), true);
                ImGuiPlus::Checkbox(&win, u8"无限大招", u8"如果开启过程中游戏意外崩溃，请在您获取到大招卡后再开启，没获取到大招卡前请保持关闭(不会直接修改静态内存)", &func.flag.skill);
                //ImGui::SameLine(ImGui::GetWindowWidth() / 2);
                ImGui::EndChild();
            }
            {
                ImGui::BeginChild("修改功能", ImVec2(-1, 0), true);
                {
                    ImGui::BeginChild("血量数据", ImVec2(-1, 75), true);
                    ImGuiPlus::Checkbox(&win, u8"变更血量", u8"不会直接修改静态内存", &func.flag.hp);
                    ImGui::SameLine(100);
                    ImGuiPlus::SliderFloat(&win, &func.value.hp, ImVec2(1, 9999), u8"血量值: ");
                    ImGuiPlus::SliderFloat(&win, &func.value.hp_max, ImVec2(1, 9999), u8"血量上限值: ");
                    ImGui::EndChild();
                }
                {
                    ImGui::BeginChild("防御数据", ImVec2(-1, 75), true);
                    ImGuiPlus::Checkbox(&win, u8"变更防御", u8"不会直接修改静态内存", &func.flag.defense);
                    ImGui::SameLine(100);
                    ImGuiPlus::SliderFloat(&win, &func.value.defense, ImVec2(1, 9999), u8"防御值: ");
                    ImGuiPlus::SliderFloat(&win, &func.value.defense_max, ImVec2(1, 9999), u8"防御上限值: ");
                    ImGui::EndChild();
                }
                {
                    ImGui::BeginChild("子弹数据", ImVec2(-1, 75), true);
                    ImGuiPlus::Checkbox(&win, u8"变更子弹", u8"不会直接修改静态内存", &func.flag.bullet);
                    ImGui::SameLine(100);
                    ImGuiPlus::SliderFloat(&win, &func.value.bullet, ImVec2(1, 9999), u8"子弹值: ");
                    ImGuiPlus::SliderFloat(&win, &func.value.bullet_max, ImVec2(1, 9999), u8"子弹上限值: ");
                    ImGui::EndChild();
                }
                {
                    ImGui::BeginChild("经验数据", ImVec2(-1, 50), true);
                    ImGuiPlus::Button(&win, u8"增加经验", u8"不会直接修改静态内存", ImVec2(80, 30), ImGuiPlus::BUTTON_STYLE(), [hwnd](WINGUIPLUS_STATUS status) {
                        switch (status)
                        {
                        case WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN:
                            r3.Read<double>((PVOID)func.exp_addr, func.info.exp);
                            r3.Write(func.exp_addr, (DOUBLE)func.value.exp + func.info.exp);
                            break;
                        }
                        });
                    ImGui::SameLine(100);
                    ImGuiPlus::SliderFloat(&win, &func.value.exp, ImVec2(1, 10000), u8"欲增加值: ");
                    ImGui::EndChild();
                }
                {
                    ImGui::BeginChild("无限地狱数据", ImVec2(-1, 100), true);
                    ImGui::Text(u8"无限地狱功能(仅在此模式下的功能)");
                    ImGuiPlus::Button(&win, u8"增加金币", u8"不会直接修改静态内存", ImVec2(80, 30), ImGuiPlus::BUTTON_STYLE(), [hwnd](WINGUIPLUS_STATUS status) {
                        switch (status)
                        {
                        case WINGUIPLUS_STATUS::MOUSE_LEFT_DOWN:
                            r3.Read<int>((PVOID)func.unlimited.gold_addr, func.info.unlimited.gold);
                            r3.Write(func.unlimited.gold_addr, (int)func.value.unlimited.gold + func.info.unlimited.gold);
                            break;
                        }
                        });
                    ImGui::SameLine(100);
                    ImGuiPlus::SliderFloat(&win, &func.value.unlimited.gold, ImVec2(1, 10000), u8"欲增加值: ");
                    ImGui::EndChild();
                }
                {
                    ImGuiPlus::Checkbox(&win, u8"变更速度", u8"不会直接修改静态内存", &func.flag.speed);
                    ImGui::SameLine(100);
                    ImGuiPlus::SliderFloat(&win, &func.value.speed, ImVec2(500, 9999), u8"速度值: ");
                    ImGuiPlus::Checkbox(&win, u8"变更攻速", u8"不会直接修改静态内存", &func.flag.attack_speed);
                    ImGui::SameLine(100);
                    ImGuiPlus::SliderFloat(&win, &func.value.attack_speed, ImVec2(1, 9999999), u8"攻速值: ");
                }
                ImGui::EndChild();
            }
        }
        });
    ImGuiPlus::run();
}