#include "GlobalInstance.h"
#include "DataSave.h"
#include "Hero.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MyRes.h"
#include "Const.h"
#include "Quest.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

std::vector<Hero*> GlobalInstance::vec_myHeros;
std::vector<Hero*> GlobalInstance::vec_rand3Heros;

std::vector<S_HeroAttr> GlobalInstance::vec_herosAttr;
std::vector<ResCreator*> GlobalInstance::vec_resCreators;

std::map<std::string, AllResources> GlobalInstance::map_AllResources;
std::map<std::string, EquipData> GlobalInstance::map_Equip;
std::map<std::string, GFData> GlobalInstance::map_GF;

std::map<std::string, EquipSuit> GlobalInstance::map_EquipSuit;

std::map<int, NPCAttrData> GlobalInstance::map_NpcAttrData;
std::map<std::string, NPCData> GlobalInstance::map_Npcs;

std::vector<TaskData> GlobalInstance::vec_TaskMain;
TaskData GlobalInstance::myCurMainData;

std::vector<TaskData> GlobalInstance::vec_TaskBranch;
TaskData GlobalInstance::myCurBranchData;

std::map<std::string, DailyTaskData> GlobalInstance::map_DTdata;

std::map<std::string, EventData> GlobalInstance::map_eventdata;//事件宝箱概率数据

std::vector<AchieveData> GlobalInstance::vec_achievedata;

std::vector<ShopData> GlobalInstance::vec_shopdata;
std::map<std::string, int> GlobalInstance::map_buyVipDays;
std::vector<std::string> GlobalInstance::vec_buyVipIds;

std::map<std::string, NpcRelationData> GlobalInstance::map_npcrelation;
std::map<std::string, NpcFriendly> GlobalInstance::map_myfriendly;
int GlobalInstance::npcmasterfinish = 0;

int GlobalInstance::servertime = 0;
int GlobalInstance::refreshHeroTime = 0;
int GlobalInstance::refreshResTime = 0;

int GlobalInstance::refreshMarketTime = 0;

int GlobalInstance::totalFarmercount = 0;

Hero* GlobalInstance::myCardHeros[6];

std::map<std::string, S_MainMap> GlobalInstance::map_mapsdata;

std::map<std::string, TBoxData> GlobalInstance::map_TBoxs;

std::vector<MessageData> GlobalInstance::vec_messsages;
std::vector<MessageData> GlobalInstance::vec_notice;

DynamicValueInt GlobalInstance::mySoliverCount;
DynamicValueInt GlobalInstance::myCoinCount;

std::string GlobalInstance::myID;
std::string GlobalInstance::myNickName;

int GlobalInstance::unlockchapter = 1;

std::string GlobalInstance::noticeID = "";

int GlobalInstance::myOutMapCarry = 100;

int GlobalInstance::silverRefHeroCount = 0;

int GlobalInstance::resetSilverRefHeroCountTime = 0;

bool GlobalInstance::isNewHeroRefresh = false;

std::string GlobalInstance::qq;

bool GlobalInstance::isCheat = false;

S_TimeGift GlobalInstance::serverTimeGiftData;

GlobalInstance::GlobalInstance()
{

}
GlobalInstance::~GlobalInstance()
{

}

GlobalInstance* GlobalInstance::_Context = NULL;

GlobalInstance* GlobalInstance::getInstance() {
	if (_Context == NULL) {
		_Context = new GlobalInstance;
	}
	return _Context;
}

std::string GlobalInstance::UUID()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return getDeviceIDInKeychain();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "865499033926005-********************";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	
	if (JniHelper::getStaticMethodInfo(methodInfo, ANDOIRJNICLSNAME, "UUID", "()Ljava/lang/String;"))
	{
		jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = methodInfo.env->GetStringUTFChars(jstr, 0);
	}
	return ret;
#endif
}

std::string GlobalInstance::getVersionCode()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return getvercode();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "1.0.0";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	if (JniHelper::getStaticMethodInfo(methodInfo, ANDOIRJNICLSNAME, "getVersion", "()Ljava/lang/String;"))
	{
		jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = methodInfo.env->GetStringUTFChars(jstr, 0);
	}
	return ret;
#endif
}

std::string GlobalInstance::getPackageName()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return getbundleid();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "com.kuxx.jh";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	if (JniHelper::getStaticMethodInfo(methodInfo, ANDOIRJNICLSNAME, "getPkgName", "()Ljava/lang/String;"))
	{
		jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = methodInfo.env->GetStringUTFChars(jstr, 0);
	}
	return ret;
#endif
}

std::string GlobalInstance::getChannelId()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return getbundleid();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "win32";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	if (JniHelper::getStaticMethodInfo(methodInfo, ANDOIRJNICLSNAME, "getChannelID", "()Ljava/lang/String;"))
	{
		jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = methodInfo.env->GetStringUTFChars(jstr, 0);
	}
	return ret;
#endif
}

std::string GlobalInstance::getPlatForm()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return "IOS";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "WIN32";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return "ANDROID";
#endif
}


int GlobalInstance::getSysSecTime()
{
	time_t timep;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	time(&timep);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	timep = tv.tv_sec;
#endif
	return timep;
}

int GlobalInstance::createRandomNum(int val)
{
	int syssec = GlobalInstance::getSysSecTime();
	int static randNum = 0;
	randNum += 3600 * 24;
	syssec += randNum;
	if (randNum < 0 || randNum > INT32_MAX)
	{
		syssec = 0;
		randNum = 0;
	}
	srand(syssec);
	int r = rand() % val;
	return r;
}


void GlobalInstance::loadInitData()
{
	refreshHeroTime = DataSave::getInstance()->getRefreshHeroTime();

	int langtype = DataSave::getInstance()->getLocalLang();
	GlobalInstance::getInstance()->setLang(langtype);

	refreshResTime = DataSave::getInstance()->getRefreshResTime();

	totalFarmercount = DataSave::getInstance()->getTotalFarmers();

	refreshMarketTime = DataSave::getInstance()->getRefreshMarketTime();

	unlockchapter = DataSave::getInstance()->getUnlockChapter();

	for (int i = 0; i < 6; i++)
	{
		myCardHeros[i] = NULL;
	}

	GlobalInstance::mySoliverCount.setValue(DataSave::getInstance()->getMySoliverCount());
	GlobalInstance::myCoinCount.setValue(DataSave::getInstance()->getMyCoinCount());

	silverRefHeroCount = DataSave::getInstance()->getSilverRefHeroCount();

	serverTimeGiftData.isopen = false;
	serverTimeGiftData.lefttime = 0;
	serverTimeGiftData.turn = 0;
}

void GlobalInstance::saveMyHeros()
{
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		saveHeroByIndex(i);
	}
}

void GlobalInstance::saveHero(Hero* hero)
{
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		if (hero->getName().compare(GlobalInstance::vec_myHeros[i]->getName()) == 0)
		{
			saveHeroByIndex(i);
			break;
		}
	}
}

