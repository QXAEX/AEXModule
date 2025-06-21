#include "../../../../AEXModule.h";
Memory::R3 r3;
void main() {
	DWORD pid = Process::ProcessGetID("main.exe");
	if (pid == -1) {
		printf("进程不存在\n");
		return;
	}
	if (!r3.Open(pid)) {
		printf("进程打开失败\n");
		return;
	}
	size_t addr = (size_t)r3.GetModuleBase("main.exe");
	if (addr == 0) {
		printf("模块地址获取失败\n");
		return;
	}
	printf("main.exe 基址: %p\n", addr);
	size_t newAddr = r3.Allocate();
	if (newAddr == 0) {
		printf("内存分配失败\n");
		return;
	}
	printf("新内存地址: %p\n", newAddr);
	Byteset data = "Hello, world!";
	r3.Write((PVOID)newAddr, data);
	Byteset buffer;
	r3.Read((PVOID)newAddr, buffer, data.size());
	printf("新内存内容: %s\n", buffer.append({ 0 }).data());
	r3.Write((PVOID)newAddr, (int)114514);
	r3.Read((PVOID)newAddr, buffer, sizeof(int));
	printf("新内存内容: %d\n", *(int*)buffer.data());
	r3.Free((PVOID)newAddr);
	Byteset data2;//原来的数据
	r3.Read((PVOID)(addr + 0x1DE90), data2, 30);
	r3.hookJump((PVOID)(addr + 0x1DE90), (PVOID)(addr + 0x1DED0), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, true);
	r3.Write((PVOID)(addr + 0x1DE90), data2, true);
	std::vector<PVOID> list = r3.Search("40 55 57 48 81 EC 08 01 00 00 48 8D 6C 24 20 48 8D 0D 0E 62 97 00", true);//找msg2函数头
	for (auto i : list) {
		printf("地址: %p\n", i);
	}
	r3.Close();
	printf("结束\n");
}