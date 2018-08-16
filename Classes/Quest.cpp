#include "Quest.h"
#include "DataSave.h"
#include "CommonFuncs.h"
#include "MyRes.h"
#include "Resource.h"
#include "SoundManager.h"

std::vector<TaskMainData> Quest::myFinishMainQuest;
std::map<std::string, int> Quest::map_NpcQuestRes;
std::map<std::string, int> Quest::map_NpcBranchQuestRes;
std::map<int, int> Quest::map_DailyTypeCount;
std::map<int, int> Quest::map_PointReward;

std::map<int, std::map<std::string, int>> Quest::map_achieveTypeCount;

bool Quest::initFinishTaskData()
{
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		TaskMainData data = GlobalInstance::vec_TaskMain[i];
		if (data.isfinish >= QUEST_FINISH)
		{
			myFinishMainQuest.push_back(data);
		}
	}

	return true;
}

void Quest::initCurNeedData()
{
	std::string str = DataSave::getInstance()->getMyCurTaskNeed();
	if (str.length()>0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			std::string resid = vec_one[0];
			int count = atoi(vec_one[1].c_str());
			map_NpcQuestRes[resid] = count;
		}
	}
}

void Quest::AddFinishQuest(TaskMainData data)
{
	myFinishMainQuest.push_back(data);
	map_NpcQuestRes.clear();
	DataSave::getInstance()->setMyCurTaskNeed("");
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
			AddFinishQuest(GlobalInstance::myCurMainData);
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
			if (i + 1 < GlobalInstance::vec_TaskMain.size())
			{
				GlobalInstance::myCurMainData = GlobalInstance::vec_TaskMain[i + 1];//当前任务下一个
			}
			break;
		}
	}
	GlobalInstance::getInstance()->saveMyTaskMainData();
}

int Quest::getTypeBtn(int id,int ftype)
{
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		if (GlobalInstance::vec_TaskMain[i].id == id)
		{
			for (unsigned int j = 0; j < GlobalInstance::vec_TaskMain[i].type.size(); j++)
			{
				if (GlobalInstance::vec_TaskMain[i].type[j] == GlobalInstance::vec_TaskMain[i].finishtype)
				{
					return j;
				}
			}
			break;
		}
	}
	return -1;
}