void GlobalInstance::saveHeroByIndex(int index)
{
	std::string herokey = StringUtils::format("hero%d", index);
	Hero* hero = GlobalInstance::vec_myHeros[index];
	std::string datastr = StringUtils::format("%s-%d-%d-%d-%d-%.2f-%d-%d-%.2f-%d-%d-%d-%d-%d;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex(), hero->getRandAttr(), hero->getState(), hero->getPos(), hero->getHp(), 
		hero->getTrainHour(), hero->getTrainTime(), hero->getPower().getValue(), hero->getPowerTime(), hero->getChangeCount());
	DataSave::getInstance()->setHeroData(herokey, datastr);
}

void GlobalInstance::loadMyHeros()
{
	for (int i = 0; i < 50; i++)
	{
		std::string herokey = StringUtils::format("hero%d", i);
		std::string herodatastr = DataSave::getInstance()->getHeroData(herokey);
		if (herodatastr.length() > 0)
		{
			Hero* hero = NULL;
			std::vector<std::string> vec_retstr;
			CommonFuncs::split(herodatastr, vec_retstr, ";");
			if (vec_retstr.size() > 0)//人物属性
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(vec_retstr[0], vec_tmp, "-");
				if (vec_tmp.size() >= 14)
				{
					hero = new Hero();
					hero->setName(vec_tmp[0]);
					DynamicValueInt expv;
					expv.setValue(atoi(vec_tmp[1].c_str()));
					hero->setExp(expv);
					hero->setVocation(atoi(vec_tmp[2].c_str()));
					hero->setPotential(atoi(vec_tmp[3].c_str()));
					hero->setSex(atoi(vec_tmp[4].c_str()));
					hero->setRandAttr(atof(vec_tmp[5].c_str()));
					int state = atoi(vec_tmp[6].c_str());
					hero->setState(state);
					int pos = atoi(vec_tmp[7].c_str());
					hero->setPos(pos);
					if (pos > 0 && state != HS_DEAD)
					{
						GlobalInstance::myCardHeros[pos - 1] = hero;
					}

					hero->setHp(atof(vec_tmp[8].c_str()));
					hero->setTrainHour(atoi(vec_tmp[9].c_str()));
					hero->setTrainTime(atoi(vec_tmp[10].c_str()));
					DynamicValueInt pdv;
					pdv.setValue(atoi(vec_tmp[11].c_str()));
					hero->setPower(pdv);
					hero->setPowerTime(atoi(vec_tmp[12].c_str()));
					hero->setChangeCount(atoi(vec_tmp[13].c_str()));
				}
			}
			if (vec_retstr.size() > 1)//装备属性
			{
				if (hero != NULL)
				{

				}
			}
			if (hero != NULL)
				GlobalInstance::vec_myHeros.push_back(hero);
		}
	}
}

void GlobalInstance::saveRand3Heros()
{
	std::string str;
	for (unsigned int i = 0; i < GlobalInstance::vec_rand3Heros.size(); i++)
	{
		Hero* hero = GlobalInstance::vec_rand3Heros[i];
		std::string onestr = StringUtils::format("%s-%d-%d-%d-%d-%.02f-%d-%d-%d;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex(), hero->getRandAttr(), hero->getState(), hero->getTrainHour(), hero->getTrainTime());
		str.append(onestr);

	}
	DataSave::getInstance()->setRand3HeroData(str.substr(0, str.length() - 1));
}

void GlobalInstance::loadRand3Heros()
{

	std::string herodatastr = DataSave::getInstance()->getRand3HeroData();
	if (herodatastr.length() > 0)
	{
		Hero* hero = NULL;
		std::vector<std::string> vec_retstr;
		CommonFuncs::split(herodatastr, vec_retstr, ";");

		for (unsigned int i = 0; i < vec_retstr.size(); i++)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(vec_retstr[i], vec_tmp, "-");
			if (vec_tmp.size() >= 7)
			{
				hero = new Hero();
				hero->setName(vec_tmp[0]);
				DynamicValueInt expv;
				expv.setValue(atoi(vec_tmp[1].c_str()));
				hero->setExp(expv);
				hero->setVocation(atoi(vec_tmp[2].c_str()));
				hero->setPotential(atoi(vec_tmp[3].c_str()));
				hero->setSex(atoi(vec_tmp[4].c_str()));
				hero->setRandAttr(atof(vec_tmp[5].c_str()));
				hero->setState(atoi(vec_tmp[6].c_str()));
			}
			if (hero != NULL)
				GlobalInstance::vec_rand3Heros.push_back(hero);
		}
	}
}

void GlobalInstance::loadHerosAttrData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/heroatr.json"));
	rapidjson::Value& allData = doc["h"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			S_HeroAttr data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["exp"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_exp.push_back(v[m].GetInt());
			}

			v = jsonvalue["atk"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_atk.push_back(v[m].GetDouble());
			}

			v = jsonvalue["df"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_df.push_back(v[m].GetDouble());
			}

			v = jsonvalue["maxhp"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_maxhp.push_back(v[m].GetDouble());
			}

			v = jsonvalue["avoid"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_avoid.push_back(v[m].GetDouble());
			}
			v = jsonvalue["crit"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_crit.push_back(v[m].GetDouble());
			}

			v = jsonvalue["speed"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_atkspeed.push_back(v[m].GetDouble());
			}

			vec_herosAttr.push_back(data);
		}
	}
}

void GlobalInstance::saveRefreshHeroTime(int time)
{
	refreshHeroTime = time;
	DataSave::getInstance()->setRefreshHeroTime(time);
}


int GlobalInstance::getRefreshHeroTime()
{
	return refreshHeroTime;
}

bool GlobalInstance::checkifSameName(std::string nickname)
{
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		if (nickname.compare(GlobalInstance::vec_myHeros[i]->getName()) == 0)
		{
			return true;
		}
	}
	for (unsigned int i = 0; i < GlobalInstance::vec_rand3Heros.size(); i++)
	{
		if (nickname.compare(GlobalInstance::vec_rand3Heros[i]->getName()) == 0)
		{
			return true;
		}
	}
	return false;
}

void GlobalInstance::saveRefreshResTime(int time)
{
	refreshResTime = time;
	DataSave::getInstance()->setRefreshResTime(time);
}


int GlobalInstance::getRefreshResTime()
{
	return refreshResTime;
}

void GlobalInstance::loadEventData()
{
	/*rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/event.json"));
	rapidjson::Value& allData = doc["b"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			EventData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["pr"];
			data.pr = atoi(v.GetString());

			v = jsonvalue["max"];
			data.max = atoi(v.GetString());

			v = jsonvalue["min"];
			data.min = atoi(v.GetString());

			map_eventdata[data.id] = data;
		}
	}*/
}

void GlobalInstance::loadAchieveData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/achieve.json"));
	rapidjson::Value& allData = doc["b"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			AchieveData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["type"];
			data.type = atoi(v.GetString());

			v = jsonvalue["achid"];
			data.achid = v.GetString();

			v = jsonvalue["count"];
			data.count = atoi(v.GetString());

			v = jsonvalue["rewards"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string rewards = v[i].GetString();
				if (rewards.length() > 5)
				{
					std::vector<std::string> vecstr;
					CommonFuncs::split(rewards, vecstr, "-");
					data.rewards.push_back(vecstr);
				}
			}

			data.state = DAILY_UNFINISHED;

			vec_achievedata.push_back(data);
		}
	}
}

