#include "GlobalInstance.h"
#include "DataSave.h"
#include "Hero.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MyRes.h"
#include "Const.h"
#include "Quest.h"
#include "Building.h"

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

int GlobalInstance::myOutMapCarry = 100;

int GlobalInstance::silverRefHeroCount = 0;

int GlobalInstance::resetSilverRefHeroCountTime = 0;

bool GlobalInstance::isNewHeroRefresh = false;

std::string GlobalInstance::qq;

std::string GlobalInstance::legalcopyurl;

bool GlobalInstance::isCheat = false;

S_TimeGift GlobalInstance::serverTimeGiftData;

Hero* GlobalInstance::myOnChallengeHeros[6];//竞技赛我选择的6个英雄

Hero* GlobalInstance::matchPairHeros[6];//匹配到的6个英雄
MyMatchInfo GlobalInstance::myMatchInfo;

MyRankInfo GlobalInstance::myRankInfo;

bool GlobalInstance::isServerReceipt = false;

bool GlobalInstance::isResetData = false;

bool GlobalInstance::isNotSameUUID = false;

std::string GlobalInstance::upgradeurl;

LoginData GlobalInstance::loginData;

int GlobalInstance::punishment = 0;

std::vector<int> GlobalInstance::vec_newerguides;

std::vector<int> GlobalInstance::vec_costCoins;

std::vector<int> GlobalInstance::vec_costprops;

bool GlobalInstance::isBuyFirstCharge = false;

std::vector<int> GlobalInstance::vec_mazeroute;

int GlobalInstance::mazerouteindex = 0;

std::string GlobalInstance::eventfrommapid;
int GlobalInstance::eventstartmappos = -1;

int GlobalInstance::map_randstartpos = -1;

std::vector<std::string> GlobalInstance::vec_news;

int GlobalInstance::getNewsTime = 0;

int GlobalInstance::mazeEventData[3] = { 0 };

std::string GlobalInstance::timeMarketStr;
std::map<std::string, TimeMartData> GlobalInstance::map_timeMartData;

bool GlobalInstance::ishasmazeentry = false;

DynamicValueInt GlobalInstance::totalPayAmout;

std::vector<bool> GlobalInstance::vec_rebateisget;

bool GlobalInstance::isHasNewhero = false;
int GlobalInstance::showz002hinttextcount = 0;

std::vector<S_StarData> GlobalInstance::vec_stardata;

int GlobalInstance::curMapFinishStars = 0;

int GlobalInstance::takeoutherocount = 0;

int GlobalInstance::contunefightsucccount = 0;

std::vector<S_ChapterStarAwd> GlobalInstance::vec_chaperstarawds;

std::map<std::string, S_BUILDINREPAIR> GlobalInstance::map_buildingrepairdata;

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
	return "********************";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
	if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "UUID", "()Ljava/lang/String;"))
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
	return "2.0.3";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
	if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "getVersion", "()Ljava/lang/String;"))
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
	std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
	if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "getPkgName", "()Ljava/lang/String;"))
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
	std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
	if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "getChannelID", "()Ljava/lang/String;"))
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

std::string GlobalInstance::getIDFA()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	return getiOSIDFA();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return "WIN32 IDFA TEST";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return "ANDROID_IDFA";
#endif
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
std::string GlobalInstance::getAppMD5Sign()
{

	std::string ret;
	JniMethodInfo methodInfo;
	std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
	if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "getMD5Sign", "()Ljava/lang/String;"))
	{
		jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
		ret = methodInfo.env->GetStringUTFChars(jstr, 0);
	}
	return ret;
}
#endif


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
	//int syssec = GlobalInstance::getSysSecTime();
	//int static randNum = 0;
	//randNum += 3600 * 24;
	//syssec += randNum;
	//if (randNum < 0 || randNum > INT32_MAX)
	//{
	//	syssec = 0;
	//	randNum = 0;
	//}
	//log("zhou syssec = %d", syssec);
	//srand(syssec);
	//int r = rand() % val;
	//return r;

	//3.x 以后随机数产生方法
	auto rand = RandomHelper();
	//产生MIN~MIX的整数包括最大和最小
	int r = rand.random_int(0, val - 1);
	return r;
}


