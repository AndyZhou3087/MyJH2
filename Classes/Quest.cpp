#include "Quest.h"
#include "DataSave.h"
#include "CommonFuncs.h"
#include "MyRes.h"
#include "Resource.h"
#include "SoundManager.h"
#include "MapBlockScene.h"
#include "ResBase.h"
#include "Const.h"

std::vector<TaskData> Quest::myFinishMainQuest;
std::vector<TaskData> Quest::myFinishBranchQuest;
std::map<int, int> Quest::map_DailyTypeCount;
std::map<int, int> Quest::map_PointReward;

std::map<int, std::map<std::string, int>> Quest::map_achieveTypeCount;

bool Quest::initFinishTaskData()
{
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		TaskData data = GlobalInstance::vec_TaskMain[i];
		if (data.isfinish >= QUEST_FINISH)
		{
			myFinishMainQuest.push_back(data);
		}
	}

	return true;
}


void Quest::AddFinishQuest(TaskData data)
{
	myFinishMainQuest.push_back(data);
	//特效
	CommonFuncs::playCommonLvUpAnim(g_MapBlockScene, "texiao_rwwc");
}

void Quest::saveMainData()
{
	std::string str;
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		if (GlobalInstance::myCurMainData.id == GlobalInstance::vec_TaskMain[i].id)
		{
			GlobalInstance::vec_TaskMain[i].isfinish = GlobalInstance::myCurMainData.isfinish;
			GlobalInstance::vec_TaskMain[i].finishtype = GlobalInstance::myCurMainData.finishtype;
			AddFinishQuest(GlobalInstance::vec_TaskMain[i]);
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
			if (i + 1 < GlobalInstance::vec_TaskMain.size())
			{
				GlobalInstance::myCurMainData = GlobalInstance::vec_TaskMain[i + 1];//当前任务下一个
			}
			break;
		}
	}
	GlobalInstance::getInstance()->saveMyTaskMainData();

	//解锁章节
	if (g_MapBlockScene != NULL)
	{
		g_MapBlockScene->showUnlockChapter();
	}
}

int Quest::getMutexMainQuestType(int id)
{
	for (unsigned int i = 0; i < myFinishMainQuest.size(); i++)
	{
		TaskData data = myFinishMainQuest[i];
		if (data.mutex1.size() > 0)
		{
			int fid = data.mutex1[0];
			int ftype = data.mutex1[1];
			//判断互斥1里是否有当前任务id
			if (id == fid)
			{
				return ftype;
			}
		}
		if (data.mutex2.size() > 0)
		{
			int fid = data.mutex2[0];
			int ftype = data.mutex2[1];
			//判断互斥2里是否有当前任务id
			if (id == fid)
			{
				return ftype;
			}
		}
	}
	return 0;
}

