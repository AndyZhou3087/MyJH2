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

	//�жϻ���,���з��ػ���id�����ͣ������򷵻�-1
	static int getMutexMainQuestType();

public:
	static std::vector<TaskMainData> myFinishMainQuest;

public:
	
	static void saveData();
};

#endif