bool Quest::getMutexMainQuestType(int id, int type)
{
	for (unsigned int i = 0; i < myFinishMainQuest.size(); i++)
	{
		TaskMainData data = myFinishMainQuest[i];
		if (getTypeBtn(data.id, data.finishtype) == BTN_1)
		{
			if (data.mutex1.size() > 0)
			{
				int fid = data.mutex1[0];
				int ftype = data.mutex1[1];
				//判断互斥1里是否有当前任务id
				if (id == fid)
				{
					if (type == ftype)
					{
						return true;
					}
				}
			}
		}
		else
		{
			if (data.mutex2.size() > 0)
			{
				int fid = data.mutex2[0];
				int ftype = data.mutex2[1];
				//判断互斥2里是否有当前任务id
				if (id == fid)
				{
					if (type == ftype)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
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

void Quest::setResQuestData(std::string resid, int count, std::string npcid)
{
	if (!getMainQuestNpc(npcid))
	{
		return;
	}
	for (unsigned int i = 0; i < GlobalInstance::myCurMainData.need1.size(); i++)
	{
		std::map<std::string, int> one_res = GlobalInstance::myCurMainData.need1[i];
		std::map<std::string, int>::iterator oneit = one_res.begin();
		std::string cresid = oneit->first;
		if (resid.compare(cresid) == 0)
		{
			map_NpcQuestRes[resid] += count;
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
				MyRes::Use(resid, count, MYPACKAGE);
			}
		}
	}

	//判断是否给予完成
	if (getResCountFinish())
	{
		finishQuest();

		std::string curstr;
		std::map<std::string, int>::iterator it;
		for (it = map_NpcQuestRes.begin(); it != map_NpcQuestRes.end(); it++)
		{
			std::string cresid = it->first;
			std::string onestr = StringUtils::format("%s-%d;", cresid.c_str(), map_NpcQuestRes[it->first]);
			curstr.append(onestr);
		}
		DataSave::getInstance()->setMyCurTaskNeed(curstr.substr(0, curstr.length() - 1));
	}
}

bool Quest::getResCountFinish()
{
	if (GlobalInstance::myCurMainData.isfinish == QUEST_TASK)
	{
		return false;
	}
	int fcount = 0;
	for (unsigned int i = 0; i < GlobalInstance::myCurMainData.need1.size(); i++)
	{
		std::map<std::string, int> one_res = GlobalInstance::myCurMainData.need1[i];
		std::map<std::string, int>::iterator oneit = one_res.begin();
		std::string cresid = oneit->first;
		if (map_NpcQuestRes[cresid] >= one_res[cresid])
		{
			fcount++;
		}
	}

	if (fcount == GlobalInstance::myCurMainData.need1.size())
	{
		return true;
	}
	return false;
}

void Quest::finishQuest()
{
	GlobalInstance::myCurMainData.isfinish = QUEST_FINISH;
	GlobalInstance::myCurMainData.finishtype = QUEST_GIVE;
	saveMainData();
}

void Quest::finishFightMain(int ftype)
{
	GlobalInstance::myCurMainData.isfinish = QUEST_FINISH;
	GlobalInstance::myCurMainData.finishtype = ftype;
	saveMainData();
}

bool Quest::getMainQuest()
{
	if (GlobalInstance::myCurMainData.isfinish == QUEST_ACC)
	{
		return true;
	}
	return false;
}

/*************支线任务**************/

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

void Quest::setResBranchQuestData(std::string resid, int count, std::string npcid)
{
	if (!getBranchQuestNpc(npcid))
	{
		return;
	}
	for (unsigned int i = 0; i < GlobalInstance::myCurBranchData.need.size(); i++)
	{
		std::vector<std::string> one_res = GlobalInstance::myCurBranchData.need[i];
		std::string cresid = one_res[0];
		int count = atoi(one_res[1].c_str());
		if (resid.compare(cresid) == 0)
		{
			map_NpcBranchQuestRes[resid] += count;
		}
	}

	//判断是否给予完成
	if (getResBranchFinish())
	{
		finishBranchQuest();

		std::string curstr;
		std::map<std::string, int>::iterator it;
		for (it = map_NpcBranchQuestRes.begin(); it != map_NpcBranchQuestRes.end(); it++)
		{
			std::string cresid = it->first;
			std::string onestr = StringUtils::format("%s-%d;", cresid.c_str(), map_NpcBranchQuestRes[it->first]);
			curstr.append(onestr);
		}
		DataSave::getInstance()->setMyCurBranchNeed(curstr.substr(0, curstr.length() - 1));
	}
}

bool Quest::getResBranchFinish()
{
	if (GlobalInstance::myCurBranchData.isfinish == QUEST_TASK)
	{
		return false;
	}
	int fcount = 0;
	for (unsigned int i = 0; i < GlobalInstance::myCurBranchData.need.size(); i++)
	{
		std::vector<std::string> one_res = GlobalInstance::myCurBranchData.need[i];
		std::string cresid = one_res[0];
		int count = atoi(one_res[1].c_str());
		if (map_NpcBranchQuestRes[cresid] >= count)
		{
			fcount++;
		}
	}

	if (fcount == GlobalInstance::myCurBranchData.need.size())
	{
		return true;
	}
	return false;
}

void Quest::finishBranchQuest()
{
	GlobalInstance::myCurBranchData.isfinish = QUEST_FINISH;
	saveBranchData();
	setDailyTask(FINISH_BRANCH, 1);
}

void Quest::saveBranchData()
{
	std::string str;
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskBranch.size(); i++)
	{
		if (GlobalInstance::myCurBranchData.id == GlobalInstance::vec_TaskBranch[i].id)
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
			GlobalInstance::vec_TaskBranch[i].isfinish = GlobalInstance::myCurBranchData.isfinish;
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
		if (data->type == type && data->count == map_DailyTypeCount[type])
		{
			data->state = DAILY_FINISHED;
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
			break;
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
	int t = GlobalInstance::servertime / 60 / 60 / 24;
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

/**********************成就***************************/

void Quest::initAchieveTypeCount(std::string str)
{
	std::vector<std::string> vec_tmp;
	CommonFuncs::split(str, vec_tmp, ";");
	for (unsigned int i = 0; i < vec_tmp.size(); i++)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(vec_tmp[i], vec_one, "-");
		std::map<std::string, int> map_one;
		map_one[vec_one[1]] = atoi(vec_one[2].c_str());
		map_achieveTypeCount[atoi(vec_one[0].c_str())] = map_one;
	}
}

void Quest::setAchieveTypeCount(int type, int count, std::string resid)
{
	map_achieveTypeCount[type][resid] += count;

	for (unsigned int i = 0; i < GlobalInstance::vec_achievedata.size(); i++)
	{
		AchieveData* data = &GlobalInstance::vec_achievedata[i];
		if (data->type == type && data->count == map_achieveTypeCount[type][resid] && data->achid.compare(resid) == 0)
		{
			data->state = DAILY_FINISHED;
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FINISHMSSION);
			break;
		}
	}
	GlobalInstance::getInstance()->saveMyAchieveData();

	std::string str;
	std::map<int, std::map<std::string, int>>::iterator mit;
	for (mit = map_achieveTypeCount.begin(); mit != map_achieveTypeCount.end(); mit++)
	{
		std::map<std::string, int>::iterator map_achdata = mit->second.begin();
		std::string key = map_achdata->first;
		std::string onestr = StringUtils::format("%d-%s-%d;", mit->first, key.c_str(), map_achdata->second);
		str.append(onestr);
	}
	DataSave::getInstance()->setAchieveTypeCount(str.substr(0, str.length() - 1));
}