void GlobalInstance::getMyAchieveData()
{
	std::string str = DataSave::getInstance()->getMyAchieveData();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			for (unsigned int i = 0; i < vec_achievedata.size(); i++)
			{
				if (vec_achievedata[i].id.compare(vec_one[0]) == 0)
				{
					vec_achievedata[i].state = atoi(vec_one[1].c_str());
				}
			}
		}
	}

	str = DataSave::getInstance()->getAchieveTypeCount();
	if (str.length()>0)
	{
		Quest::initAchieveTypeCount(str);
	}
}

void GlobalInstance::saveMyAchieveData()
{
	std::string str;
	for (unsigned int i = 0; i < vec_achievedata.size(); i++)
	{
		AchieveData data = vec_achievedata[i];
		if (data.state >= DAILY_FINISHED)
		{
			std::string onestr = StringUtils::format("%d-%d;", atoi(data.id.c_str()), data.state);
			str.append(onestr);
		}
	}
	if (str.length() > 0)
	{
		DataSave::getInstance()->setMyAchieveData(str.substr(0, str.length() - 1));
	}
}

void GlobalInstance::loadResCreatorData()
{
	std::string str = DataSave::getInstance()->getResCreatorData();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < 5; i++)
		{
			if (i < vec_tmp.size())
			{
				std::vector<std::string> vec_one;
				CommonFuncs::split(vec_tmp[i], vec_one, "-");
				std::string cid = StringUtils::format("r%03d", i + 1);
				ResCreator* creator = new ResCreator(cid);
				DynamicValueInt dlv;
				dlv.setValue(atoi(vec_one[0].c_str()));
				creator->setLv(dlv);
				DynamicValueInt dcount;
				dcount.setValue(atoi(vec_one[1].c_str()));
				creator->setFarmersCount(dcount);
				GlobalInstance::vec_resCreators.push_back(creator);
			}
			else
			{
				std::string cid = StringUtils::format("r%03d", i + 1);
				ResCreator* creator = new ResCreator(cid);
				GlobalInstance::vec_resCreators.push_back(creator);
			}
		}
	}
}

void GlobalInstance::loadTaskMainData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/maintask.json"));
	rapidjson::Value& allData = doc["sq"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			TaskData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = atoi(v.GetString());

			v = jsonvalue["title"];
			data.name = v.GetString();

			v = jsonvalue["type"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.type.push_back(v[m].GetInt());
			}

			v = jsonvalue["desc"];
			data.desc = v.GetString();

			v = jsonvalue["place"];
			data.place = v.GetString();

			v = jsonvalue["npc"];
			data.npcid = v.GetString();

			v = jsonvalue["bossword"];
			data.bossword = v.GetString();

			v = jsonvalue["need1"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 3)
				{
					std::map<std::string, int> map_tmp;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					map_tmp[vec_tmp[0]] = atoi(vec_tmp[1].c_str());
					data.need1.push_back(map_tmp);
				}
			}

			v = jsonvalue["need1desc"];
			data.need1desc = v.GetString();

			v = jsonvalue["need2"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 3)
				{
					std::map<std::string, int> map_tmp;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					map_tmp[vec_tmp[0]] = atoi(vec_tmp[1].c_str());
					data.need2.push_back(map_tmp);
				}
			}

			v = jsonvalue["need2desc"];
			data.need2desc = v.GetString();

			v = jsonvalue["mutex1"];
			std::string str = v.GetString();
			if (str.length() > 1)
			{
				std::string onestr = v.GetString();
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(onestr, vec_tmp, "-");
				data.mutex1.push_back(atoi(vec_tmp[0].c_str()));
				data.mutex1.push_back(atoi(vec_tmp[1].c_str()));
			}

			v = jsonvalue["mutex2"];
			str = v.GetString();
			if (str.length() > 1)
			{
				std::string onestr = v.GetString();
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(onestr, vec_tmp, "-");
				data.mutex2.push_back(atoi(vec_tmp[0].c_str()));
				data.mutex2.push_back(atoi(vec_tmp[1].c_str()));
			}

			v = jsonvalue["reward1"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 5)
				{
					std::vector<std::string> vec;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					for (unsigned int j = 0; j < vec_tmp.size(); j++)
					{
						vec.push_back(vec_tmp[j]);
					}
					data.reward1.push_back(vec);
				}
			}

			v = jsonvalue["reward2"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 5)
				{
					std::vector<std::string> vec;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					for (unsigned int j = 0; j < vec_tmp.size(); j++)
					{
						vec.push_back(vec_tmp[j]);
					}
					data.reward2.push_back(vec);
				}
			}

			v = jsonvalue["bossword1"];
			data.bossword1 = v.GetString();
			v = jsonvalue["bossword2"];
			data.bossword2 = v.GetString();

			v = jsonvalue["fight1"]; 
			data.isFight1 = atoi(v.GetString());
			v = jsonvalue["fight2"];
			data.isFight2 = atoi(v.GetString());
			
			data.isfinish = 1;//默认未接受任务
			data.finishtype = 0;

			vec_TaskMain.push_back(data);
		}
	}
}

void GlobalInstance::loadMyTaskMainData()
{
	std::string str = DataSave::getInstance()->getMyMainTask();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			if (vec_TaskMain[i].id == atoi(vec_one[0].c_str()))
			{
				vec_TaskMain[i].isfinish = atoi(vec_one[1].c_str());
				vec_TaskMain[i].finishtype = atoi(vec_one[2].c_str());
			}
		}

		Quest::initFinishTaskData();

		if (vec_TaskMain[vec_tmp.size()-1].isfinish == QUEST_ACC)
		{
			myCurMainData = vec_TaskMain[vec_tmp.size()-1];
			Quest::initCurBranchNeedData();
		}
		else if (vec_TaskMain[vec_tmp.size() - 1].isfinish >= QUEST_FINISH)
		{
			if (vec_tmp.size() < GlobalInstance::vec_TaskMain.size())
			{
				myCurMainData = vec_TaskMain[vec_tmp.size()];
			}
			else
			{
				myCurMainData = vec_TaskMain[vec_tmp.size() - 1];
			}
		}
		
	}
	else
	{
		myCurMainData = vec_TaskMain[0];
	}
}

void GlobalInstance::saveMyTaskMainData()
{
	std::string str;
	std::string mystr = "";
	sort(vec_TaskMain.begin(), vec_TaskMain.end(), larger_callback);
	for (unsigned i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		TaskData data = GlobalInstance::vec_TaskMain[i];
		if (data.isfinish >= QUEST_FINISH)
		{
			std::string onestr = StringUtils::format("%d-%d-%d;", data.id, data.isfinish, data.finishtype);
			str.append(onestr);
		}
		if (data.isfinish == QUEST_ACC)
		{
			mystr = StringUtils::format("%d-%d-%d;", data.id, data.isfinish, data.finishtype);
		}
	}
	str.append(mystr);
	if (str.length() > 0)
	{
		DataSave::getInstance()->setMyMainTask(str.substr(0, str.length() - 1));
	}
}