void GlobalInstance::loadInitData()
{
	refreshHeroTime = DataSave::getInstance()->getRefreshHeroTime();
	resetSilverRefHeroCountTime = DataSave::getInstance()->getResetSilverRefHeroCountTime();

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

	if (DataSave::getInstance()->getLocalUUID().length() <= 0)
		DataSave::getInstance()->setLocalUUID(GlobalInstance::getInstance()->UUID());

	silverRefHeroCount = DataSave::getInstance()->getSilverRefHeroCount();

	serverTimeGiftData.isopen = false;
	serverTimeGiftData.lefttime = 0;
	serverTimeGiftData.turn = 0;

	map_timeMartData.clear();

	vec_newerguides.clear();

	for (int i = 0; i < 94; i++)
	{
		vec_newerguides.push_back(1);
	}

	vec_costCoins.clear();
	for (int i = 0; i < 8; i++)
	{
		vec_costCoins.push_back(0);
	}

	vec_costprops.clear();
	for (int i = 0; i < COSTPROP_END; i++)
	{
		vec_costprops.push_back(0);
	}

	vec_costCoins[0] = DataSave::getInstance()->getTCostCoin();
	std::string str = DataSave::getInstance()->getPosCostCoin();

	if (str.length() > 0)
	{
		std::vector<std::string> vec_ret;
		CommonFuncs::split(str, vec_ret, "-");
		for (unsigned int i = 0; i < vec_ret.size(); i++)
		{
			if (i < vec_costCoins.size() - 1)
			{
				vec_costCoins[i + 1] = atoi(vec_ret[i].c_str());
			}
		}
	}

	str = DataSave::getInstance()->getPropsCount();
	
	if (str.length() > 0)
	{
		std::vector<std::string> vec_ret;
		CommonFuncs::split(str, vec_ret, "-");
		for (unsigned int i = 0; i < vec_ret.size(); i++)
		{
			if (i < vec_costprops.size())
			{
				vec_costprops[i] = atoi(vec_ret[i].c_str());
			}
		}
	}
	str = DataSave::getInstance()->getMazeEventCount();

	if (str.length() > 0)
	{
		std::vector<std::string> vec_ret;
		CommonFuncs::split(str, vec_ret, "-");
		for (unsigned int i = 0; i < vec_ret.size(); i++)
		{
			if (i < 3)
			{
				mazeEventData[i] = atoi(vec_ret[i].c_str());
			}
		}
	}

	vec_rebateisget.clear();

	for (int i = 0; i < 8; i++)
	{
		vec_rebateisget.push_back(false);
	}
	str = DataSave::getInstance()->getRebateAwds();

	if (str.length() > 0)
	{
		for (unsigned int i = 0; i < str.size(); i++)
			GlobalInstance::vec_rebateisget[i] = atoi(str.substr(i, 1).c_str()) == 1?true:false;
	}
}

void GlobalInstance::saveMyHeros()
{
	int qus[QUMAX] = { 0 };
	int herocount = GlobalInstance::vec_myHeros.size();
	int deathherocount = 0;
	for (int i = 0; i < herocount; i++)
	{
		if (GlobalInstance::vec_myHeros[i]->getState() == HS_DEAD)
			deathherocount++;
		qus[GlobalInstance::vec_myHeros[i]->getPotential()]++;
		saveHeroByIndex(i);
	}

	std::string cstr = StringUtils::format("%d-%d", herocount - deathherocount, deathherocount);

	for (int i = 0; i < QUMAX; i++)
	{
		std::string pstr = StringUtils::format("-%d", qus[i]);
		cstr.append(pstr);
	}

	for (int i = 0; i < 6; i++)
	{
		Hero* hero = GlobalInstance::myCardHeros[i];
		if (hero != NULL)
		{
			std::string hstr = StringUtils::format(";%s-%d-%d-%d-%d", hero->getName().c_str(), hero->getLevel(), hero->getVocation(), hero->getPotential(), hero->getPos());
			cstr.append(hstr);
		}
	}
	DataSave::getInstance()->setMyHeroCount(cstr);
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

	int qus[QUMAX] = { 0 };
	int herocount = GlobalInstance::vec_myHeros.size();
	int deathherocount = 0;
	for (int i = 0; i < herocount; i++)
	{
		if (GlobalInstance::vec_myHeros[i]->getState() == HS_DEAD)
			deathherocount++;
		qus[GlobalInstance::vec_myHeros[i]->getPotential()]++;
	}

	std::string cstr = StringUtils::format("%d-%d", herocount - deathherocount, deathherocount);

	for (int i = 0; i < QUMAX; i++)
	{
		std::string pstr = StringUtils::format("-%d", qus[i]);
		cstr.append(pstr);
	}

	for (int i = 0; i < 6; i++)
	{
		Hero* hero = GlobalInstance::myCardHeros[i];
		if (hero != NULL)
		{
			std::string hstr = StringUtils::format(";%s-%d-%d-%d-%d", hero->getName().c_str(), hero->getLevel(), hero->getVocation(), hero->getPotential(), hero->getPos());
			cstr.append(hstr);
		}
	}

	DataSave::getInstance()->setMyHeroCount(cstr);
}

