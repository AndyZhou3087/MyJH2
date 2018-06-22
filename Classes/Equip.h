/********************************************************************
* 装配资源类，包括武器，防具，护手，首饰
*********************************************************************/
#ifndef _EQUIP_H_
#define _EQUIP_H_
#include "cocos2d.h"
#include "DynamicValue.h"
#include "ResBase.h"
USING_NS_CC;

class Equip: public ResBase
{
public:
	Equip();
	~Equip();

	CC_SYNTHESIZE(DynamicValueInt, m_qu, QU);//品质
	CC_SYNTHESIZE(DynamicValueInt, m_lv, Lv);//强化等级
public:
	std::vector<std::string> vec_stones;//佩戴的宝石
};
#endif