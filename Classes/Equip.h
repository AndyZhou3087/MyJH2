/********************************************************************
* 装配资源类，包括武器，防具，护手，首饰
*********************************************************************/
#ifndef _EQUIP_H_
#define _EQUIP_H_
#include "cocos2d.h"
#include "DynamicValue.h"
#include "Equipable.h"
USING_NS_CC;

class Equip: public Equipable
{
public:
	Equip();
	~Equip();

public:
	std::vector<std::string> vec_stones;//佩戴的宝石

	//攻击
	float getAtk();

	//防御
	float getDf();

	//血量
	float getHp();

	//防御
	float getSuitDf();

	//血量
	float getSuitHp();

	//攻击速度
	float getAtkSpeed();

	//暴击
	float getCrit();

	//闪避
	float getDodge();
};
#endif