void GlobalInstance::loadTaskBranchData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/branchtask.json"));
	rapidjson::Value& allData = doc["sq"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			TaskData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = atoi(v.GetString());

			v = jsonvalue["title"];
			data.name = v.GetString();

			v = jsonvalue["type"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.type.push_back(v[m].GetInt());
			}

			v = jsonvalue["desc"];
			data.desc = v.GetString();

			v = jsonvalue["place"];
			data.place = v.GetString();

			v = jsonvalue["npc"];
			data.npcid = v.GetString();

			v = jsonvalue["bossword"];
			data.bossword = v.GetString();

			v = jsonvalue["need1"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 3)
				{
					std::map<std::string, int> map_tmp;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					map_tmp[vec_tmp[0]] = atoi(vec_tmp[1].c_str());
					data.need1.push_back(map_tmp);
				}
			}

			v = jsonvalue["need1desc"];
			data.need1desc = v.GetString();

			v = jsonvalue["need2"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 3)
				{
					std::map<std::string, int> map_tmp;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					map_tmp[vec_tmp[0]] = atoi(vec_tmp[1].c_str());
					data.need2.push_back(map_tmp);
				}
			}

			v = jsonvalue["need2desc"];
			data.need2desc = v.GetString();

			v = jsonvalue["mutex1"];
			std::string str = v.GetString();
			if (str.length() > 1)
			{
				std::string onestr = v.GetString();
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(onestr, vec_tmp, "-");
				data.mutex1.push_back(atoi(vec_tmp[0].c_str()));
				data.mutex1.push_back(atoi(vec_tmp[1].c_str()));
			}

			v = jsonvalue["mutex2"];
			str = v.GetString();
			if (str.length() > 1)
			{
				std::string onestr = v.GetString();
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(onestr, vec_tmp, "-");
				data.mutex2.push_back(atoi(vec_tmp[0].c_str()));
				data.mutex2.push_back(atoi(vec_tmp[1].c_str()));
			}

			v = jsonvalue["reward1"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 5)
				{
					std::vector<std::string> vec;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					for (unsigned int j = 0; j < vec_tmp.size(); j++)
					{
						vec.push_back(vec_tmp[j]);
					}
					data.reward1.push_back(vec);
				}
			}

			v = jsonvalue["reward2"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 5)
				{
					std::vector<std::string> vec;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					for (unsigned int j = 0; j < vec_tmp.size(); j++)
					{
						vec.push_back(vec_tmp[j]);
					}
					data.reward2.push_back(vec);
				}
			}

			v = jsonvalue["bossword1"];
			data.bossword1 = v.GetString();
			v = jsonvalue["bossword2"];
			data.bossword2 = v.GetString();

			v = jsonvalue["fight1"];
			data.isFight1 = atoi(v.GetString());
			v = jsonvalue["fight2"];
			data.isFight2 = atoi(v.GetString());

			data.isfinish = 1;//默认未接受任务
			data.finishtype = 0;

			vec_TaskBranch.push_back(data);
		}
	}
}

void GlobalInstance::loadMyTaskBranchData()
{
	std::string str = DataSave::getInstance()->getMyBranchTask();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			if (vec_TaskBranch[i].id == atoi(vec_one[0].c_str()))
			{
				vec_TaskBranch[i].isfinish = atoi(vec_one[1].c_str());
				vec_TaskBranch[i].finishtype = atoi(vec_one[2].c_str());
			}
		}

		Quest::initFinishTaskBranchData();
		if (vec_TaskBranch[vec_tmp.size() - 1].isfinish == QUEST_ACC)
		{
			myCurBranchData = vec_TaskBranch[vec_tmp.size() - 1];
		}
		else if (vec_TaskBranch[vec_tmp.size() - 1].isfinish >= QUEST_FINISH)
		{
			myCurBranchData = vec_TaskBranch[vec_tmp.size() - 1];
		}

	}
	else
	{
		myCurBranchData = vec_TaskBranch[0];
	}
}

void GlobalInstance::saveMyTaskBranchData()
{
	std::string str;
	std::string mystr = "";
	sort(vec_TaskBranch.begin(), vec_TaskBranch.end(), larger_branchcallback);
	for (unsigned i = 0; i < GlobalInstance::vec_TaskBranch.size(); i++)
	{
		TaskData data = GlobalInstance::vec_TaskBranch[i];
		if (data.isfinish >= QUEST_FINISH)
		{
			std::string onestr = StringUtils::format("%d-%d-%d;", data.id, data.isfinish, data.finishtype);
			str.append(onestr);
		}
		if (data.isfinish == QUEST_ACC)
		{
			mystr = StringUtils::format("%d-%d-%d;", data.id, data.isfinish, data.finishtype);
		}
	}
	str.append(mystr);
	if (str.length() > 0)
	{
		DataSave::getInstance()->setMyBranchTask(str.substr(0, str.length() - 1));
	}
}

void GlobalInstance::loadDailyTaskData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/dailytask.json"));
	rapidjson::Value& allData = doc["sq"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			DailyTaskData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["type"];
			data.type = atoi(v.GetString());

			v = jsonvalue["count"];
			data.count = atoi(v.GetString());

			v = jsonvalue["rewards"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 3)
				{
					std::vector<std::string> vec;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					for (unsigned int j = 0; j < vec_tmp.size(); j++)
					{
						vec.push_back(vec_tmp[j]);
					}
					data.goods.push_back(vec);
				}
			}

			v = jsonvalue["points"];
			data.points = atoi(v.GetString());

			data.state = DAILY_UNFINISHED;

			map_DTdata[data.id] = data;
		}
	}
}

void GlobalInstance::getMyDailyTaskData()
{
	std::string str = DataSave::getInstance()->getMyyDailyTaskData();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			if (map_DTdata[vec_one[0]].id.compare(vec_one[0]) == 0)
			{
				map_DTdata[vec_one[0]].state = atoi(vec_one[1].c_str());
			}
		}
	}

	str = DataSave::getInstance()->getDailyTypeCount();
	if (str.length()>0)
	{
		Quest::initDailyTypeCount(str);
	}
	str = DataSave::getInstance()->getMyyDailyReward();
	if (str.length()>0)
	{
		Quest::initDailyPointReward(str);
	}
}

void GlobalInstance::saveMyDailyTaskData()
{
	std::string str;
	std::map<std::string, DailyTaskData>::iterator it;
	for (it = map_DTdata.begin(); it != map_DTdata.end(); it++)
	{
		DailyTaskData data = map_DTdata[it->first];
		if (data.state >= DAILY_FINISHED)
		{
			std::string onestr = StringUtils::format("%d-%d;", atoi(data.id.c_str()), data.state);
			str.append(onestr);
		}
	}
	if (str.length() > 0)
	{
		DataSave::getInstance()->setMyDailyTaskData(str.substr(0, str.length() - 1));
	}
}

bool GlobalInstance::larger_callback(TaskData a, TaskData b)
{
	int idA = a.id;
	int idB = b.id;
	if (idA < idB)
		return true;
	else
		return false;
}

bool GlobalInstance::larger_branchcallback(TaskData a, TaskData b)
{
	int idA = a.id;
	int idB = b.id;
	if (idA < idB)
		return true;
	else
		return false;
}