void GlobalInstance::saveHeroByIndex(int index)
{
	std::string herokey = StringUtils::format("hero%d", index);
	Hero* hero = GlobalInstance::vec_myHeros[index];
	std::string datastr = StringUtils::format("%s-%d-%d-%d-%d-%s-%d-%d-%d-%d-%d-%d-%d-%d;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex(), hero->getId().c_str(), hero->getState(), hero->getPos(), (int)hero->getHp(), 
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
					hero->setId(vec_tmp[5]);
					int state = atoi(vec_tmp[6].c_str());
					hero->setState(state);
					int pos = atoi(vec_tmp[7].c_str());
					hero->setPos(pos);
					if (pos > 0 && state != HS_DEAD)
						GlobalInstance::myCardHeros[pos - 1] = hero;

					for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
					{
						ResBase* res = MyRes::vec_MyResources[i];
						if (res->getType() >= T_ARMOR && res->getType() <= T_NG)
						{
							Equipable* eres = (Equipable*)res;
							if (hero->getName().compare(eres->getWhos()) == 0)
							{
								hero->setEquipable(eres, eres->getType());
							}
						}
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
			v = jsonvalue["attr"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_attrbias.push_back(v[m].GetInt());
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
	if (str.length() <= 0)
	{
		str = DataSave::getInstance()->getMyOldMainTask();
	}

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
		}
		else if (vec_TaskMain[vec_tmp.size() - 1].isfinish >= QUEST_FINISH)
		{
			if (vec_tmp.size() < vec_TaskMain.size())
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

	if (str.length() <= 0)
	{
		str = DataSave::getInstance()->getMyOldBranchTask();
	}

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
			if (vec_tmp.size() < vec_TaskBranch.size())
			{
				myCurBranchData = vec_TaskBranch[vec_tmp.size()];
			}
			else
			{
				myCurBranchData = vec_TaskBranch[vec_tmp.size() - 1];
			}
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

			v = jsonvalue["subtype"];
			data.type = atoi(v.GetString());

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
			if (jsonvalue.HasMember("friendly"))
			{
				v = jsonvalue["friendly"];
				map_AllResources[rid].friendly = atoi(v.GetString());
			}
		}
	}
}

void GlobalInstance::loadMyResData()
{
	std::string str = DataSave::getInstance()->getMyRes();
	if (str.length() > 0)
	{
		int i001count = 0;
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;

			CommonFuncs::split(vec_tmp[i], vec_one, "-");
			std::string rid = vec_one[0];

			int m = -1;
			for (int k = 0; k < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); k++)
			{
				if (rid.compare(0, 1, RES_TYPES_CHAR[k]) == 0)
				{
					m = k;
					break;
				}
			}
			if (rid.length() > 4)
				continue;

			if (m >= T_ARMOR && m <= T_FASHION)
			{
				Equip* res = new Equip();

				//兼容装备招募券会抽到神兵宝甲（神兵宝甲只能是红色）
				if (GlobalInstance::map_Equip[rid].type >= 100)
				{
					int r = GlobalInstance::getInstance()->createRandomNum(28);
					rid = StringUtils::format("a%03d", r);
				}
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
			else if (m > -1)
			{
				if (rid.compare("i001") == 0)//兼容i001异常问题
				{
					i001count += atoi(vec_one[1].c_str());
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

		//兼容i001异常问题
		if (i001count > 0)
		{
			MyRes::Add("i002", i001count);
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
			if (vec_tmp[i].length() > 0)
			{
				std::vector<std::string> vec_one;
				std::string sep = "-";
				if (std::string::npos != vec_tmp[i].find("#"))
					sep = "#";

				CommonFuncs::split(vec_tmp[i], vec_one, sep);

				std::string renationstr;
				if (std::string::npos == vec_tmp[i].find("#"))
				{
					std::string f = vec_tmp[i].substr(5, vec_tmp[i].find_last_of("-") - 5);
					map_myfriendly[vec_one[0]].friendly = atoi(f.c_str());
					renationstr = vec_tmp[i].substr(vec_tmp[i].find_last_of("-") + 1);
				}
				else
				{
					map_myfriendly[vec_one[0]].friendly = atoi(vec_one[1].c_str());
					renationstr = vec_one[2];
				}
				std::vector<std::string> vec_two;
				CommonFuncs::split(renationstr, vec_two, ",");
				for (unsigned int i = 0; i < vec_two.size(); i++)
				{
					map_myfriendly[vec_one[0]].relation.push_back(atoi(vec_two[i].c_str()));
				}
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
		if (data.friendly != 0)
		{
			std::string s;
			for (unsigned int i = 0; i < data.relation.size(); i++)
			{
				std::string s1 = StringUtils::format("%d,", data.relation[i]);
				s.append(s1);
			}
			std::string onestr = StringUtils::format("%s#%d#%s;", data.npcid.c_str(), data.friendly, s.substr(0, s.length() - 1).c_str());
			str.append(onestr);
		}
	}
	if (str.length() > 0)
	{
		DataSave::getInstance()->setNpcFriendly(str.substr(0, str.length() - 1));
	}
}

void GlobalInstance::loadChapterStarAwds()
{
	rapidjson::Document rsdoc = ReadJsonFile(ResourcePath::makePath("json/chapterstar.json"));
	rapidjson::Value& rsallData = rsdoc["mr"];
	for (unsigned int i = 0; i < rsallData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = rsallData[i];
		if (jsonvalue.IsObject())
		{
			S_ChapterStarAwd data;
			rapidjson::Value& v = jsonvalue["stars"];

			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_starnum.push_back(v[m].GetInt());
				data.vec_getstate.push_back(0);
			}

			v = jsonvalue["awds"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				std::vector<std::string> vec_str;
				for (unsigned int n = 0; n < v[m].Size(); n++)
				{
					vec_str.push_back(v[m][n].GetString());
				}
				data.vec_adws.push_back(vec_str);
			}

			v = jsonvalue["id"];
			int chapter = atoi(v.GetString());
			std::string savestr = DataSave::getInstance()->getChapterStarAwd(chapter);
			if (savestr.length() >= 3)
			{
				for (int t = 0; t < 3; t++)
				{
					int state = atoi(savestr.substr(t, 1).c_str());
					data.vec_getstate[t] = state;
				}
			}

			vec_chaperstarawds.push_back(data);
		}
	}
}

bool GlobalInstance::compareHighEquip(int type, Hero* herodata)
{
	Equipable* myequip = herodata->getEquipable(type);
	bool ishas = false;
	if (myequip != NULL)
	{
		for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
		{

			ResBase* res = MyRes::vec_MyResources[i];
			if (res->getWhere() == MYEQUIP)
			{
				continue;
			}
			if (type == res->getType())
			{
				Equipable* m_res = (Equipable*)res;
				if (type >= T_WG && type <= T_NG)
				{
					if (GlobalInstance::map_GF[m_res->getId()].vec_skillbns[herodata->getVocation()] >= 1)
					{
						if (type == T_WG)
						{
							if (m_res->getAtk() > myequip->getAtk())
							{
								ishas = true;
								break;
							}
						}
						else if (type == T_NG)
						{
							if (m_res->getDf() > myequip->getDf())
							{
								ishas = true;
								break;
							}
						}
					}

				}
				else if (type == T_ARMOR)
				{
					if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
					{
						if (m_res->getAtk() > myequip->getAtk())
						{
							ishas = true;
							break;
						}
					}
				}

				else if (type == T_EQUIP)
				{
					if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
					{
						if (m_res->getHp() > myequip->getHp())
						{
							ishas = true;
							break;
						}
					}
				}
				else if (type == T_HANDARMOR)
				{
					if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
					{
						if (m_res->getDodge() > myequip->getDodge())
						{
							ishas = true;
							break;
						}
					}
				}
				else if (type == T_FASHION)
				{
					if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
					{
						if (m_res->getHp() > myequip->getHp())
						{
							ishas = true;
							break;
						}
					}

				}
			}
		}
		if (ishas)
		{
			return true;
		}
	}
	return false;
}

Equipable* GlobalInstance::compareFitEquip(int type, Hero* herodata)
{
	Equipable* ret_equipable = NULL;
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getWhere() == MYEQUIP)
		{
			continue;
		}

		if (type == res->getType() && herodata != NULL)
		{
			Equipable* m_res = (Equipable*)res;
			if (type >= T_WG && type <= T_NG)
			{
				if (GlobalInstance::map_GF[m_res->getId()].vec_skillbns[herodata->getVocation()] >= 1)
				{
					if (ret_equipable == NULL)
						ret_equipable = m_res;
					else
					{
						if (type == T_WG)
						{
							if (m_res->getAtk() > ret_equipable->getAtk())
							{
								ret_equipable = m_res;
							}
						}
						else if (type == T_NG)
						{
							if (m_res->getDf() > ret_equipable->getDf())
							{
								ret_equipable = m_res;
							}
						}
					}
				}
	
			}
			else if (type == T_ARMOR)
			{
				if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
				{
					if (ret_equipable == NULL)
					{
						ret_equipable = m_res;
					}
					else
					{
						if (m_res->getAtk() > ret_equipable->getAtk())
						{
							ret_equipable = m_res;
						}
					}
				}
			}
			else if (type == T_EQUIP)
			{
				if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
				{
					if (ret_equipable == NULL)
					{
						ret_equipable = m_res;
					}
					else
					{
						if (m_res->getHp() > ret_equipable->getHp())
						{
							ret_equipable = m_res;
						}
					}
				}
			}
			else if (type == T_HANDARMOR)
			{
				if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
				{
					if (ret_equipable == NULL)
					{
						ret_equipable = m_res;
					}
					else
					{
						if (m_res->getDodge() > ret_equipable->getDodge())
						{
							ret_equipable = m_res;
						}
					}
				}
			}
			else if (type == T_FASHION)
			{
				if (GlobalInstance::map_Equip[m_res->getId()].vec_bns[herodata->getVocation()] >= 1)
				{
					if (ret_equipable == NULL)
					{
						ret_equipable = m_res;
					}
					else
					{
						if (m_res->getHp() > ret_equipable->getHp())
						{
							ret_equipable = m_res;
						}
					}
				}
			}
		}
	}
	return ret_equipable;
}

int GlobalInstance::getMatchLvByScroe(int m_lv)
{
	int lv = 0;
	int lvscores[] = { 100, 300, 700, 1000, INT32_MAX };
	int lvsize = sizeof(lvscores) / sizeof(lvscores[0]);
	
	for (int i = 0; i < lvsize; i++)
	{
		if (m_lv < lvscores[i])
		{
			lv = i;
			break;
		}
	}
	
	return lv;
}

void GlobalInstance::parsePairFriendly(std::string fstr)
{	
	if (fstr.length() > 0)
	{
		std::vector<std::string> vec_fstr;
		CommonFuncs::split(fstr, vec_fstr, ";");

		for (unsigned int i = 0; i < vec_fstr.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_fstr[i], vec_one, "-");
			if (vec_one.size() >= 2)
			{
				std::string nid = StringUtils::format("n%s", vec_one[0].c_str());
				int ralation = atoi(vec_one[1].c_str());
				NpcFriendly s_f;
				s_f.npcid = nid;
				s_f.friendly = 0;
				if (ralation == NPC_COUPEL + 1)
				{
					s_f.relation.push_back(1);
					s_f.relation.push_back(3);
				}
				else if (ralation == NPC_COUPEL + 2)
				{
					s_f.relation.push_back(2);
					s_f.relation.push_back(3);
				}
				else if (ralation == NPC_COUPEL + 3)
				{
					s_f.relation.push_back(1);
					s_f.relation.push_back(2);
				}
				else
				{
					s_f.relation.push_back(ralation);
				}

				GlobalInstance::myMatchInfo.map_pairfriendly[nid] = s_f;
			}
		}
	}
}

bool GlobalInstance::isCanUpgradeBuilding()
{
	if (Building::map_buildingDatas["7homehill"]->level.getValue() < Building::map_buildingDatas["7homehill"]->maxlevel.getValue() - 1
		|| Building::map_buildingDatas["6innroom"]->level.getValue() < Building::map_buildingDatas["6innroom"]->maxlevel.getValue() - 1
		|| Building::map_buildingDatas["5market"]->level.getValue() < Building::map_buildingDatas["5market"]->maxlevel.getValue() - 1
		|| Building::map_buildingDatas["2smithy"]->level.getValue() < Building::map_buildingDatas["2smithy"]->maxlevel.getValue() - 1
		|| Building::map_buildingDatas["4trainigroom"]->level.getValue() < Building::map_buildingDatas["4trainigroom"]->maxlevel.getValue() - 1)
	{
		return true;
	}
	return false;
}

bool GlobalInstance::isAllFinishBranch()
{
	if (Quest::myFinishBranchQuest.size() == GlobalInstance::vec_TaskBranch.size())
	{
		return true;
	}
	return false;
}

bool GlobalInstance::checkNewQuest()
{
	//主线
	int mcount = 0;
	for (unsigned int i = 0; i < Quest::myFinishMainQuest.size(); i++)
	{
		TaskData data = Quest::myFinishMainQuest[i];
		if (data.isfinish == QUEST_FINISH)
		{
			mcount++;
			break;
		}
	}
	//支线
	int bcount = 0;
	for (unsigned int i = 0; i < Quest::myFinishBranchQuest.size(); i++)
	{
		TaskData data = Quest::myFinishBranchQuest[i];
		if (data.isfinish == QUEST_FINISH)
		{
			bcount++;
			break;
		}
	}
	//每日
	int dcount = 0;
	std::map<std::string, DailyTaskData>::iterator it;
	for (it = GlobalInstance::map_DTdata.begin(); it != GlobalInstance::map_DTdata.end(); it++)
	{
		DailyTaskData data = GlobalInstance::map_DTdata[it->first];
		if (data.state == DAILY_FINISHED)
		{
			dcount++;
			break;
		}
	}
	//log("-------maintask = %d, branchtask = %d, mcount = %d, bcount = %d, dcount = %d ", GlobalInstance::myCurMainData.isfinish, GlobalInstance::myCurBranchData.isfinish, mcount, bcount, dcount);
	if (GlobalInstance::myCurMainData.isfinish == QUEST_TASK || GlobalInstance::myCurBranchData.isfinish == QUEST_TASK || mcount > 0 || bcount > 0 || dcount > 0)
	{
		return true;
	}
	return false;
}

std::vector<std::vector<std::string>> GlobalInstance::getMatchRewardByLv(int lv)
{
	std::vector<std::vector<std::string>> vec_matchlv;
	std::vector<std::string> vec_retstr;
	CommonFuncs::split(GlobalInstance::myMatchInfo.rewardstr, vec_retstr, ";");
	for (unsigned int i = 0; i < vec_retstr.size(); i++)
	{
		std::vector<std::vector<std::string>> vec_mstr;
		std::vector<std::string> vec_match;
		CommonFuncs::split(vec_retstr[i], vec_match, ",");
		for (unsigned m = 0; m < vec_match.size(); m++)
		{
			std::vector<std::string> vec_res;
			CommonFuncs::split(vec_match[m], vec_res, "-");
			vec_mstr.push_back(vec_res);
		}
		if (i == lv)
		{
			vec_matchlv = vec_mstr;
			break;
		}
	}
	return vec_matchlv;
}

bool GlobalInstance::strengthMaterial(Equipable* m_res)
{
	int equipcount = 0;
	for (int i = 0; i < 3; i++)
	{
		std::string restr = StringUtils::format("q00%d", i + 1);
		if (MyRes::getMyResCount(restr) >= m_res->getLv().getValue() + 1)
		{
			equipcount++;
		}
	}
	if (equipcount == 3 && (m_res->getLv().getValue() + 1) < (sizeof(COSTLV) / sizeof(COSTLV[0])))
	{
		return true;
	}
	return false;
}

float GlobalInstance::compareOtherEquip(ResBase* m_res, Hero* herodata, int attr)
{
	Equipable* myequip = herodata->getEquipable(m_res->getType());
	if (myequip != NULL)
	{
		float m_hp = myequip->getHp();
		float m_atk = myequip->getAtk();
		float m_df = myequip->getDf();
		float m_atspeed = myequip->getAtkSpeed();
		float m_crit = myequip->getCrit();
		float m_dodge = myequip->getDodge();
		Equipable* equip = (Equip*)m_res;
		if (attr == 0)
		{
			float hp = equip->getHp();
			if (m_hp - hp != 0)
			{
				return m_hp - hp;
			}
		}
		else if (attr == 1)
		{
			float atk = equip->getAtk();
			if (m_atk - atk != 0)
			{
				return m_atk - atk;
			}
		}
		else if (attr == 2)
		{
			float atk = equip->getDf();
			if (m_df - atk != 0)
			{
				return m_df - atk;
			}
		}
		else if (attr == 3)
		{
			float atk = equip->getAtkSpeed();
			if (m_atspeed - atk != 0)
			{
				return m_atspeed - atk;
			}
		}
		else if (attr == 4)
		{
			float atk = equip->getCrit();
			if (m_crit - atk != 0)
			{
				return m_crit - atk;
			}
		}
		else if (attr == 1)
		{
			float atk = equip->getDodge();
			if (m_dodge - atk != 0)
			{
				return m_dodge - atk;
			}
		}
	}

	return 0.0f;
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
		Equipable* equip = (Equipable*)GlobalInstance::vec_myHeros[index]->getEquipable(i);
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

		v = jsonvalue["pf"];
		s_submap.pf = atoi(v.GetString());

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

		if (jsonvalue.HasMember("starc"))
		{
			v = jsonvalue["starc"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				s_submap.vec_starc.push_back(v[i].GetString());
			}
		}
		if (jsonvalue.HasMember("fstar3awd"))
		{
			v = jsonvalue["fstar3awd"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				s_submap.vec_f3starawds.push_back(v[i].GetString());
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
		if (hero->getLevel() + 1 >= 10)
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

bool GlobalInstance::getCanUpgradeCount(std::string gstr)
{
	for (int i = 1; i < 5; i++)
	{
		std::string str = StringUtils::format("%s%d", gstr.c_str(), i);
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
	
	GlobalInstance::mySoliverCount.setValue(GlobalInstance::mySoliverCount.getValue() - abs(val.getValue()));
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
	GlobalInstance::myCoinCount.setValue(GlobalInstance::myCoinCount.getValue() - abs(val.getValue()));
	DataSave::getInstance()->setMyCoinCount(GlobalInstance::myCoinCount.getValue());

	vec_costCoins[0] += abs(val.getValue());
	DataSave::getInstance()->setTCostCoin(vec_costCoins[0]);

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
	int innroomlv = Building::map_buildingDatas["6innroom"]->level.getValue();
	if (innroomlv < 10)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(100);
		if (r < 55)
			return 0;
		else if (r < 95)
			return 1;
		return 2;
	}
	else if (innroomlv < 19)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(100);
		if (r < 50)
			return 0;
		else if (r < 89)
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
		else if (r < 8399)
			return 1;
		else if (r < 9899)
			return 2;
		else if (r < 9999)
			return 3;
		return 4;
	}
	return 0;
}

int GlobalInstance::generateHeroPotentialByCoin()
{
	int innroomlv = Building::map_buildingDatas["6innroom"]->level.getValue();
	if (innroomlv < 10)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(100);
		if (r < 84)
			return 1;
		else if (r < 99)
			return 2;
		return 3;
	}
	else if (innroomlv < 19)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(10000);
		if (r < 7899)
			return 1;
		else if (r < 9899)
			return 2;
		else if (r < 9999)
			return 3;
		return 4;
	}
	else
	{
		int r = GlobalInstance::getInstance()->createRandomNum(10000);
		if (r < 9498)
			return 2;
		else if (r < 9998)
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
		else if (qu >= 4)
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
				data.vec_rnd.push_back(rnd);
			}
			map_TBoxs[data.id] = data;
		}
	}
}

