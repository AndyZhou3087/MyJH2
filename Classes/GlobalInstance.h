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
	int saleval;//价格
}AllResources;

typedef struct
{
	std::string id;
	std::string name;
	std::vector<float> vec_bns;//额外加成
	int maxhp;
	int atk;
	int df;
	float avoid;//闪避率
	float crit;//暴击率
	float speed;//攻击速度
}EquipData;

typedef enum
{
	//是否完成此任务，0已接受未完成，1未接受任务，2已完成未领取，3已领取奖励
	MAIN_ACC = 0,
	MAIN_TASK,
	MAIN_FINISH,
	MAIN_GET
}TASKMAINSTATE;

typedef enum
{
	//1表示给东西，2表示战斗,二选一即可完成任务(只有1和2两种类型)
	MAIN_GIVE = 1,
	MAIN_FIGHT
}TASKMAINTYPE;

typedef struct
{
	int id;
	std::string name;
	std::vector<int> type;//条件二选一，1表示给东西，2表示战斗,二选一即可完成任务(只有1和2两种类型)
	std::string desc;
	std::string place;
	std::string npcid;
	std::string bossword;
	std::vector<std::map<std::string, int>> need1;//条件二选一，物品，r001-10(id-count)
	std::string need1desc;
	int need2;//条件二选一，战斗，填写type
	std::string need2desc;
	std::vector<int> mutex1;//互斥1，1-1,id为1的type为1的互斥
	std::vector<int> mutex2;//互斥2
	std::vector<std::vector<std::string>> reward1;//条件1的奖励
	std::vector<std::vector<std::string>> reward2;//条件2的奖励
	int isfinish;//是否完成此任务，0已接受未完成，1未接受任务，2已完成未领取，3已领取奖励
	int finishtype;//完成任务类型，1表示条件1完成，2表示条件2完成
}TaskMainData;

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

	//加载装备资源
	void loadEquipData();

	//加载主线任务
	void loadTaskMainData();

	//加载已完成主线任务
	void loadMyTaskMainData();

	//保存已完成主线任务
	void saveMyTaskMainData();

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

	void costMySoliverCount(DynamicValueInt val);

	DynamicValueInt getMyCoinCount();

	void addMyCoinCount(DynamicValueInt val);

	void costMyCoinCount(DynamicValueInt val);

	//根据装备品质生成镶嵌孔数
	int generateStoneCount(int qu);

	//保存刷新市场时间
	void saveRefreshMarketTime(int time);

	//设置刷新资源时间
	int getRefreshMarketTime();

	//主线任务进行排序
	static bool larger_callback(TaskMainData a, TaskMainData b);
private:
	static GlobalInstance* _Context;//类实例
public:
	static std::vector<Hero*> vec_myHeros;//招募的英雄
	static std::vector<Hero*> vec_rand3Heros;//待招募的3个英雄

	static std::vector<S_HeroAttr> vec_herosAttr;//英雄基础属性

	static std::vector<ResCreator*> vec_resCreators;//资源生产

	static std::map<std::string, AllResources> map_AllResources;//资源名字

	static std::map<std::string, EquipData> map_Equip;//装备名字

	static std::vector<TaskMainData> vec_TaskMain;//主线任务

	static TaskMainData myCurMainData;//当前主线任务

	static int servertime;//服务器时间

	static Hero* myCardHeros[6];//出城选择的6个英雄

	static std::map<std::string, S_MainMap> map_mapsdata;//地图数据
private:
	static int refreshHeroTime;
	static int refreshResTime;
	static int refreshMarketTime;
	static int totalFarmercount;
	static DynamicValueInt mySoliverCount;
	static DynamicValueInt myCoinCount;

};
#endif

