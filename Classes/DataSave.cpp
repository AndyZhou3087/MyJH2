#include "DataSave.h"
#include "cocos2d.h"
#include "MD5.h"
#include "Utility.h"

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
	return loadStringDataByKey("myres", "r001-10000-1;r002-10000-1;r003-10000-1;r004-10000-1;r005-10000-1");
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
	return loadIntDataByKey("soliver", 50000000);
}

void DataSave::setMySoliverCount(int val)
{
	saveIntDataByKey("soliver", val);
}

int DataSave::getMyCoinCount()
{
	return loadIntDataByKey("coin", 50000000);
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
}

void DataSave::setHeadId(std::string val)
{
	saveStringDataByKey("headID", val);
}

std::string DataSave::getHeadId()
{
	return loadStringDataByKey("headID", "images/cardh_0_0");
}