#pragma once
#ifndef __QUEST_H_
#define __QUEST_H_

#include "cocos2d.h"
#include "GlobalInstance.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

typedef enum
{
	BTN_1 = 1,
	BTN_2,
}TASKBTNTYPE;

typedef enum
{
	MAIN = 0,
	BRANCH,
}QUESTTASKTYPE;

class Quest
{
public:

	/*************主线任务数据逻辑**************/
	//初始化已完成的任务
	static bool initFinishTaskData();

	//添加已完成任务
	static void AddFinishQuest(TaskData data);

	//判断二选一条件下是否有互斥,若有返回true
	static int getMutexMainQuestType(int id);

	//判断是否接受此npc任务
	static bool getMainQuestNpc(std::string npcid);

	//判断是否接在当前地图
	static bool getMainQuestMap(std::string mapid);

	//判断条件1情况下消耗的物品id和数量
	static bool checkResQuestData(std::string resid, int count, std::string npcid, std::vector<std::map<std::string, int>> need);

	static void saveMainData();

	static void finishTaskMain();

	//判断是否接受主线任务
	static bool getMainQuest();

	static bool isMainQuestFinish(int questid);

	//主线是否领取奖励和接受当前任务
	static bool isShowQuestTip();

	static void setFinishTaskState(int type, TaskData* data);

	/*************支线任务数据逻辑**************/

	//初始化已完成的任务
	static bool initFinishTaskBranchData();

	//添加已完成任务
	static void AddFinishBranchQuest(TaskData data);

	//判断二选一条件下是否有互斥,若有返回true
	static int getMutexBranchQuestType(int id);

	//判断是否接受此npc任务
	static bool getBranchQuestNpc(std::string npcid);

	//判断是否接在当前地图
	static bool getBranchQuestMap(std::string mapid);

	//判断赠送物品情况下消耗的物品id和数量
	static bool checkResBranchQuestData(std::string resid, int count, std::string npcid, std::vector<std::map<std::string, int>> need);

	static void saveBranchData();

	static void finishTaskBranch();

	//判断是否接受支线任务
	static bool getBranchQuest();

	//支线是否领取奖励和接受当前任务
	static bool isShowBranchQuestTip();

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

	//判断每日任务是否有可领取
	static bool getDailyTaskGetRewards();

	/*****************成就任务数据逻辑*****************/

	static void initAchieveTypeCount(std::string str);

	//每日任务
	static void setAchieveTypeCount(int type, int count,std::string resid = "0");

	static void restData();

public:
	static std::vector<TaskData> myFinishMainQuest;

	static std::vector<TaskData> myFinishBranchQuest;

	//每日任务type-count
	static std::map<int, int> map_DailyTypeCount;

	//积分奖励count-state
	static std::map<int, int> map_PointReward;

	//成就任务type-count
	static std::map<int, std::map<std::string, int>> map_achieveTypeCount;

};

#endif
