#pragma once
#ifndef __QUEST_H_
#define __QUEST_H_

#include "cocos2d.h"
#include "GlobalInstance.h"
class Quest
{
public:

	//初始化已完成的任务
	static bool initFinishTaskData();

	//初始化当前任务已完成的物品
	static void initCurNeedData();

	//添加已完成任务
	static void AddFinishQuest(TaskMainData data);

	//判断二选一条件下是否有互斥,若有返回true
	static bool getMutexMainQuestType(int id, int type);

	//判断是否接受此npc任务
	static bool getMainQuestNpc(std::string npcid);

	//判断条件1情况下消耗的物品id和数量
	static void setResQuestData(std::string resid, int count, std::string npcid);

	//判断消耗的物品数量是否达标
	static bool getResCountFinish();

	static void saveData();

public:
	static std::vector<TaskMainData> myFinishMainQuest;

	static std::map<std::string, int> map_NpcQuestRes;

};

#endif
