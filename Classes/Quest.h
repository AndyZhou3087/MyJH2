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

	/*************�������������߼�**************/
	//��ʼ������ɵ�����
	static bool initFinishTaskData();

	//��ʼ����ǰ��������ɵ���Ʒ
	static void initCurNeedData();

	//������������
	static void AddFinishQuest(TaskData data);

	//�ж϶�ѡһ�������Ƿ��л���,���з���true
	static int getMutexMainQuestType(int id);

	//�ж��Ƿ���ܴ�npc����
	static bool getMainQuestNpc(std::string npcid);

	//�ж��Ƿ���ڵ�ǰ��ͼ
	static bool getMainQuestMap(std::string mapid);

	//�ж�����1��������ĵ���Ʒid������
	static bool checkResQuestData(std::string resid, int count, std::string npcid);

	//�ж����ĵ���Ʒ�����Ƿ���
	static bool getResCountFinish();

	static void saveMainData();

	static void finishQuest();

	static void finishTaskMain(int ftype);

	static int getTypeBtn(int id, int ftype);

	//�ж��Ƿ������������
	static bool getMainQuest();

	static bool isMainQuestFinish(int questid);

	//�����Ƿ���ȡ�����ͽ��ܵ�ǰ����
	static bool isShowQuestTip();

	static void setFinishTaskState(int type, TaskData* data);

	/*************֧�����������߼�**************/

	//��ʼ������ɵ�����
	static bool initFinishTaskBranchData();

	//��ʼ����ǰ��������ɵ���Ʒ
	static void initCurBranchNeedData();

	//������������
	static void AddFinishBranchQuest(TaskData data);

	//�ж϶�ѡһ�������Ƿ��л���,���з���true
	static int getMutexBranchQuestType(int id);

	//�ж��Ƿ���ܴ�npc����
	static bool getBranchQuestNpc(std::string npcid);

	//�ж��Ƿ���ڵ�ǰ��ͼ
	static bool getBranchQuestMap(std::string mapid);

	//�ж�������Ʒ��������ĵ���Ʒid������
	static bool checkResBranchQuestData(std::string resid, int count, std::string npcid);

	//�ж����ĵ���Ʒ�����Ƿ���
	static bool getResBranchFinish();

	static void saveBranchData();

	static void finishBranchQuest();

	static void finishTaskBranch(int ftype);

	static int getTypeBranchBtn(int id, int ftype);

	//�ж��Ƿ����֧������
	static bool getBranchQuest();

	//֧���Ƿ���ȡ�����ͽ��ܵ�ǰ����
	static bool isShowBranchQuestTip();

	/*************ÿ�������߼�**************/
	//ÿ������
	static void initDailyTypeCount(std::string str);
	//ÿ�ջ���
	static void initDailyPointReward(std::string str);
	//���浱�����
	static void saveDailyPointReward(int p);
	//ÿ������
	static void setDailyTask(int type, int count);

	//����ÿ������
	static void resetDailyTask();

	//�ж�ÿ�������Ƿ��п���ȡ
	static bool getDailyTaskGetRewards();

	/*****************�ɾ����������߼�*****************/

	static void initAchieveTypeCount(std::string str);

	//ÿ������
	static void setAchieveTypeCount(int type, int count,std::string resid = "0");

	static void restData();

public:
	static std::vector<TaskData> myFinishMainQuest;

	static std::map<std::string, int> map_NpcQuestRes;

	static std::vector<TaskData> myFinishBranchQuest;

	//֧������npc��Ʒ
	static std::map<std::string, int> map_NpcBranchQuestRes;

	//ÿ������type-count
	static std::map<int, int> map_DailyTypeCount;

	//���ֽ���count-state
	static std::map<int, int> map_PointReward;

	//�ɾ�����type-count
	static std::map<int, std::map<std::string, int>> map_achieveTypeCount;

};

#endif