void GlobalInstance::heroSortByLv()
{
	std::sort(vec_myHeros.begin(), vec_myHeros.end(), larger_heroLvCB);
	std::sort(vec_myHeros.begin(), vec_myHeros.end(), sort_heroDeathCB);
	

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

bool GlobalInstance::sort_heroDeathCB(Hero* a, Hero* b)
{
	if (a->getState() == HS_DEAD && b->getState() != HS_DEAD)
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

void GlobalInstance::setIsNichnameCountMax(bool val)
{
	DataSave::getInstance()->setIsNichnameCountMax(val);
}

bool GlobalInstance::getIsNichnameCountMax()
{
	return DataSave::getInstance()->getIsNichnameCountMax();
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

void GlobalInstance::loadBuildingsLv()
{
	std::vector<int> vec_lv;
	std::string str = DataSave::getInstance()->getBuildingsLv();
	std::vector<std::string> vec_strs;
	CommonFuncs::split(str, vec_strs, "-");
	for (unsigned int i = 0; i < vec_strs.size(); i++)
	{
		vec_lv.push_back(atoi(vec_strs[i].c_str()));
	}
	int index = 0;
	std::map<std::string, Building*>::iterator it;
	for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
	{
		Building::map_buildingDatas[it->first]->level.setValue(vec_lv[index]);
		index++;
	}
}

void GlobalInstance::saveNewerGuide(int index, bool val)
{
	GlobalInstance::vec_newerguides[index] = val;
	std::string str;
	for (unsigned int i = 0; i < GlobalInstance::vec_newerguides.size(); i++)
	{
		std::string pstr = StringUtils::format("%d", GlobalInstance::vec_newerguides[i]);
		str.append(pstr);
	}
	DataSave::getInstance()->setNewerGuides(str);
}

void GlobalInstance::setPosCostCoin(int forwhere, int addval)
{
	vec_costCoins[forwhere] += addval;
	std::string str;

	for (unsigned int i = 1; i < vec_costCoins.size(); i++)
	{
		if (str.length() > 0)
			str.append("-");
		std::string pstr = StringUtils::format("%d", vec_costCoins[i]);
		str.append(pstr);
	}
	DataSave::getInstance()->setPosCostCoin(str);
}

void GlobalInstance::usePropsCount(int idindex, int useval)
{
	vec_costprops[idindex] += useval;
	std::string str;
	for (unsigned int i = 0; i < vec_costprops.size(); i++)
	{
		if (str.length() > 0)
			str.append("-");
		std::string pstr = StringUtils::format("%d", vec_costprops[i]);
		str.append(pstr);
	}
	DataSave::getInstance()->setPropsCount(str);
}

void GlobalInstance::setMazeEventData(int entercount, int useexitcount, int usecoin)
{
	mazeEventData[0] += entercount;
	mazeEventData[1] += useexitcount;
	mazeEventData[2] += usecoin;
	std::string str = StringUtils::format("%d-%d-%d", mazeEventData[0], mazeEventData[1], mazeEventData[2]);
	DataSave::getInstance()->setMazeEventCount(str);
}

bool GlobalInstance::getIsGetRebateAwds(int index)
{
	return GlobalInstance::vec_rebateisget[index];
}

void GlobalInstance::setIsGetRebateAwds(int index, bool val)
{
	vec_rebateisget[index] = val;
	std::string str;
	for (unsigned int i = 0; i < vec_rebateisget.size(); i++)
	{
		std::string onestr = StringUtils::format("%d", vec_rebateisget[i]?1:0);
		str.append(onestr);
	}
	if (str.length() > 0)
	{
		DataSave::getInstance()->setRebateAwds(str);
	}
}

void GlobalInstance::parseBuildingRepairData()
{
	rapidjson::Document docdata = ReadJsonFile(ResourcePath::makePath("json/repairbuild.json"));
	rapidjson::Value& alldata = docdata["rbs"];
	for (unsigned int i = 0; i < alldata.Size(); i++)
	{
		rapidjson::Value& jsonvalue = alldata[i];
		if (jsonvalue.IsObject())
		{
			S_BUILDINREPAIR data;
			rapidjson::Value& v = jsonvalue["name"];
			data.name = v.GetString();

			v = jsonvalue["rbres"];

			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_repairres.push_back(v[m].GetString());
			}
			v = jsonvalue["rbawd"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_adws.push_back(v[m].GetString());
			}
			data.state = 0;
			data.repairtime = 0;
			map_buildingrepairdata[data.name] = data;
		}
	}

	std::string str = DataSave::getInstance()->getBuildingBroken();

	if (str.length() > 0)
	{
		std::map<std::string, S_BUILDINREPAIR>::iterator bbit = GlobalInstance::map_buildingrepairdata.begin();
		std::vector<std::string> vec_onbuilding;
		CommonFuncs::split(str, vec_onbuilding, ";");

		for (unsigned int i = 0; i < vec_onbuilding.size(); i++)
		{
			std::vector<std::string> vec_;
			CommonFuncs::split(vec_onbuilding[i], vec_, ",");
			GlobalInstance::map_buildingrepairdata[bbit->first].state = atoi(vec_[0].c_str());
			GlobalInstance::map_buildingrepairdata[bbit->first].repairtime = atoi(vec_[1].c_str());
			bbit++;
		}
	}
}

void GlobalInstance::setBuildingBroken()
{
	std::map<std::string, S_BUILDINREPAIR>::iterator bbit;

	std::string str;
	for (bbit = GlobalInstance::map_buildingrepairdata.begin(); bbit != GlobalInstance::map_buildingrepairdata.end(); bbit++)
	{
		std::string onestr = StringUtils::format("%d,%d", bbit->second.state, bbit->second.repairtime);
		
		if (str.length() > 0)
			str.append(";");
		str.append(onestr);
	}
	if (str.length() > 0)
	{
		DataSave::getInstance()->setBuildingBroken(str);
	}
}

void GlobalInstance::resetData()
{
	MyRes::deleteAllRes();
	Quest::restData();
	for (unsigned int i = 0; i < vec_myHeros.size(); i++)
	{
		delete vec_myHeros[i];
		vec_myHeros[i] = NULL;
	}
	vec_myHeros.clear();

	for (unsigned int i = 0; i < vec_rand3Heros.size(); i++)
	{
		delete vec_rand3Heros[i];
		vec_rand3Heros[i] = NULL;
	}
	vec_rand3Heros.clear();

	vec_herosAttr.clear();

	for (unsigned int i = 0; i < vec_resCreators.size(); i++)
	{
		delete vec_resCreators[i];
		vec_resCreators[i] = NULL;
	}
	vec_resCreators.clear();

	map_AllResources.clear();

	map_EquipSuit.clear();

	map_Equip.clear();

	map_GF.clear();

	map_NpcAttrData.clear();

	map_Npcs.clear();

	vec_TaskMain.clear();

	vec_TaskBranch.clear();

	map_DTdata.clear();

	for (int i = 0; i < 6; i++)
	{
		if (myCardHeros[i] != NULL)
		{
			myCardHeros[i] = NULL;
		}

		if (myOnChallengeHeros[i] != NULL)
		{
			myOnChallengeHeros[i] = NULL;
		}
		if (matchPairHeros[i] != NULL)
		{
			matchPairHeros[i] = NULL;
		}
	}
	
	map_mapsdata.clear();

	map_TBoxs.clear();

	vec_messsages.clear();

	vec_notice.clear();

	map_eventdata.clear();

	vec_achievedata.clear();

	vec_shopdata.clear();

	map_buyVipDays.clear();

	vec_buyVipIds.clear();

	map_npcrelation.clear();

	map_myfriendly.clear();

	npcmasterfinish = 0;

	servertime = 0;
	refreshHeroTime = 0;
	refreshResTime = 0;

	refreshMarketTime = 0;

	totalFarmercount = 0;


	mySoliverCount.setValue(0);
	myCoinCount.setValue(0);

	unlockchapter = 1;

	myOutMapCarry = 100;

	silverRefHeroCount = 0;

	resetSilverRefHeroCountTime = 0;

	isNewHeroRefresh = false;

	isCheat = false;

	upgradeurl = "";

	loginData.logindays = 0;
	loginData.isGeted = false;
	map_timeMartData.clear();
	vec_rebateisget.clear();
	vec_stardata.clear();

	curMapFinishStars = 0;
}

std::string GlobalInstance::getUserDefaultXmlString(int type)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return FileUtils::getInstance()->getStringFromFile(UserDefault::getInstance()->getXMLFilePath());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
	if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "getUserDefaultXmlString", "(I)Ljava/lang/String;"))
	{
		jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, type);
		ret = methodInfo.env->GetStringUTFChars(jstr, 0);
	}
	return ret;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return getUserDefaultXml(type);