bool Quest::getMainQuestNpc(std::string npcid)
{
	if (GlobalInstance::myCurMainData.isfinish == QUEST_ACC && npcid.compare(GlobalInstance::myCurMainData.npcid) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Quest::getMainQuestMap(std::string mapid)
{
	if (GlobalInstance::myCurMainData.place.compare(mapid) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Quest::checkResQuestData(std::string resid, int count, std::string npcid, std::vector<std::map<std::string, int>> need)
{
	if (!getMainQuestNpc(npcid))
	{
		return false;
	}
	for (unsigned int i = 0; i < need.size(); i++)
	{
		std::map<std::string, int> one_res = need[i];
		std::map<std::string, int>::iterator oneit = one_res.begin();
		std::string cresid = oneit->first;
		if (resid.compare(cresid) == 0)
		{
			if (cresid.compare("r006") == 0)//银两
			{
				if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= count)
				{
					DynamicValueInt dval;
					dval.setValue(count);
					GlobalInstance::getInstance()->costMySoliverCount(dval);
				}
			}
			else if (cresid.compare("r012") == 0)//元宝
			{
				if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= count)
				{
					DynamicValueInt dval;
					dval.setValue(count);
					GlobalInstance::getInstance()->costMyCoinCount(dval);
				}
			}
			else
			{
				//先判断仓库里的数量是否足够，再判断背包数量

				int where1 = MYPACKAGE;
				int where2 = MYSTORAGE;
				if (cresid.compare("r001") == 0)
				{
					where1 = MYSTORAGE;
					where2 = MYPACKAGE;
				}
				int count1 = MyRes::getMyResCount(cresid, where1);
				int count2 = MyRes::getMyResCount(cresid, where2);
				if (count1 >= count)
				{
					ResBase* res = MyRes::getMyRes(resid, where1);
					if (res != NULL)
					{
						MyRes::Use(res, count, where1);
					}
				}
				else
				{
					ResBase* res = MyRes::getMyRes(resid, where1);
					if (res != NULL && count1 > 0)
					{
						MyRes::Use(res, count1, where1);
					}
					if (count2 >= count - count1)
					{
						ResBase* res = MyRes::getMyRes(resid, where2);
						if (res != NULL)
						{
							MyRes::Use(res, count - count1, where2);
						}
					}
				}	
			}
		}
	}

	finishTaskMain();

	return true;
}

void Quest::finishTaskMain()
{
	GlobalInstance::myCurMainData.isfinish = QUEST_FINISH;
	saveMainData();

	int mtid = GlobalInstance::myCurMainData.id;
	//完成主线任务获得点赞券
	for (int i = 0; i < sizeof(ADDZANMAINTASKCOUNTS) / sizeof(ADDZANMAINTASKCOUNTS[0]); i++)
	{
		if (mtid == ADDZANMAINTASKCOUNTS[i])
		{
			if (mtid > 5)
			{
				MyRes::Add("j004", 2);
			}
			else
				MyRes::Add("j004", 1);
			break;
		}
	}

#ifdef UMENG
	umeng::eventDict dict;
	std::string eventstr = StringUtils::format("maintask_%d", GlobalInstance::myCurMainData.id);
	dict["status"] = "finish";
	umeng::MobClickCpp::event(eventstr.c_str(), &dict);
#endif
}

bool Quest::getMainQuest()
{
	if (GlobalInstance::myCurMainData.isfinish == QUEST_ACC)
	{
		return true;
	}
	return false;
}

bool Quest::isMainQuestFinish(int questid)
{
	for (unsigned int i = 0; i < myFinishMainQuest.size(); i++)
	{
		TaskData data = myFinishMainQuest[i];
		if (data.id == questid)
		{
			if (data.isfinish >= QUEST_FINISH)
			{
				return true;
			}
		}
	}
	return false;
}

bool Quest::isShowQuestTip()
{
	if (GlobalInstance::myCurMainData.isfinish != QUEST_ACC)
	{
		return true;
	}
	/*for (unsigned int i = 0; i < myFinishMainQuest.size(); i++)
	{
		TaskData data = myFinishMainQuest[i];
		if (data.isfinish != QUEST_GET)
		{
			return true;
		}
	}*/
	return false;
}

/*************支线任务**************/

bool Quest::initFinishTaskBranchData()
{
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskBranch.size(); i++)
	{
		TaskData data = GlobalInstance::vec_TaskBranch[i];
		if (data.isfinish >= QUEST_FINISH)
		{
			myFinishBranchQuest.push_back(data);
		}
	}

	return true;
}

int Quest::getMutexBranchQuestType(int id)
{
	for (unsigned int i = 0; i < myFinishBranchQuest.size(); i++)
	{
		TaskData data = myFinishBranchQuest[i];
		if (data.mutex1.size() > 0)
		{
			int fid = data.mutex1[0];
			int ftype = data.mutex1[1];
			//判断互斥1里是否有当前任务id
			if (id == fid)
			{
				return ftype;
			}
		}
		if (data.mutex2.size() > 0)
		{
			int fid = data.mutex2[0];
			int ftype = data.mutex2[1];
			//判断互斥2里是否有当前任务id
			if (id == fid)
			{
				return ftype;
			}
		}
	}
	return 0;
}

bool Quest::getBranchQuestNpc(std::string npcid)
{
	if (GlobalInstance::myCurBranchData.isfinish == QUEST_ACC && npcid.compare(GlobalInstance::myCurBranchData.npcid) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Quest::getBranchQuestMap(std::string mapid)
{
	if (GlobalInstance::myCurBranchData.place.compare(mapid) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Quest::checkResBranchQuestData(std::string resid, int count, std::string npcid, std::vector<std::map<std::string, int>> need)
{
	if (!getBranchQuestNpc(npcid))
	{
		return false;
	}
	for (unsigned int i = 0; i < need.size(); i++)
	{
		std::map<std::string, int> one_res = need[i];
		std::map<std::string, int>::iterator oneit = one_res.begin();
		std::string cresid = oneit->first;
		if (resid.compare(cresid) == 0)
		{
			if (cresid.compare("r006") == 0)//银两
			{
				if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= count)
				{
					DynamicValueInt dval;
					dval.setValue(count);
					GlobalInstance::getInstance()->costMySoliverCount(dval);
				}
			}
			else if (cresid.compare("r012") == 0)//元宝
			{
				if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= count)
				{
					DynamicValueInt dval;
					dval.setValue(count);
					GlobalInstance::getInstance()->costMyCoinCount(dval);
				}
			}
			else
			{
				int where1 = MYPACKAGE;
				int where2 = MYSTORAGE;
				if (cresid.compare("r001") == 0)
				{
					where1 = MYSTORAGE;
					where2 = MYPACKAGE;
				}
				int count1 = MyRes::getMyResCount(cresid, where1);
				int count2 = MyRes::getMyResCount(cresid, where2);
				if (count1 >= count)
				{
					ResBase* res = MyRes::getMyRes(resid, where1);
					if (res != NULL)
					{
						MyRes::Use(res, count, where1);
					}
				}
				else
				{
					ResBase* res = MyRes::getMyRes(resid, where1);
					if (res != NULL && count1 > 0)
					{
						MyRes::Use(res, count1, where1);
					}
					if (count2 >= count - count1)
					{
						ResBase* res = MyRes::getMyRes(resid, where2);
						if (res != NULL)
						{
							MyRes::Use(res, count - count1, where2);
						}
					}
				}
			}
		}
	}
	finishTaskBranch();
	return true;
}

void Quest::AddFinishBranchQuest(TaskData data)
{
	myFinishBranchQuest.push_back(data);
	//特效
	CommonFuncs::playCommonLvUpAnim(g_MapBlockScene, "texiao_rwwc");
}

void Quest::finishTaskBranch()
{
	GlobalInstance::myCurBranchData.isfinish = QUEST_FINISH;
	saveBranchData();
	setDailyTask(FINISH_BRANCH, 1);
	setAchieveTypeCount(FINISH_BRANCH, 1);

#ifdef UMENG
	umeng::eventDict dict;
	std::string eventstr = StringUtils::format("branchtask_%d", GlobalInstance::myCurBranchData.id);
	dict["status"] = "finish";
	umeng::MobClickCpp::event(eventstr.c_str(), &dict);
#endif
}

void Quest::saveBranchData()
{
	std::string str;
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskBranch.size(); i++)
	{
		if (GlobalInstance::myCurBranchData.id == GlobalInstance::vec_TaskBranch[i].id)
		{
			GlobalInstance::vec_TaskBranch[i].isfinish = GlobalInstance::myCurBranchData.isfinish;
			GlobalInstance::vec_TaskBranch[i].finishtype = GlobalInstance::myCurBranchData.finishtype;
			AddFinishBranchQuest(GlobalInstance::vec_TaskBranch[i]);
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
			if (i + 1 < GlobalInstance::vec_TaskBranch.size())
			{
				GlobalInstance::myCurBranchData = GlobalInstance::vec_TaskBranch[i + 1];//当前任务下一个
			}
			break;
		}
	}
	GlobalInstance::getInstance()->saveMyTaskBranchData();
}

bool Quest::getBranchQuest()
{
	if (GlobalInstance::myCurBranchData.isfinish == QUEST_ACC)
	{
		return true;
	}
	return false;
}

bool Quest::isShowBranchQuestTip()
{
	if (GlobalInstance::myCurBranchData.isfinish != QUEST_ACC)
	{
		return true;
	}
	/*for (unsigned int i = 0; i < myFinishBranchQuest.size(); i++)
	{
		TaskData data = myFinishBranchQuest[i];
		if (data.isfinish != QUEST_GET)
		{
			return true;
		}
	}*/
	return false;
}

/*************每日数据逻辑**************/

void Quest::initDailyTypeCount(std::string str)
{
	std::vector<std::string> vec_tmp;
	CommonFuncs::split(str, vec_tmp, ";");
	for (unsigned int i = 0; i < vec_tmp.size(); i++)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(vec_tmp[i], vec_one, "-");
		map_DailyTypeCount[atoi(vec_one[0].c_str())] = atoi(vec_one[1].c_str());
	}
}

void Quest::initDailyPointReward(std::string str)
{
	std::vector<std::string> vec_tmp;
	CommonFuncs::split(str, vec_tmp, ";");
	for (unsigned int i = 0; i < vec_tmp.size(); i++)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(vec_tmp[i], vec_one, "-");
		map_PointReward[atoi(vec_one[0].c_str())] = atoi(vec_one[1].c_str());
	}
}

