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
	CC_SYNTHESIZE(DynamicValueInt, m_exp, Exp); //等级
	CC_SYNTHESIZE(int, m_vocation, Vocation);// 职业
	CC_SYNTHESIZE(int, m_potential, Potential);//潜力，品质
	CC_SYNTHESIZE(int, m_sex, Sex);//性别
	int getLevel();
	int getAtk();
	int getDf();
	int getHp();
	int getAtkSpeed();
	int getCrit();
	int getDodge();
private:

public:
	/********************************************************************
	* 
	*********************************************************************/
};
#endif

