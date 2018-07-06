#include "Quest.h"
#include "DataSave.h"

std::vector<TaskMainData> Quest::myFinishMainQuest;

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

int Quest::getMutexMainQuestType(int id,int type)
{

	return -1;
}

void Quest::AddFinishQuest(TaskMainData data)
{
	myFinishMainQuest.push_back(data);
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
			break;
		}
	}
	GlobalInstance::getInstance()->saveMyTaskMainData();
}
