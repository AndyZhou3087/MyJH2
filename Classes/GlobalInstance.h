/****************************************************************
静态数据操作类
****************************************************************/
#ifndef _STATIC_DATA_H_
#define _STATIC_DATA_H_
#include "cocos2d.h"
#include "Hero.h"
USING_NS_CC;


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
	//语言
	CC_SYNTHESIZE(int, m_lang, Lang);

	//保存英雄数据到本地
	void saveHeros();

	//从本地读取英雄数据
	void loadHeros();
private:
	static GlobalInstance* _Context;//类实例
public:
	static std::map<std::string, Hero*> map_myHeros;//招募的英雄
	static std::vector<Hero*> vec_rand3Heros;//待招募的3个英雄
};
#endif