void GlobalInstance::saveResCreatorData()
{
	std::string str;
	for (unsigned i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
	{
		if (GlobalInstance::vec_resCreators[i]->getLv().getValue() >= 0)
		{
			std::string onestr = StringUtils::format("%d-%d;", GlobalInstance::vec_resCreators[i]->getLv().getValue(), GlobalInstance::vec_resCreators[i]->getFarmersCount().getValue());
			str.append(onestr);
		}
	}
	DataSave::getInstance()->setResCreatorData(str.substr(0,str.length() - 1));	
}

void GlobalInstance::loadEquipData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/equip.json"));
	rapidjson::Value& allData = doc["g"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			EquipData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["maxhp"];
			data.maxhp = atoi(v.GetString());

			v = jsonvalue["atk"];
			data.atk = atoi(v.GetString());

			v = jsonvalue["df"];
			data.df = atoi(v.GetString());

			v = jsonvalue["avoid"];
			data.avoid = atof(v.GetString());

			v = jsonvalue["crit"];
			data.crit = atof(v.GetString());

			v = jsonvalue["speed"];
			data.speed = atof(v.GetString());

			v = jsonvalue["bns"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_bns.push_back(v[m].GetFloat());
			}

			map_Equip[data.id] = data;
		}
	}
}

void GlobalInstance::loadGFData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/gf.json"));
	rapidjson::Value& allData = doc["g"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			GFData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["qu"];
			data.qu = atoi(v.GetString());

			v = jsonvalue["skill"];
			std::string skillstr = v.GetString();
			data.skill = atoi(skillstr.substr(2).c_str());

			v = jsonvalue["skillbns"];

			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_skillbns.push_back(v[m].GetInt());
			}

			v = jsonvalue["rnd"];
			data.skillrnd = atoi(v.GetString());

			v = jsonvalue["skef"];
			data.skilleff1 = atof(v.GetString());

			v = jsonvalue["skef1"];
			data.skilleff2 = atoi(v.GetString());
			
			v = jsonvalue["maxhp"];
			for (unsigned  int m = 0; m < v.Size(); m++)
			{
				data.vec_hp.push_back(v[m].GetInt());
			}

			v = jsonvalue["atk"];
			for (unsigned  int m = 0; m < v.Size(); m++)
			{
				data.vec_atk.push_back(v[m].GetInt());
			}

			v = jsonvalue["df"];
			for (unsigned  int m = 0; m < v.Size(); m++)
			{
				data.vec_df.push_back(v[m].GetInt());
			}

			v = jsonvalue["avoid"];
			for (unsigned  int m = 0; m < v.Size(); m++)
			{
				data.vec_avoid.push_back(v[m].GetFloat());
			}

			v = jsonvalue["crit"];
			for (unsigned  int m = 0; m < v.Size(); m++)
			{
				data.vec_crit.push_back(v[m].GetFloat());
			}

			v = jsonvalue["speed"];
			for (unsigned  int m = 0; m < v.Size(); m++)
			{
				data.vec_speed.push_back(v[m].GetFloat());
			}

			v = jsonvalue["bns"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_herobns.push_back(v[m].GetFloat());
			}

			v = jsonvalue["exp"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_exp.push_back(v[m].GetInt());
			}

			map_GF[data.id] = data;
		}
	}
}

void GlobalInstance::loadNpcData()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/npcatr.json"));
	rapidjson::Value& allData = doc["n"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			NPCAttrData data;
			rapidjson::Value& v = jsonvalue["vocation"];
			data.vocation = atoi(v.GetString());

			v = jsonvalue["maxhp"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_maxhp.push_back(v[m].GetInt());
			}

			v = jsonvalue["atk"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_atk.push_back(v[m].GetInt());
			}

			v = jsonvalue["df"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_df.push_back(v[m].GetInt());
			}

			v = jsonvalue["avoid"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_avoid.push_back(v[m].GetFloat());
			}

			v = jsonvalue["crit"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_crit.push_back(v[m].GetFloat());
			}

			v = jsonvalue["speed"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_speed.push_back(v[m].GetFloat());
			}

			v = jsonvalue["exp"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_bnsexp.push_back(v[m].GetFloat());
			}

			map_NpcAttrData[data.vocation] = data;
		}
	}

	rapidjson::Document docnpc = ReadJsonFile(ResourcePath::makePath("json/npc.json"));
	rapidjson::Value& allNpcs = docnpc["n"];
	for (unsigned int i = 0; i < allNpcs.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allNpcs[i];
		if (jsonvalue.IsObject())
		{
			NPCData data;
			rapidjson::Value& v = jsonvalue["vocation"];
			data.vocation = atoi(v.GetString());

			v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["nicon"];
			data.icon = v.GetString();

			map_Npcs[data.id] = data;
		}
	}
}

void GlobalInstance::loadAllResourcesData()
{
	int langtype = DataSave::getInstance()->getLocalLang();
	std::string langname = StringUtils::format("lang/%s/allresdesc.json", ResourceLang::makeLangName(langtype).c_str());
	rapidjson::Document rddoc = ReadJsonFile(ResourcePath::makePath(langname));
	rapidjson::Value& rdallData = rddoc["rd"];
	for (unsigned int i = 0; i < rdallData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = rdallData[i];
		if (jsonvalue.IsObject())
		{
			AllResources data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["cname"];
			data.name = v.GetString();

			if (jsonvalue.HasMember("desc"))
			{
				v = jsonvalue["desc"];
				data.desc = v.GetString();
			}

			map_AllResources[data.id] = data;
		}
	}

	rapidjson::Document rsdoc = ReadJsonFile(ResourcePath::makePath("json/resaction.json"));
	rapidjson::Value& rsallData = rsdoc["r"];
	for (unsigned int i = 0; i < rsallData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = rsallData[i];
		if (jsonvalue.IsObject())
		{
			rapidjson::Value& v = jsonvalue["id"];
			std::string rid = v.GetString();

			if (jsonvalue.HasMember("nres"))
			{
				v = jsonvalue["nres"];
				for (unsigned int j = 0;j < v.Size(); j++)
				{
					std::string onestr = v[j].GetString();
					if (onestr.length() > 3)
					{
						std::map<std::string, int> map_tmp;
						std::vector<std::string> vec_tmp;
						CommonFuncs::split(onestr, vec_tmp, "-");
						map_tmp[vec_tmp[0]] = atoi(vec_tmp[1].c_str());
						map_AllResources[rid].vec_needres.push_back(map_tmp);
					}

				}
			}
			if (jsonvalue.HasMember("sale"))
			{
				v = jsonvalue["sale"];
				map_AllResources[rid].silverval = atoi(v.GetString());
			}
			if (jsonvalue.HasMember("coin"))
			{
				v = jsonvalue["coin"];
				map_AllResources[rid].coinval = atoi(v.GetString());
			}
		}
	}
}

