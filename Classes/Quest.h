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

	//添加已完成任务
	static void AddFinishQuest(TaskMainData data);

	//判断二选一条件下是否有互斥,若有返回互斥id的类型，若无则返回-1
	static int getMutexMainQuestType(int id, int type);

public:
	static std::vector<TaskMainData> myFinishMainQuest;

public:
	
	static void saveData();
};

#endif
