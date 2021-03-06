﻿#include "DataSave.h"
#include "cocos2d.h"
#include "MD5.h"
#include "Utility.h"
#include "Const.h"

DataSave* DataSave::_Context = NULL;

DataSave* DataSave::getInstance() {
	if (_Context == NULL) {
		_Context = new DataSave;
	}
	return _Context;
}

void DataSave::purgeGameSave() {
	CC_SAFE_DELETE(_Context);
}

int DataSave::getLocalLang()
{
	return loadIntDataByKey("lang", 1);
}

void DataSave::setLocalLang(int val)
{
	saveIntDataByKey("lang", val);
}

void DataSave::deleteLocalHero(int heroindex)
{
	std::string key = StringUtils::format("hero%d", heroindex);
	removeEleByKey(key);
}

std::string DataSave::getHeroData(std::string herokey)
{
	return loadStringDataByKey(herokey, "");
}

void DataSave::setHeroData(std::string herokey, std::string herodata)
{
	saveStringDataByKey(herokey, herodata);
}

std::string DataSave::getRand3HeroData()
{
	return loadStringDataByKey("randheros", "");
}

void DataSave::setRand3HeroData(std::string herodata)
{
	saveStringDataByKey("randheros", herodata);
}

int DataSave::getRefreshHeroTime()
{
	return loadIntDataByKey("refreshherotime", 0);
}

void DataSave::setRefreshHeroTime(int time)
{
	saveIntDataByKey("refreshherotime", time);
}

int DataSave::getRefreshResTime()
{
	return loadIntDataByKey("refreshrestime", 0);
}

void DataSave::setRefreshResTime(int time)
{
	saveIntDataByKey("refreshrestime", time);
}

std::string DataSave::getResCreatorData()
{
	return loadStringDataByKey("rescreator", "0-0");
}

void DataSave::setResCreatorData(std::string val)
{
	saveStringDataByKey("rescreator", val);
}

int DataSave::getFarmersCount()
{
	return loadIntDataByKey("creatorfarmers", 0);
}

void DataSave::setFarmersCount(int count)
{
	saveIntDataByKey("creatorfarmers", count);
}

void DataSave::setMyRes(std::string strval)
{
	saveStringDataByKey("myres", strval);
}

std::string DataSave::getMyRes()
{
	return loadStringDataByKey("myres", defaultres);
}

void DataSave::setMyBranchTask(std::string strval)
{
	saveStringDataByKey("BranchTask", strval);
}

std::string DataSave::getMyBranchTask()
{
	return loadStringDataByKey("BranchTask", "");//id-状态
}

void DataSave::setMyMainTask(std::string strval)
{
	saveStringDataByKey("MainTask", strval);
}

std::string DataSave::getMyMainTask()
{
	return loadStringDataByKey("MainTask", "");//id-状态-type
}

void DataSave::setMyCurTaskNeed(std::string strval)
{
	saveStringDataByKey("CurTaskNeed", strval);
}

std::string DataSave::getMyCurTaskNeed()
{
	return loadStringDataByKey("CurTaskNeed", "");//id-count
}

void DataSave::setMyCurBranchNeed(std::string strval)
{
	saveStringDataByKey("CurBranchNeed", strval);
}

std::string DataSave::getMyCurBranchNeed()
{
	return loadStringDataByKey("CurBranchNeed", "");//id-count
}

void DataSave::setMyAchieveData(std::string strval)
{
	saveStringDataByKey("achieve", strval);
}

std::string DataSave::getMyAchieveData()
{
	return loadStringDataByKey("achieve", "");//id-state
}

//成就任务次数
void DataSave::setAchieveTypeCount(std::string strval)
{
	saveStringDataByKey("achieveTypeCount", strval);//8个类型
}

std::string DataSave::getAchieveTypeCount()
{
	return loadStringDataByKey("achieveTypeCount", "0-0-0;1-0-0;2-0-0;3-0-0;4-0-0;5-0-0;6-0-0;7-0-0;8-0-0;9-0-0;10-0-0;11-0-0");//type-count
}

void DataSave::setMyDailyTaskData(std::string strval)
{
	saveStringDataByKey("DailyTask", strval);
}

std::string DataSave::getMyyDailyTaskData()
{
	return loadStringDataByKey("DailyTask", "");//id-state
}

//保存当前时间天数：从1970-1-1开始
void DataSave::setMyFreshDate(int d)
{
	saveIntDataByKey("FreshDate", d);
}

int DataSave::getMyFreshDate()
{
	return loadIntDataByKey("FreshDate", 0);
}

//每日类型任务次数
void DataSave::setDailyTypeCount(std::string strval)
{
	saveStringDataByKey("DailyTypeCount", strval);//8个类型
}

std::string DataSave::getDailyTypeCount()
{
	return loadStringDataByKey("DailyTypeCount", "0-0;1-0;2-0;3-0;4-0;5-0;6-0;7-0;10-0;11-0");//type-count
}

void DataSave::setMyDailyReward(std::string strval)
{
	saveStringDataByKey("DailyReward", strval);//8个类型
}

std::string DataSave::getMyyDailyReward()
{
	return loadStringDataByKey("DailyReward", "50-0;100-0;150-0;200-0");//积分-状态
}

int DataSave::getTotalFarmers()
{
	return loadIntDataByKey("totalfarmers", 5);
}