void GlobalInstance::loadMyResData()
{
	std::string str = DataSave::getInstance()->getMyRes();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			std::string rid = vec_one[0];

			int m = 0;
			for (; m < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); m++)
			{
				if (rid.compare(0, 1, RES_TYPES_CHAR[m]) == 0)
					break;
			}
			if (m >= T_ARMOR && m <= T_FASHION)
			{
				Equip* res = new Equip();
				res->setId(rid);
				res->setType(m);
				DynamicValueInt dv;
				dv.setValue(atoi(vec_one[1].c_str()));
				res->setCount(dv);
				res->setWhere(atoi(vec_one[2].c_str()));

				DynamicValueInt dv1;
				dv1.setValue(atoi(vec_one[3].c_str()));
				res->setQU(dv1);

				DynamicValueInt dv2;
				dv2.setValue(atoi(vec_one[4].c_str()));
				res->setLv(dv2);

				CommonFuncs::split(vec_one[5], res->vec_stones, ",");

				res->setWhos(vec_one[6]);

				MyRes::vec_MyResources.push_back(res);
			}
			else if (m >= T_WG && m <= T_NG)
			{
				GongFa* res = new GongFa();
				res->setId(rid);
				res->setType(m);
				DynamicValueInt dv;
				dv.setValue(atoi(vec_one[1].c_str()));
				res->setCount(dv);
				res->setWhere(atoi(vec_one[2].c_str()));

				DynamicValueInt dv1;
				dv1.setValue(atoi(vec_one[3].c_str()));
				res->setQU(dv1);

				DynamicValueInt dv2;
				dv2.setValue(atoi(vec_one[4].c_str()));
				res->setExp(dv2);

				res->setWhos(vec_one[5]);

				MyRes::vec_MyResources.push_back(res);
			}

			else
			{
				ResBase* res = new ResBase();
				res->setId(rid);
				res->setType(m);
				DynamicValueInt dv;
				dv.setValue(atoi(vec_one[1].c_str()));
				res->setCount(dv);
				res->setWhere(atoi(vec_one[2].c_str()));
				MyRes::vec_MyResources.push_back(res);
			}
		}
	}
}

void GlobalInstance::loadShopData()
{
	rapidjson::Document rsdoc = ReadJsonFile(ResourcePath::makePath("json/shop.json"));
	rapidjson::Value& rsallData = rsdoc["gs"];
	for (unsigned int i = 0; i < rsallData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = rsallData[i];
		if (jsonvalue.IsObject())
		{
			ShopData data;
			rapidjson::Value& v = jsonvalue["icon"];
			data.icon = v.GetString();

			v = jsonvalue["type"];
			data.type = atoi(v.GetString());

			v = jsonvalue["name"];
			data.name = v.GetString();

			v = jsonvalue["price"];
			data.price = atoi(v.GetString());

			v = jsonvalue["desc"];
			data.desc = v.GetString();

			v = jsonvalue["count"];
			data.count = atoi(v.GetString());

			v = jsonvalue["show"];

			data.show = atoi(v.GetString()) == 1?true:false;

			v = jsonvalue["res"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 5)
				{
					std::vector<std::string> vec;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					for (unsigned int j = 0; j < vec_tmp.size(); j++)
					{
						vec.push_back(vec_tmp[j]);
					}
					data.res.push_back(vec);
				}
			}

			v = jsonvalue["paycode"];
			data.paycode = v.GetString();

			vec_shopdata.push_back(data);
		}
	}
}

void GlobalInstance::loadNpcFriendData()
{
	rapidjson::Document rsdoc = ReadJsonFile(ResourcePath::makePath("json/friend.json"));
	rapidjson::Value& rsallData = rsdoc["b"];
	for (unsigned int i = 0; i < rsallData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = rsallData[i];
		if (jsonvalue.IsObject())
		{
			NpcRelationData data;
			rapidjson::Value& v = jsonvalue["npcid"];
			data.npcid = v.GetString();

			v = jsonvalue["desc"];
			data.desc = v.GetString();

			v = jsonvalue["talk"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				data.talk.push_back(v[i].GetString());
			}

			v = jsonvalue["mytalk"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				data.mytalk.push_back(v[i].GetString());
			}

			v = jsonvalue["res"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 1)
				{
					data.res.push_back(onestr);
				}
			}

			v = jsonvalue["enemynpc"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 1)
				{
					data.enemynpc.push_back(onestr);
				}
			}

			v = jsonvalue["friendmax"];
			data.friendmax = atoi(v.GetString());

			v = jsonvalue["friendneed"];
			data.friendneed = atoi(v.GetString());

			v = jsonvalue["friendword"];
			data.friendword = v.GetString();

			v = jsonvalue["masterneed"];
			data.masterneed = atoi(v.GetString());

			v = jsonvalue["masterword"];
			data.masterword = v.GetString();

			v = jsonvalue["marryneed"];
			data.marryneed = atoi(v.GetString());

			v = jsonvalue["marryword"];
			data.marryword = v.GetString();

			v = jsonvalue["masterratio"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				float ratio = v[i].GetFloat();
				data.masterratio.push_back(ratio);
			}
			
			v = jsonvalue["conpelratio"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				float ratio = v[i].GetFloat();
				data.conpelratio.push_back(ratio);
			}

			v = jsonvalue["friendratio"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				float ratio = v[i].GetFloat();
				data.friendratio.push_back(ratio);
			}

			v = jsonvalue["fightcost"];
			data.fightcost = atoi(v.GetString());
			
			v = jsonvalue["reward"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 5)
				{
					std::vector<std::string> vec;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					for (unsigned int j = 0; j < vec_tmp.size(); j++)
					{
						vec.push_back(vec_tmp[j]);
					}
					data.reward.push_back(vec);
				}
			}
			
			map_npcrelation[data.npcid] = data;

			NpcFriendly fdata;

			fdata.npcid = data.npcid;
			fdata.friendly = 0;
			//fdata.relation = NPC_NORMOL;

			map_myfriendly[fdata.npcid] = fdata;
		}
	}
}

void GlobalInstance::loadNpcFriendly()
{
	std::string str = DataSave::getInstance()->getNpcFriendly();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			map_myfriendly[vec_one[0]].friendly = atoi(vec_one[1].c_str());
			std::vector<std::string> vec_two;
			CommonFuncs::split(vec_one[2], vec_two, ",");
			for (unsigned int i = 0; i < vec_two.size(); i++)
			{
				map_myfriendly[vec_one[0]].relation.push_back(atoi(vec_two[i].c_str()));
			}
		}
	}
}

void GlobalInstance::saveNpcFriendly()
{
	std::string str;
	std::map<std::string, NpcFriendly>::iterator it;
	for (it = map_myfriendly.begin(); it != map_myfriendly.end(); it++)
	{
		NpcFriendly data = map_myfriendly[it->first];
		if (data.friendly > 0)
		{
			std::string s;
			for (unsigned int i = 0; i < data.relation.size(); i++)
			{
				std::string s1 = StringUtils::format("%d,", data.relation[i]);
				s.append(s1);
			}
			std::string onestr = StringUtils::format("%s-%d-%s;", data.npcid.c_str(), data.friendly, s.substr(0, s.length() - 1).c_str());
			str.append(onestr);
		}
	}
	if (str.length() > 0)
	{
		DataSave::getInstance()->setNpcFriendly(str.substr(0, str.length() - 1));
	}
}

int GlobalInstance::getTotalFarmers()
{
	return totalFarmercount;
}

void GlobalInstance::saveTotalFarmers(int val)
{
	totalFarmercount = val;
	DataSave::getInstance()->setTotalFarmers(val);
}