#endif
}

void GlobalInstance::cleanUserDefaultXmlData()
{
	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();

	std::string content = getUserDefaultXmlString(0);

	int err = pDoc->Parse(content.c_str());
	if (err != 0)
	{
		delete pDoc;
		return;
	}
	tinyxml2::XMLElement *rootEle = pDoc->RootElement();
	//int total = rootEle->IntAttribute("total");
	//获取第一个节点属性
	//const XMLAttribute *attribute = rootEle->FirstAttribute();
	//打印节点属性名和值
	//log("attribute<em>name = %s,attribute</em>value = %s", attribute->Name(), attribute->Value());</p>
	tinyxml2::XMLElement *element = rootEle->FirstChildElement();
	while (element != NULL)
	{
		std::string key = element->Name();
		if (key.compare("jhpayrebate") != 0)
			UserDefault::getInstance()->deleteValueForKey(element->Name());
		element = element->NextSiblingElement();
	}

	UserDefault::getInstance()->flush();

	delete pDoc;
}

void GlobalInstance::upgradeApp(std::string url)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	std::string ret;
	JniMethodInfo methodInfo;
	std::string clsname = StringUtils::format("%s/AppActivity", ANDOIRJNICLS);
	if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "upgradeApk", "(Ljava/lang/String;)V"))
	{
		jstring str1 = methodInfo.env->NewStringUTF(url.c_str());
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, str1);
	}
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	openAppUri(url.c_str());
#endif
}

