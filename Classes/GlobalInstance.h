/****************************************************************
静态数据操作类
****************************************************************/
#ifndef _STATIC_DATA_H_
#define _STATIC_DATA_H_
#include "cocos2d.h"
#include "Hero.h"
#include "ResCreator.h"
#include "ResBase.h"
USING_NS_CC;

typedef struct
{
	std::string id;
	std::string name;
	std::vector<int> vec_exp;//升级所需经验值
	std::vector<float> vec_atk;//攻击
	std::vector<float> vec_df;//防御
	std::vector<float> vec_maxhp;//最大血量
	std::vector<float> vec_avoid;//闪避
	std::vector<float> vec_crit;//暴击
	std::vector<float> vec_atkspeed;//攻击速度
}S_HeroAttr;

//小地图数据
typedef struct
{
	std::string id;
	int ph;//小地图消耗的精力，体力
	std::vector<std::string> vec_awd;//可能获得物品
}S_SubMap;

//大地图数据
typedef struct
{
	std::string id;
	std::map<std::string, S_SubMap> map_sublist;//小地图list
}S_MainMap;

typedef struct
{
	std::string id;
	std::string name;
	std::string desc;
	std::vector<std::map<std::string, int>> vec_needres;//合成需要的资源
}AllResources;

class GlobalInstance
{
public:
	GlobalInstance();
	~GlobalInstance();
	/****************************
	类实例，单例模式
	****************************/
	static GlobalInstance* getInstance();

	/****************************
	获取设备唯一ID
	*****************************/
	static std::string UUID();

	/****************************
	版本code
	*****************************/
	std::string getVersionCode();

	/****************************
	包名com.csfb.jh
	*****************************/
	std::string getPackageName();

	/****************************
	渠道号
	*****************************/
	std::string getChannelId();

	int getSysSecTime();

	int createRandomNum(int val);
public:

	//加载初始化数据

	void loadInitData();
	//语言
	CC_SYNTHESIZE(int, m_lang, Lang);

	//保存英雄数据到本地
	void saveMyHeros();

	//从本地读取英雄数据
	void loadMyHeros();

	//加载英雄属性数据
	void loadHerosAttrData();

	//保存随机生成的3个英雄数据到本地
	void saveRand3Heros();

	//从本地读取随机生成的3个英雄数据
	void loadRand3Heros();

	//保存刷新英雄时间
	void saveRefreshHeroTime(int time);

	//设置刷新英雄时间
	int getRefreshHeroTime();

	//检查是否重名
	bool checkifSameName(std::string nickname);


	//保存刷新资源时间
	void saveRefreshResTime(int time);

	//设置刷新资源时间
	int getRefreshResTime();

	//解析生产资源数据
	void loadResCreatorData();

	//保存生产资源数据
	void saveResCreatorData();

	//加载资源名称
	void loadAllResourcesData();

	//加载我的资源
	void loadMyResData();

	//总的工人数
	int getTotalFarmers();

	//总的工人数
	void saveTotalFarmers(int val);

	//正在工作的工人数
	int getWorkingFarmerCount();

	//解雇英雄
	void fireHero(int index);

	//英雄总的负重
	int getTotalCaryy();

	void parseMapJson();

	DynamicValueInt getMySoliverCount();

	void addMySoliverCount(DynamicValueInt val);

	DynamicValueInt getMyCoinCount();

	void addMyCoinCount(DynamicValueInt val);
private:
	static GlobalInstance* _Context;//类实例
public:
	static std::vector<Hero*> vec_myHeros;//招募的英雄
	static std::vector<Hero*> vec_rand3Heros;//待招募的3个英雄

	static std::vector<S_HeroAttr> vec_herosAttr;//英雄基础属性

	static std::vector<ResCreator*> vec_resCreators;//资源生产

	static std::map<std::string, AllResources> map_AllResources;//资源名字

	static int servertime;//服务器时间

	static Hero* myCardHeros[6];//出城选择的6个英雄

	static std::map<std::string, S_MainMap> map_mapsdata;//地图数据
private:
	static int refreshHeroTime;
	static int refreshResTime;
	static int totalFarmercount;
	static DynamicValueInt mySoliverCount;
	static DynamicValueInt myCoinCount;

};
#endif