int GlobalInstance::getWorkingFarmerCount()
{
	int count = 0;
	for (unsigned i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
	{
		count += GlobalInstance::vec_resCreators[i]->getFarmersCount().getValue();
	}
	return count;
}

void GlobalInstance::fireHero(int index)
{
	//清掉出战的数据
	for (int i = 0; i < 6; i++)
	{
		Hero* myhero = GlobalInstance::myCardHeros[i];

		if (myhero != NULL && myhero->getName().compare(GlobalInstance::vec_myHeros[index]->getName()) == 0)
		{
			GlobalInstance::myCardHeros[i] = NULL;
		}
	}


	//先删除掉所有本地英雄节点数据

	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		DataSave::getInstance()->deleteLocalHero(i);
	}
	//装备换回到仓库

	bool equipchange = false;
	for (int i = T_ARMOR; i <= T_NG; i++)
	{
		Equipable* equip = (Equipable*)MyRes::getMyPutOnResByType(i, GlobalInstance::vec_myHeros[index]->getName());
		if (equip != NULL)
		{
			equipchange = true;
			equip->setWhere(MYSTORAGE);
			equip->setWhos("");
		}
	}
	//保存装备数据
	if (equipchange)
		MyRes::saveData();
	

	//释放内存
	delete GlobalInstance::vec_myHeros[index];

	GlobalInstance::vec_myHeros[index] = NULL;
	//删除当前英雄列表
	GlobalInstance::vec_myHeros.erase(GlobalInstance::vec_myHeros.begin() + index);

	//保存数据
	GlobalInstance::getInstance()->saveMyHeros();
}

int GlobalInstance::getTotalCarry()
{
	int ret = 30;

	for (unsigned int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			ret += (GlobalInstance::myCardHeros[i]->getLevel() + 1) * 10;
		}
	}
	return ret;
}

int GlobalInstance::calcFoodMakeOut()
{
	int foodountcount = 0;
	if (GlobalInstance::vec_resCreators.size() > 0)
	{
		int needfood[] = { 2,3,4,10 };
		foodountcount = GlobalInstance::vec_resCreators[0]->getFarmersCount().getValue();
		for (unsigned int i = 1; i < GlobalInstance::vec_resCreators.size(); i++)
		{
			ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
			int count = rescreator->getFarmersCount().getValue() * needfood[i - 1];
			foodountcount -= count;
		}
	}
	return foodountcount;
}

void GlobalInstance::parseMapJson()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/mainmap.json"));
	rapidjson::Value& mainData = doc["m"];

	for (unsigned int i = 0; i < mainData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = mainData[i];
		S_MainMap s_mainmap;
		s_mainmap.id = jsonvalue["id"].GetString();
		map_mapsdata[s_mainmap.id] = s_mainmap;
	}

	rapidjson::Document docsub = ReadJsonFile(ResourcePath::makePath("json/submap.json"));
	rapidjson::Value& subData = docsub["m"];
	for (unsigned int i = 0; i < subData.Size(); i++)
	{
		S_SubMap s_submap;
		rapidjson::Value& jsonvalue = subData[i];
		rapidjson::Value& v = jsonvalue["id"];
		s_submap.id = v.GetString();

		v = jsonvalue["ph"];
		s_submap.ph = atoi(v.GetString());

		v = jsonvalue["type"];
		s_submap.bgtype = atoi(v.GetString());

		if (jsonvalue.HasMember("awd"))
		{
			v = jsonvalue["awd"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				s_submap.vec_awd.push_back(v[i].GetString());
			}
		}

		std::string mainid = s_submap.id.substr(0, s_submap.id.find_last_of("-"));
		map_mapsdata[mainid].map_sublist[s_submap.id] = s_submap;
	}
}

int GlobalInstance::getMyHerosDeadCount()
{
	int count = 0;
	for (unsigned int i = 0; i < vec_myHeros.size(); i++)
	{
		Hero* hero = vec_myHeros[i];
		if (hero->getState() == HS_DEAD)
		{
			count++;
		}
	}
	return count;
}

int GlobalInstance::getHerosChangeLevelCount()
{
	int count = 0;
	for (unsigned int i = 0; i < vec_myHeros.size(); i++)
	{
		Hero* hero = vec_myHeros[i];
		if (hero->getLevel() + 1 == 10)
		{
			count++;
			break;
		}
	}
	return count;
}

int GlobalInstance::getHerosLevelCount(int lv)
{
	int count = 0;
	for (unsigned int i = 0; i < vec_myHeros.size(); i++)
	{
		Hero* hero = vec_myHeros[i];
		if (hero->getLevel() + 1 >= lv)
		{
			count++;
			break;
		}
	}
	return count;
}

int GlobalInstance::getFightHerosLevel()
{
	int lv = 0;
	int count = 0;
	for (unsigned int i = 0; i < vec_myHeros.size(); i++)
	{
		Hero* hero = vec_myHeros[i];
		if (hero->getState() == HS_TAKEON)
		{
			count++;
			lv += hero->getLevel();
		}
	}
	int avlv = 0;
	if (count > 0)
	{
		avlv = lv / count;
	}
	return avlv;
}

bool GlobalInstance::getUnlockHomehillCondition()
{
	for (unsigned int i = 0; i < Quest::myFinishMainQuest.size(); i++)
	{
		TaskData data = Quest::myFinishMainQuest[i];
		if (data.id == 7)//通关神龙教地图m1-4-2
		{
			return true;
		}
	}
	if (MyRes::getMyResCount("r001") < 200)
	{
		return true;
	}
	return false;
}

bool GlobalInstance::getLittleHerosPower(int comlv)
{
	for (unsigned int i = 0; i < vec_myHeros.size(); i++)
	{
		Hero* hero = vec_myHeros[i];
		if (hero->getPower().getValue() < comlv)
		{
			return true;
		}
	}
	return false;
}