void Quest::saveDailyPointReward(int p)
{
	map_PointReward[p] = 1;
	std::string str;
	std::map<int, int>::iterator it;
	for (it = map_PointReward.begin(); it != map_PointReward.end(); it++)
	{
		std::string onestr = StringUtils::format("%d-%d;", it->first, it->second);
		str.append(onestr);
	}
	DataSave::getInstance()->setMyDailyReward(str.substr(0, str.length() - 1));
}

void Quest::setDailyTask(int type, int count)
{
	map_DailyTypeCount[type] += count;

	std::map<std::string, DailyTaskData>::iterator it;
	for (it = GlobalInstance::map_DTdata.begin(); it != GlobalInstance::map_DTdata.end(); it++)
	{
		DailyTaskData* data = &GlobalInstance::map_DTdata[it->first];
		if (data->type == type && map_DailyTypeCount[type] >= data->count && data->state == DAILY_UNFINISHED)
		{
			data->state = DAILY_FINISHED;
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
		}
	}
	GlobalInstance::getInstance()->saveMyDailyTaskData();

	std::string str;
	std::map<int, int>::iterator mit;
	for (mit = map_DailyTypeCount.begin(); mit != map_DailyTypeCount.end(); mit++)
	{
		std::string onestr = StringUtils::format("%d-%d;", mit->first, mit->second);
		str.append(onestr);
	}
	DataSave::getInstance()->setDailyTypeCount(str.substr(0, str.length() - 1));
}

