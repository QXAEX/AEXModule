#include "../../../../AEXModule.h";
Memory::R3 r3;
Memory::R3HOOK hook;
void main() {
	DWORD pid = Process::ProcessGetID("main.exe");
	r3.Open(pid);
	size_t addr = r3.GetModuleBase("main.exe");
	hook.setCapture(true);//实时截取
	hook.setVirtual(true);//破虚拟
	if (!hook.Install(pid, addr + 0x1DE90, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 })) {
		printf("安装失败!\n");
		return;
	}
	printf("安装成功!\n");
	std::string data;
	while (data != "exit") {
		Memory::R3HOOK_INFO info = hook.ReadData();
		printf(
			"rax:%p, rbx:%p, rcx:%p, rdx:%p, rsi:%p, rdi:%p, rbp:%p, rsp:%p, r8:%p, r9:%p, r10:%p, r11:%p, r12:%p, r13:%p, r14:%p, r15:%p\n",
			info.rax, info.rbx, info.rcx, info.rdx, info.rsi, info.rdi, info.rbp, info.rsp, info.r8, info.r9, info.r10, info.r11, info.r12, info.r13, info.r14, info.r15
		);
		std::cin >> data;
	};
}