bool GlobalInstance::getEnoughFightHeros()
{
	int count = 0;
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			count++;
		}
	}
	if (count == 6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool GlobalInstance::getCanUpgradeCount()
{
	for (int i = 1; i < 5; i++)
	{
		std::string str = StringUtils::format("s00%d", i);
		if (MyRes::getMyResCount(str) > 0)
		{
			return true;
		}
	}
	return false;
}

DynamicValueInt GlobalInstance::getMySoliverCount()
{
	return GlobalInstance::mySoliverCount;
}

void GlobalInstance::addMySoliverCount(DynamicValueInt val)
{
	GlobalInstance::mySoliverCount.setValue(GlobalInstance::mySoliverCount.getValue() + val.getValue());
	DataSave::getInstance()->setMySoliverCount(GlobalInstance::mySoliverCount.getValue());
}

void GlobalInstance::costMySoliverCount(DynamicValueInt val)
{
	Quest::setDailyTask(ACHIEVE_SLIVER, val.getValue());
	Quest::setAchieveTypeCount(ACHIEVE_SLIVER, val.getValue());
	GlobalInstance::mySoliverCount.setValue(GlobalInstance::mySoliverCount.getValue() - val.getValue());
	DataSave::getInstance()->setMySoliverCount(GlobalInstance::mySoliverCount.getValue());
}

DynamicValueInt GlobalInstance::getMyCoinCount()
{
	return GlobalInstance::myCoinCount;
}

void GlobalInstance::addMyCoinCount(DynamicValueInt val)
{
	GlobalInstance::myCoinCount.setValue(GlobalInstance::myCoinCount.getValue() + val.getValue());
	DataSave::getInstance()->setMyCoinCount(GlobalInstance::myCoinCount.getValue());
}

void GlobalInstance::costMyCoinCount(DynamicValueInt val)
{
	Quest::setDailyTask(ACHIEVE_GOLD, val.getValue());
	Quest::setAchieveTypeCount(ACHIEVE_GOLD, val.getValue());
	GlobalInstance::myCoinCount.setValue(GlobalInstance::myCoinCount.getValue() - val.getValue());
	DataSave::getInstance()->setMyCoinCount(GlobalInstance::myCoinCount.getValue());
}

int GlobalInstance::generateStoneCount(int qu)
{
	int count = 0;
	//镶嵌孔个数
	int rnd = GlobalInstance::getInstance()->createRandomNum(100);
	if (qu == 0)
	{
		if (rnd < 80)
			count = 0;
		else if (rnd < 90)
			count = 1;
		else if (rnd < 98)
			count = 2;
		else
			count = 3;
	}
	else if (qu == 1)
	{
		if (rnd < 60)
			count = 0;
		else if (rnd < 90)
			count = 1;
		else if (rnd < 98)
			count = 2;
		else
			count = 3;
	}
	else if (qu == 2)
	{
		if (rnd < 50)
			count = 0;
		else if (rnd < 80)
			count = 1;
		else if (rnd < 95)
			count = 2;
		else
			count = 3;
	}
	else if (qu == 3)
	{
		if (rnd < 40)
			count = 0;
		else if (rnd < 80)
			count = 1;
		else if (rnd < 95)
			count = 2;
		else
			count = 3;
	}
	else
	{
		if (rnd < 40)
			count = 0;
		else if (rnd < 75)
			count = 1;
		else if (rnd < 95)
			count = 2;
		else
			count = 3;
	}
	return count;
}

int GlobalInstance::generateHeroPotential()
{
	int innroomlv = DataSave::getInstance()->getBuildingLv("6innroom");
	if (innroomlv < 10)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(100);
		if (r < 50)
			return 0;
		else if (r < 90)
			return 1;
		return 2;
	}
	else if (innroomlv < 19)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(100);
		if (r < 50)
			return 0;
		else if (r < 80)
			return 1;
		else if (r < 99)
			return 2;
		return 3;
	}
	else
	{
		int r = GlobalInstance::getInstance()->createRandomNum(10000);
		if (r < 4999)
			return 0;
		else if (r < 7999)
			return 1;
		else if (r < 9899)
			return 2;
		else if (r < 9999)
			return 3;
		return 4;
	}
	return 0;
}

std::string GlobalInstance::getResUIFrameName(std::string resid, int qu)
{
	std::string str = StringUtils::format("ui/%s.png", resid.c_str());

	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		if (qu == 3)
		{
			str = StringUtils::format("ui/%s-2.png", resid.c_str());
		}
		else if (qu == 4)
		{
			str = StringUtils::format("ui/%s-3.png", resid.c_str());
		}
	}
	return str;
}

void GlobalInstance::saveRefreshMarketTime(int time)
{
	refreshMarketTime = time;
	DataSave::getInstance()->setRefreshMarketTime(time);
}


int GlobalInstance::getRefreshMarketTime()
{
	return refreshMarketTime;
}

void GlobalInstance::parseSuitJson()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/suitequip.json"));
	rapidjson::Value& allData = doc["sq"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			EquipSuit data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["suite"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_suit.push_back(v[m].GetString());
			}
			v = jsonvalue["bns"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_bns.push_back(v[m].GetDouble());
			}
			map_EquipSuit[data.id] = data;
		}
	}
}

void GlobalInstance::parseTBoxJson()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/tbox.json"));
	rapidjson::Value& allData = doc["b"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			TBoxData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["awd"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_awds.push_back(v[m].GetString());
			}
			v = jsonvalue["rnd"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				int rnd = v[m].GetInt();
				for (unsigned int n = 0; n < data.vec_rnd.size(); n++)
				{
					rnd += data.vec_rnd[n];
				}
				data.vec_rnd.push_back(rnd);
			}
			map_TBoxs[data.id] = data;
		}
	}
}

void GlobalInstance::heroSortByLv()
{
	std::sort(vec_myHeros.begin(), vec_myHeros.end(), larger_heroLvCB);

	for (int m = 0; m < 6; m++)
	{
		for (unsigned int i = 0; i < vec_myHeros.size(); i++)
		{
			Hero* myhero = GlobalInstance::myCardHeros[m];
			if (myhero != NULL && myhero->getName().compare(vec_myHeros[i]->getName()) == 0)
			{
				GlobalInstance::myCardHeros[m] = vec_myHeros[i];
				break;
			}
		}
	}
}

bool GlobalInstance::larger_heroLvCB(Hero* a, Hero* b)
{
	if (a->getLevel() > b->getLevel())
		return true;
	else
		return false;
}

std::string GlobalInstance::getMyID()
{
	return myID;
}

std::string GlobalInstance::getMyNickName()
{
	return myNickName;
}

void GlobalInstance::setMyID(std::string val)
{
	myID = val;
}

void GlobalInstance::setMyNickName(std::string val)
{
	myNickName = val;
}

int GlobalInstance::getUnlockChapter()
{
	return unlockchapter;
}
void GlobalInstance::saveUnlockChapter(int val)
{
	unlockchapter = val;
	DataSave::getInstance()->setUnlockChapter(val);
}

void GlobalInstance::setSilverRefHeroCount(int val)
{
	if (val > 19)//最大上限
		val = 19;
	silverRefHeroCount = val;
	DataSave::getInstance()->setSilverRefHeroCount(val);
}


int GlobalInstance::getSilverRefHeroCount()
{
	return silverRefHeroCount;
}

void GlobalInstance::setResetSilverRefHeroCountTime(int val)
{
	resetSilverRefHeroCountTime = val;
	DataSave::getInstance()->setResetSilverRefHeroCountTime(val);
}

int GlobalInstance::getResetSilverRefHeroCountTime()
{
	return resetSilverRefHeroCountTime;
}

void GlobalInstance::recoveCardHeroMaxHp()
{
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			if (GlobalInstance::myCardHeros[i]->getState() == HS_DEAD)
				GlobalInstance::myCardHeros[i] = NULL;
			else
				GlobalInstance::myCardHeros[i]->setHp(GlobalInstance::myCardHeros[i]->getMaxHp());
		}
	}
}

std::string GlobalInstance::getUserDefaultXmlString()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return FileUtils::getInstance()->getStringFromFile(UserDefault::getInstance()->getXMLFilePath());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	if (JniHelper::getStaticMethodInfo(methodInfo, ANDOIRJNICLSNAME, "getUserDefaultXmlString", "()Ljava/lang/String;"))
	{
		jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = methodInfo.env->GetStringUTFChars(jstr, 0);
	}
	return ret;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return getUserDefaultXml();
#endif
}
