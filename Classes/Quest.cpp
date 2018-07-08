#include "Quest.h"
#include "DataSave.h"
#include "CommonFuncs.h"

std::vector<TaskMainData> Quest::myFinishMainQuest;

std::map<std::string, int> Quest::map_NpcQuestRes;

bool Quest::initFinishTaskData()
{
	for (int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		TaskMainData data = GlobalInstance::vec_TaskMain[i];
		if (data.isfinish >= MAIN_FINISH)
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
		for (int i = 0; i < vec_tmp.size(); i++)
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

void Quest::saveData()
{
	std::string str;
	for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		if (GlobalInstance::myCurMainData.id == GlobalInstance::vec_TaskMain[i].id)
		{
			GlobalInstance::vec_TaskMain[i].isfinish = GlobalInstance::myCurMainData.isfinish;
			GlobalInstance::vec_TaskMain[i].finishtype = GlobalInstance::myCurMainData.finishtype;
			AddFinishQuest(GlobalInstance::myCurMainData);

			GlobalInstance::myCurMainData = GlobalInstance::vec_TaskMain[i + 1];//��ǰ������һ��
			break;
		}
	}
	GlobalInstance::getInstance()->saveMyTaskMainData();
}

bool Quest::getMutexMainQuestType(int id, int type)
{
	for (int i = 0; i < myFinishMainQuest.size(); i++)
	{
		TaskMainData data = myFinishMainQuest[i];
		if (data.finishtype == MAIN_GIVE)
		{
			if (data.mutex1.size() > 0)
			{
				int fid = data.mutex1[0];
				int ftype = data.mutex1[1];
				//�жϻ���1���Ƿ��е�ǰ����id
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
				//�жϻ���2���Ƿ��е�ǰ����id
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
	if (GlobalInstance::myCurMainData.isfinish == MAIN_TASK && npcid.compare(GlobalInstance::myCurMainData.npcid) == 0)
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
	for (int i = 0; i < GlobalInstance::myCurMainData.need1.size(); i++)
	{
		std::map<std::string, int> one_res = GlobalInstance::myCurMainData.need1[i];
		std::map<std::string, int>::iterator oneit = one_res.begin();
		std::string cresid = oneit->first;
		if (resid.compare(cresid) == 0)
		{
			map_NpcQuestRes[resid] += count;
		}
	}

	std::string curstr;
	std::map<std::string, int>::iterator it;
	for (it = map_NpcQuestRes.begin(); it != map_NpcQuestRes.end(); it++)
	{
		std::string onestr = StringUtils::format("%s-%d;", it->first, map_NpcQuestRes[it->first]);
		curstr.append(onestr);
	}
	DataSave::getInstance()->setMyCurTaskNeed(curstr);
}

bool Quest::getResCountFinish()
{
	if (GlobalInstance::myCurMainData.isfinish == MAIN_TASK)
	{
		return false;
	}
	int fcount = 0;
	for (int i = 0; i < GlobalInstance::myCurMainData.need1.size(); i++)
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