/********************************************************************
* 功法资源类，包括内功，外功
*********************************************************************/
#ifndef _GONGFA_H_
#define _GONGFA_H_
#include "cocos2d.h"
#include "DynamicValue.h"
#include "Equipable.h"
USING_NS_CC;

class GongFa : public Equipable
{
public:
	GongFa();
	~GongFa();

	CC_SYNTHESIZE(DynamicValueInt, m_exp, Exp);//经验值
	DynamicValueInt getLv();
public:

	//攻击
	float getAtk();

	//防御
	float getDf();

	//血量
	float getHp();

	//攻击速度
	float getAtkSpeed();

	//暴击
	float getCrit();

	//闪避
	float getDodge();
};
#endif