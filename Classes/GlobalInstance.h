/****************************************************************
静态数据操作类
****************************************************************/
#ifndef _STATIC_DATA_H_
#define _STATIC_DATA_H_
#include "cocos2d.h"
#include "Hero.h"
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

class GlobalInstance
{
public:
	GlobalInstance();
	~GlobalInstance();
	/****************************
	类实例，单例模式
	****************************/
	static GlobalInstance* getInstance();
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

private:
	static GlobalInstance* _Context;//类实例
public:
	static std::vector<Hero*> GlobalInstance::vec_myHeros;//招募的英雄
	static std::vector<Hero*> vec_rand3Heros;//待招募的3个英雄

	static std::vector<S_HeroAttr> vec_herosAttr;//英雄基础属性

	static int servertime;//服务器时间
private:
	static int refreshHeroTime;

};
#endif

