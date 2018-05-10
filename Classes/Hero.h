/********************************************************************
* 英雄
*********************************************************************/
#ifndef _HERO_H_
#define _HERO_H_
#include "cocos2d.h"
#include "json.h"
#include "DynamicValue.h"
USING_NS_CC;

#define HEROMAX 4
class Hero
{
public:
	Hero();
	~Hero();

	CC_SYNTHESIZE(std::string, m_name, Name);//英雄名称，唯一不重复
	CC_SYNTHESIZE(DynamicValueInt, m_exp, Exp); //经验值
	CC_SYNTHESIZE(int, m_vocation, Vocation);// 职业
	CC_SYNTHESIZE(int, m_potential, Potential);//潜力，品质
	CC_SYNTHESIZE(int, m_sex, Sex);//性别
	CC_SYNTHESIZE(bool, m_isrecruited, IsRecruited);//是否招募

	//等级，根据经验值算等级
	int getLevel();

	//攻击
	int getAtk();

	//防御
	int getDf();

	//血量
	int getHp();

	//攻击速度
	int getAtkSpeed();

	//暴击
	int getCrit();

	//闪避
	int getDodge();

	//随机生成一个英雄
	void generate();
private:
	//随机生成一个名字
	std::string generateName();
public:
	/********************************************************************
	* 
	*********************************************************************/
};
#endif

