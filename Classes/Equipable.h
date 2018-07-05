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

	CC_SYNTHESIZE(DynamicValueInt, m_qu, QU);//品质
	CC_SYNTHESIZE(std::string, m_whos, Whos);//谁装配上了
	DynamicValueInt getLv();
public:
};
#endif