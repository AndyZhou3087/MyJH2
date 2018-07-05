#include "GlobalInstance.h"
#include "DataSave.h"
#include "Hero.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MyRes.h"
#include "Const.h"
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

std::vector<TaskMainData> GlobalInstance::map_TaskMain;
std::vector<MyTaskMainData> GlobalInstance::map_myTaskMain;

TaskMainData* GlobalInstance::myCurTaskMain;

int GlobalInstance::servertime = 0;
int GlobalInstance::refreshHeroTime = 0;
int GlobalInstance::refreshResTime = 0;

int GlobalInstance::refreshMarketTime = 0;

int GlobalInstance::totalFarmercount = 0;

Hero* GlobalInstance::myCardHeros[6];

std::map<std::string, S_MainMap> GlobalInstance::map_mapsdata;

DynamicValueInt GlobalInstance::mySoliverCount;
DynamicValueInt GlobalInstance::myCoinCount;

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
	return "00000000-0000-0000-0000-000000000000";
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

	for (int i = 0; i < 6; i++)
	{
		myCardHeros[i] = NULL;
	}

	GlobalInstance::mySoliverCount.setValue(DataSave::getInstance()->getMySoliverCount());
	GlobalInstance::myCoinCount.setValue(DataSave::getInstance()->getMyCoinCount());
}

void GlobalInstance::saveMyHeros()
{
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		std::string herokey = StringUtils::format("hero%d", i);
		Hero* hero = GlobalInstance::vec_myHeros[i];
		std::string datastr = StringUtils::format("%s-%d-%d-%d-%d-%.2f-%d-%d-%.2f;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex(), hero->getRandAttr(), hero->getState(), hero->getPos(), hero->getHp());
		DataSave::getInstance()->setHeroData(herokey, datastr);
	}
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
				if (vec_tmp.size() >= 8)
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
					int pos = atoi(vec_tmp[7].c_str());
					hero->setPos(pos);
					if (pos > 0)
					{
						GlobalInstance::myCardHeros[pos - 1] = hero;
					}

					hero->setHp(atof(vec_tmp[8].c_str()));
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
		std::string onestr = StringUtils::format("%s-%d-%d-%d-%d-%.02f-%d;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex(), hero->getRandAttr(), hero->getState());
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

			v = jsonvalue["name"];
			data.name = v.GetString();

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
				data.vec_atkspeed.push_back(1.0f/v[m].GetDouble());
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

void GlobalInstance::loadResCreatorData()
{
	std::string str = DataSave::getInstance()->getResCreatorData();
	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ";");
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
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
			TaskMainData data;
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
			data.need2 = atoi(v.GetString());

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
				if (onestr.length() > 3)
				{
					std::map<std::string, int> map_tmp;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					map_tmp[vec_tmp[0]] = atoi(vec_tmp[1].c_str());
					data.reward1.push_back(map_tmp);
				}
			}

			v = jsonvalue["reward2"];
			for (unsigned int i = 0; i < v.Size(); i++)
			{
				std::string onestr = v[i].GetString();
				if (onestr.length() > 3)
				{
					std::map<std::string, int> map_tmp;
					std::vector<std::string> vec_tmp;
					CommonFuncs::split(onestr, vec_tmp, "-");
					map_tmp[vec_tmp[0]] = atoi(vec_tmp[1].c_str());
					data.reward2.push_back(map_tmp);
				}
			}
			
			data.isfinish = 0;
			data.finishtype = -1;
			data.isGetReward = 0;

			map_TaskMain.push_back(data);
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
		for (int i = 0; i < vec_tmp.size(); i++)
		{
			std::vector<std::string> vec_one;
			CommonFuncs::split(vec_tmp[i], vec_one, "-");

			MyTaskMainData data;

			data.id = atoi(vec_one[0].c_str());
			data.isfinish = atoi(vec_one[1].c_str());
			data.type = atoi(vec_one[2].c_str());
			data.isGetReward = atoi(vec_one[3].c_str());
			map_myTaskMain.push_back(data);
		}

		myCurTaskMain = &map_TaskMain[vec_tmp.size()];
	}
	else
	{
		myCurTaskMain = &map_TaskMain[0];
	}
}

void GlobalInstance::saveMyTaskMainData()
{
	std::string str;
	for (unsigned i = 0; i < GlobalInstance::map_myTaskMain.size(); i++)
	{
		std::string onestr = StringUtils::format("%d-%d-%d-%d;", GlobalInstance::map_myTaskMain[i].id, GlobalInstance::map_myTaskMain[i].isfinish, GlobalInstance::map_myTaskMain[i].type, GlobalInstance::map_myTaskMain[i].isGetReward);
		str.append(onestr);
	}
	DataSave::getInstance()->setMyMainTask(str.substr(0, str.length() - 1));
}

void GlobalInstance::saveResCreatorData()
{
	std::string str;
	for (unsigned i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
	{
		std::string onestr = StringUtils::format("%d-%d;", GlobalInstance::vec_resCreators[i]->getLv().getValue(), GlobalInstance::vec_resCreators[i]->getFarmersCount().getValue());
		str.append(onestr);
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

			v = jsonvalue["name"];
			data.name = v.GetString();

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
				map_AllResources[rid].saleval = atoi(v.GetString());
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

			std::string types[] = { "r","a","e","h","f","w","x","s","c","d","m","b","y" };
			int m = 0;
			for (; m < sizeof(types) / sizeof(types[0]); m++)
			{
				if (rid.compare(0, 1, types[m]) == 0)
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
	//先删除掉所有本地英雄节点数据
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		DataSave::getInstance()->deleteLocalHero(i);
	}

	//释放内存
	delete GlobalInstance::vec_myHeros[index];
	//删除当前英雄列表
	GlobalInstance::vec_myHeros.erase(GlobalInstance::vec_myHeros.begin() + index);

	//保存数据
	GlobalInstance::getInstance()->saveMyHeros();
}

int GlobalInstance::getTotalCaryy()
{
	return 100;
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

		v = jsonvalue["awd"];
		for (unsigned int i = 0; i < v.Size(); i++)
		{
			s_submap.vec_awd.push_back(v[i].GetString());
		}

		std::string mainid = s_submap.id.substr(0, s_submap.id.find_last_of("-"));
		map_mapsdata[mainid].map_sublist[s_submap.id] = s_submap;
	}
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

void GlobalInstance::saveRefreshMarketTime(int time)
{
	refreshMarketTime = time;
	DataSave::getInstance()->setRefreshMarketTime(time);
}


int GlobalInstance::getRefreshMarketTime()
{
	return refreshMarketTime;
}