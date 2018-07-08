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

	//��ʼ����ǰ��������ɵ���Ʒ
	static void initCurNeedData();

	//������������
	static void AddFinishQuest(TaskMainData data);

	//�ж϶�ѡһ�������Ƿ��л���,���з���true
	static bool getMutexMainQuestType(int id, int type);

	//�ж��Ƿ���ܴ�npc����
	static bool getMainQuestNpc(std::string npcid);

	//�ж�����1��������ĵ���Ʒid������
	static void setResQuestData(std::string resid, int count, std::string npcid);

	//�ж����ĵ���Ʒ�����Ƿ���
	static bool getResCountFinish();

	static void saveData();

public:
	static std::vector<TaskMainData> myFinishMainQuest;

	static std::map<std::string, int> map_NpcQuestRes;

};

#endif
