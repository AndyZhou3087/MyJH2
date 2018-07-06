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

	//判断互斥,若有返回互斥id的类型，若无则返回-1
	static int getMutexMainQuestType();

public:
	static std::vector<TaskMainData> myFinishMainQuest;

public:
	
	static void saveData();
};

#endif