void Quest::resetDailyTask()
{
	DataSave::getInstance()->setDailyTypeCount("0-0;1-0;2-0;3-0;4-0;5-0;6-0;7-0;10-0;11-0");
	DataSave::getInstance()->setMyDailyReward("50-0;100-0;150-0;200-0");
	DataSave::getInstance()->setMyDailyPoint(0);
	GlobalInstance::getInstance()->loadDailyTaskData();
	DataSave::getInstance()->setMyDailyTaskData("");

	int zerotime = GlobalInstance::servertime + 8 * 60 * 60;
	int t = zerotime / TWENTYFOURHOURSTOSEC;
	DataSave::getInstance()->setMyFreshDate(t);
	std::string str = DataSave::getInstance()->getDailyTypeCount();
	if (str.length()>0)
	{
		initDailyTypeCount(str);
	}
	str = DataSave::getInstance()->getMyyDailyReward();
	if (str.length()>0)
	{
		initDailyPointReward(str);
	}
}

bool Quest::getDailyTaskGetRewards()
{
	std::map<std::string, DailyTaskData>::iterator it;
	for (it = GlobalInstance::map_DTdata.begin(); it != GlobalInstance::map_DTdata.end(); it++)
	{
		DailyTaskData data = GlobalInstance::map_DTdata[it->first];
		if (data.state == DAILY_FINISHED)
		{
			return true;
		}
	}
	return false;
}

