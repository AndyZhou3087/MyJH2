/********************************************************************
* 可装配资源类，包括武器，防具，护手，首饰，武功，内功
*********************************************************************/
#ifndef _EQUIPABLE_H_
#define _EQUIPABLE_H_
#include "cocos2d.h"
#include "DynamicValue.h"
#include "ResBase.h"
USING_NS_CC;

class Equipable : public ResBase
{
public:
	Equipable();
	~Equipable();

	CC_SYNTHESIZE(std::string, m_whos, Whos);//谁装配上了
	virtual DynamicValueInt getLv();
	virtual void setLv(DynamicValueInt val);
public:
	//攻击
	virtual float getAtk();

	//防御
	virtual float getDf();

	//血量
	virtual float getHp();

	//攻击速度
	virtual float getAtkSpeed();

	//暴击
	virtual float getCrit();

	//闪避
	virtual float getDodge();
public:
	DynamicValueInt m_lv;
};
#endif