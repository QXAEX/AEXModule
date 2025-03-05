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
	size_t addr = r3.GetModuleBase("main.exe");
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
	r3.Write(newAddr, data);
	Byteset buffer;
	r3.Read(newAddr, buffer, data.size());
	printf("新内存内容: %s\n", buffer.append({0}).data());
	r3.Write(newAddr, (int)114514);
	r3.Read(newAddr, buffer, sizeof(int));
	printf("新内存内容: %d\n", *(int*)buffer.data());
	r3.Free(newAddr);
	Byteset data2;//原来的数据
	r3.Read(addr + 0x1DE90, data2, 30);
	r3.hookJump(addr + 0x1DE90, addr + 0x1DED0, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },true);
	r3.Write(addr + 0x1DE90, data2,true);
	std::vector<size_t> list = r3.Search({ 0x40, 0x55, 0x57, 0x48, 0x81, 0xEC, 0xE8, 0x00, 0x00, 0x00, 0x48, 0x8D, 0x6C, 0x24, 0x20, 0x48, 0x8D, 0x0D, 0xDE, 0x61, 0x97, 0x00 });//找msg2函数头
	for (auto i : list) {
		printf("地址: %p\n", i);
	}
	r3.Close();
	printf("结束\n");
}