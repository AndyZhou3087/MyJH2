#pragma once
#ifndef __QUEST_H_
#define __QUEST_H_

#include "cocos2d.h"
#include "GlobalInstance.h"
class Quest
{
public:

	//��ʼ������ɵ�����
	static bool initFinishTaskData();

	//������������
	static void AddFinishQuest(TaskMainData data);

	//�ж϶�ѡһ�������Ƿ��л���,���з��ػ���id�����ͣ������򷵻�-1
	static int getMutexMainQuestType(int id, int type);

public:
	static std::vector<TaskMainData> myFinishMainQuest;

public:
	
	static void saveData();
};

#endif
