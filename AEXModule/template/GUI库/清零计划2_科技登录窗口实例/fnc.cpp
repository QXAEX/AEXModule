#include "fnc.h"

GAME_VALUE game_value;
Memory::R3 r3;
static size_t TheLastSurvivor_Win64_Shipping = NULL;
HWND hwnd = nullptr;
// �޸�: ����ֵ
static void write_defense() {
	if (TheLastSurvivor_Win64_Shipping) {
		if (game_value.defense_flag) {
			size_t addr = *(size_t*)r3.pointerChain(TheLastSurvivor_Win64_Shipping + 0x62E1208, { 0x0 ,0x300, 0x1E8, 0x8 }).data();
			r3.Write(addr + 0xA10, game_value.defense);//����
			r3.Write(addr + 0xA08, game_value.defense);//��������
		}
	}
}
// �޸�: Ѫ��ֵ
static void write_blood_volume() {
	if (TheLastSurvivor_Win64_Shipping) {
		if (game_value.blood_volume_flag) {
			size_t addr = *(size_t*)r3.pointerChain(TheLastSurvivor_Win64_Shipping + 0x63698C0, { 0x30 ,0x328, 0x388, 0x9B8, 0x2F0, 0x20 }).data();
			r3.Write(addr + 0x848, (double)6);//Ѫ��
			r3.Write(addr + 0x828, game_value.blood_volume);//Ѫ������
		}
	}
}
// �޸�: �ӵ���
static void write_bullet() {
	if (TheLastSurvivor_Win64_Shipping) {
		if (game_value.bullet_flag) {
			size_t addr = *(size_t*)r3.pointerChain(TheLastSurvivor_Win64_Shipping + 0x63698C0, { 0x30 ,0x328, 0x388, 0x9B8, 0x580, 0x40 }).data();
			r3.Write(addr + 0x8C8, game_value.bullet);//�ӵ���
			r3.Write(addr + 0x8D0, game_value.bullet);//�ӵ��󱸻�������
		}
	}
}
// �޸�: ���޴���
static void write_big_move() {
	if (TheLastSurvivor_Win64_Shipping) {
		if (game_value.big_move) {
			size_t addr = *(size_t*)r3.pointerChain(TheLastSurvivor_Win64_Shipping + 0x6233E18, { 0xA90 ,0xA0, 0x610, 0x20, 0x3A0, 0xE0 }).data();
			r3.Write(addr + 0x940, (double)100);//����ֵ
		}
	}
}
void Fnc::startListenProcess(PThread thread)
{
	int pid = Process::ProcessGetID("TheLastSurvivor-Win64-Shipping.exe");
	if (pid != -1 && r3.hProcess == NULL) {
		r3.Close();
		if (r3.Open(pid)) {
			TheLastSurvivor_Win64_Shipping = r3.GetModuleBase("TheLastSurvivor-Win64-Shipping.exe");
		}
	}
	else if(pid == -1){
		r3.hProcess = NULL;
	}
	else {
		write_defense();
		write_blood_volume();
		write_bullet();
		write_big_move();
	}
	if (FindWindow(NULL, L"����ƻ�2") !=  nullptr) {
		hwnd = FindWindow(NULL, L"����ƻ�2");
	}
	SetWindowText(hwnd, std::wstring(L"����ƻ�2 PID:").append(std::to_wstring(pid)).c_str());
}