/**********************成就***************************/

void Quest::initAchieveTypeCount(std::string str)
{
	std::vector<std::string> vec_tmp;
	CommonFuncs::split(str, vec_tmp, ";");
	for (unsigned int i = 0; i < vec_tmp.size(); i++)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(vec_tmp[i], vec_one, "-");
		//std::map<std::string, int> map_one;
		//map_one[vec_one[1]] = atoi(vec_one[2].c_str());
		map_achieveTypeCount[atoi(vec_one[0].c_str())][vec_one[1]] = atoi(vec_one[2].c_str());
	}
}

void Quest::setAchieveTypeCount(int type, int count, std::string resid)
{
	//if (type == HERO_LEVEL)
	//{
	//	if (count > map_achieveTypeCount[type][resid])
	//	{
	//		map_achieveTypeCount[type][resid] = count;
	//	}
	//}
	//else
	//{
	//	map_achieveTypeCount[type][resid] += count;
	//}

	for (unsigned int i = 0; i < GlobalInstance::vec_achievedata.size(); i++)
	{
		AchieveData* data = &GlobalInstance::vec_achievedata[i];
		if (data->type == type && data->achid.compare(resid) == 0)
		{
			if (type == HERO_LEVEL || type == GET_STARS || type == FINISH_MAIN)
			{
				if (count > map_achieveTypeCount[type][resid])
				{
					map_achieveTypeCount[type][resid] = count;
				}
			}
			else
			{
				map_achieveTypeCount[type][resid] += count;
			}
			break;
		}
	}

	for (unsigned int i = 0; i < GlobalInstance::vec_achievedata.size(); i++)
	{
		AchieveData* data = &GlobalInstance::vec_achievedata[i];
		if (data->type == type && data->achid.compare(resid) == 0)
		{
			if (data->count <= map_achieveTypeCount[type][resid] && data->state == DAILY_UNFINISHED)
			{
				data->state = DAILY_FINISHED;

				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
			}
		}
	}
	GlobalInstance::getInstance()->saveMyAchieveData();

	std::string str;
	std::map<int, std::map<std::string, int>>::iterator mit;
	for (mit = map_achieveTypeCount.begin(); mit != map_achieveTypeCount.end(); mit++)
	{
		std::map<std::string, int>::iterator submit;
		for (submit = mit->second.begin(); submit != mit->second.end(); submit++)
		{
			std::string key = submit->first;
			std::string onestr = StringUtils::format("%d-%s-%d;", mit->first, key.c_str(), submit->second);
			str.append(onestr);
		}
	}
	DataSave::getInstance()->setAchieveTypeCount(str.substr(0, str.length() - 1));
}

/***************主线支线***************/
void Quest::setFinishTaskState(int type, TaskData* data)
{
	if (type == MAIN)
	{
		for (unsigned int i = 0; i < myFinishMainQuest.size(); i++)
		{
			if (data->id == myFinishMainQuest[i].id)
			{
				myFinishMainQuest[i].isfinish = data->isfinish;
				break;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < myFinishBranchQuest.size(); i++)
		{
			if (data->id == myFinishBranchQuest[i].id)
			{
				myFinishBranchQuest[i].isfinish = data->isfinish;
				break;
			}
		}
	}
}

void Quest::restData()
{
	myFinishMainQuest.clear();
	myFinishBranchQuest.clear();
	map_DailyTypeCount.clear();
	map_PointReward.clear();

	map_achieveTypeCount.clear();
}