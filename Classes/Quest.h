#pragma once
#ifndef __QUEST_H_
#define __QUEST_H_

#include "cocos2d.h"
#include "GlobalInstance.h"
class Quest
{
public:

	/*************主线任务数据逻辑**************/
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

	static void saveMainData();

	static void finishQuest();

	static void finishFightMain();

	/*************支线任务数据逻辑**************/

	//判断是否接受此npc任务
	static bool getBranchQuestNpc(std::string npcid);

	//判断赠送物品情况下消耗的物品id和数量
	static void setResBranchQuestData(std::string resid, int count, std::string npcid);

	//判断消耗的物品数量是否达标
	static bool getResBranchFinish();

	static void saveBranchData();

	static void finishBranchQuest();

	/*************每日数据逻辑**************/
	//每日任务
	static void initDailyTypeCount(std::string str);
	//每日积分
	static void initDailyPointReward(std::string str);
	//保存当天积分
	static void saveDailyPointReward(int p);
	//每日任务
	static void setDailyTask(int type, int count);

	//重置每日任务
	static void resetDailyTask();

public:
	static std::vector<TaskMainData> myFinishMainQuest;

	static std::map<std::string, int> map_NpcQuestRes;

	//支线任务npc物品
	static std::map<std::string, int> map_NpcBranchQuestRes;

	//每日任务type-count
	static std::map<int, int> map_DailyTypeCount;

	//积分奖励count-state
	static std::map<int, int> map_PointReward;

};

#endif
