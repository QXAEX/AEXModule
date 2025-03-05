#pragma once
#define AEX_LIB_TYPE "MTd" // 根据你项目的设置 项目属性 -> C/C++ -> 代码生成 -> 运行库 进行对应名字的选择
#define AEX_LIB_PATH_TOP "../../../libs/AEXModule_"// 优先定义相对路径，独属本项目的路径
#include "../../../../AEXModule.h"
struct GAME_VALUE {
	double defense = 114514;//防御值
	bool defense_flag = false;//防御开关
	double blood_volume = 114514;//血量值
	bool blood_volume_flag = false;//血量开关
	int bullet = 100;//子弹值
	bool bullet_flag = false;//子弹开关
	bool big_move = false;//无限大招
};
extern GAME_VALUE game_value;
static class Fnc
{
public:
	static void msg();
	static void startListenProcess(PThread thread);
};

