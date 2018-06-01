
/********************************************************************
*数据存储操作，保存到本地
*********************************************************************/
#ifndef _DATA_SAVE_H_
#define _DATA_SAVE_H_
#include "cocos2d.h"
class DataSave
{
public:
	/****************************
	类实例，单例模式
	****************************/
	static DataSave* getInstance();

	/****************************
	释放
	****************************/
	static void purgeGameSave();

	/****************************
	获取建筑物等级
	****************************/
	int getBuildLv(std::string name);

	/****************************
	设置建筑物等级，保存到本地
	****************************/
	void setBuildLv(std::string name, int val);

	/****************************
	获取语言类型，默认简体中文
	****************************/
	int getLocalLang();

	/****************************
	设置语言类型
	****************************/
	void setLocalLang(int val);

	/****************************
	删除本地英雄数据
	@para: heroindex 英雄对于的索引
	****************************/
	void deleteLocalHero(int heroindex);

	/****************************
	获取英雄数据
	@para key, eg:hero1,列表顺序
	****************************/
	std::string getHeroData(std::string herokey);

	/****************************
	设置英雄数据
	@para key, eg:hero1,列表顺序
	@para data,组织好的一段字串
	****************************/
	void setHeroData(std::string herokey, std::string herodata);

	/****************************
	获取随机的3个英雄数据
	****************************/
	std::string getRand3HeroData();

	/****************************
	设置随机的3个英雄数据
	@para data,组织好的一段字串
	****************************/
	void setRand3HeroData(std::string herodata);
	
	/****************************
	保存刷新时间
	****************************/
	int getRefreshHeroTime();

	/****************************
	设置刷新时间
	****************************/
	void setRefreshHeroTime(int time);

	/****************************
	保存资源刷新时间
	****************************/
	int getRefreshResTime();

	/****************************
	设置资源刷新时间
	****************************/
	void setRefreshResTime(int time);

	/****************************
	保存生产者数据
	****************************/
	void setResCreatorData(std::string val);

	/***************************
	设置生产者数据
	****************************/
	std::string getResCreatorData();

	/****************************
	保存工人总数
	****************************/
	int getFarmersCount();

	/****************************
	设置工人总数
	****************************/
	void setFarmersCount(int count);

	/****************************
	保存我的资源数据
	****************************/
	void setMyRes(std::string strval);

	/****************************
	获取我的资源数据
	****************************/
	std::string getMyRes();

	/****************************
	获取招募的工人总数
	****************************/
	int getTotalFarmers();

	/****************************
	保存招募的工人总数
	****************************/
	void setTotalFarmers(int val);

	/****************************
	获取建筑物等级
	@para 建筑物名称
	****************************/
	int getBuildingLv(std::string buildingname);

	/****************************
	保存建筑物等级
	@para 建筑物名称
	@para 建筑物等级
	****************************/
	void setBuildingLv(std::string buildingname, int val);

private:
	static DataSave* _Context;//类实例

private:

};
#endif