void DataSave::setTotalFarmers(int val)
{
	saveIntDataByKey("totalfarmers", val);
}

void DataSave::setMyDailyPoint(int point)
{
	saveIntDataByKey("DailyPoint", point);
}

int DataSave::getMyyDailyPoint()
{
	return loadIntDataByKey("DailyPoint", 0);
}

int DataSave::getBuildingLv(std::string buildingname)
{
	return loadIntDataByKey(buildingname, 0);
}

void DataSave::setBuildingLv(std::string buildingname, int val)
{
	saveIntDataByKey(buildingname, val);
}

int DataSave::getMySoliverCount()
{
	return loadIntDataByKey("soliver", 10000);
}

void DataSave::setMySoliverCount(int val)
{
	saveIntDataByKey("soliver", val);
}

int DataSave::getMyCoinCount()
{
	return loadIntDataByKey("coin", 100);
}

void DataSave::setMyCoinCount(int val)
{
	saveIntDataByKey("coin", val);
}

std::string DataSave::getMarketStock()
{
	return loadStringDataByKey("marketstock", "");
}

void DataSave::setMarketStock(std::string val)
{
	saveStringDataByKey("marketstock", val);
}

int DataSave::getRefreshMarketTime()
{
	return loadIntDataByKey("refreshmarkettime", 0);
}

void DataSave::setRefreshMarketTime(int time)
{
	saveIntDataByKey("refreshmarkettime", time);
}

void DataSave::setMapVisibleArea(std::string mapkey, std::string val)
{
	saveStringDataByKey(mapkey, val);
}

std::string DataSave::getMapVisibleArea(std::string mapkey)
{
	return loadStringDataByKey(mapkey, "");
}

int DataSave::getUnlockChapter()
{
	return loadIntDataByKey("unlockc", 1);
}

void DataSave::setUnlockChapter(int val)
{
	saveIntDataByKey("unlockc", val);
}

void DataSave::setHeadId(int val)
{
	saveIntDataByKey("headID", val);
}

int DataSave::getHeadId()
{
	return loadIntDataByKey("headID", 4);
}

bool DataSave::getIsNewerGuide(int index)
{
	std::string key = StringUtils::format("guide%d", index);
	int val = loadIntDataByKey(key, 1);
	return val == 1 ? true : false;
}

void DataSave::setIsNewerGuide(int index, bool val)
{
	std::string key = StringUtils::format("guide%d", index);
	saveIntDataByKey(key, (int)val);
}

void DataSave::setSilverRefHeroCount(int val)
{
	saveIntDataByKey("silverrefhero", val);
}

int DataSave::getSilverRefHeroCount()
{
	return loadIntDataByKey("silverrefhero", 0);
}

void DataSave::setResetSilverRefHeroCountTime(int val)
{
	saveIntDataByKey("silverrefherotime", val);
}

int DataSave::getResetSilverRefHeroCountTime()
{
	return loadIntDataByKey("silverrefherotime", 0);
}

void DataSave::setNpcFriendly(std::string val)
{
	saveStringDataByKey("friendly", val);
}

std::string DataSave::getNpcFriendly()
{
	return loadStringDataByKey("friendly", "");//npcid-好友度-关系
}

bool DataSave::getMapBoxRewards(std::string key)
{
	int val = loadIntDataByKey(key, 0);
	return val == 1 ? true : false;
}

void DataSave::setMapBoxRewards(std::string key, bool val)
{
	saveIntDataByKey(key, val == true ? 1 : 0);
}

int DataSave::getMapOrderCount(std::string mapid)
{
	std::string str = StringUtils::format("order%s", mapid.c_str());
	return loadIntDataByKey(str, 0);
}

void DataSave::setMapOrderCount(std::string mapid, int val)
{
	std::string str = StringUtils::format("order%s", mapid.c_str());
	saveIntDataByKey(str, val);
}

bool DataSave::getUserProtocal()
{
	int val = loadIntDataByKey("UserProtocal", 0);
	return val == 1 ? true : false;
}

void DataSave::setUserProtocal(int val)
{
	saveIntDataByKey("UserProtocal", val);
}
//默认0主场景，1地图外场景，2地图内场景
int DataSave::getExitScene()
{
	return loadIntDataByKey("ExitScene", 0);
}

void DataSave::setExitScene(int val)
{
	saveIntDataByKey("ExitScene", val);
}

void DataSave::setMapScenePos(std::string mapid, int pos)
{
	std::string str = StringUtils::format("%s,%d", mapid.c_str(), pos);
	saveStringDataByKey("ScenePos", str);
}

std::string DataSave::getMapScenePos()
{
	return loadStringDataByKey("ScenePos", "");
}

void DataSave::setHeroMapCarryCount(int val)
{
	saveIntDataByKey("mymapcarry", val);
}

int DataSave::getHeroMapCarryCount()
{
	return loadIntDataByKey("mymapcarry", 100);
}

bool DataSave::getFirstEnter()
{
	int val = loadIntDataByKey("firstenter", 1);
	return val == 1 ? true : false;
}

void DataSave::setFirstEnter(bool val)
{
	saveIntDataByKey("firstenter", val == true ? 1 : 0);
}

int DataSave::getCheatCount()
{
	return loadIntDataByKey("chartcount", 1);
}

void DataSave::setCheatCount(int val)
{
	saveIntDataByKey("chartcount", val);
}
