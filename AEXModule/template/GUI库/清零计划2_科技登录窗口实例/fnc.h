#pragma once
#define AEX_LIB_TYPE "MTd" // ��������Ŀ������ ��Ŀ���� -> C/C++ -> �������� -> ���п� ���ж�Ӧ���ֵ�ѡ��
#define AEX_LIB_PATH_TOP "../../../libs/AEXModule_"// ���ȶ������·������������Ŀ��·��
#include "../../../../AEXModule.h"
struct GAME_VALUE {
	double defense = 114514;//����ֵ
	bool defense_flag = false;//��������
	double blood_volume = 114514;//Ѫ��ֵ
	bool blood_volume_flag = false;//Ѫ������
	int bullet = 100;//�ӵ�ֵ
	bool bullet_flag = false;//�ӵ�����
	bool big_move = false;//���޴���
};
extern GAME_VALUE game_value;
static class Fnc
{
public:
	static void msg();
	static void startListenProcess(PThread thread);
};

