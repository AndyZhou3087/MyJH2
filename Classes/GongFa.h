/********************************************************************
* 功法资源类，包括内功，外功
*********************************************************************/
#ifndef _GONGFA_H_
#define _GONGFA_H_
#include "cocos2d.h"
#include "DynamicValue.h"
#include "ResBase.h"
USING_NS_CC;

class GongFa : public ResBase
{
public:
	GongFa();
	~GongFa();

	CC_SYNTHESIZE(DynamicValueInt, m_exp, Exp);//经验值
	DynamicValueInt getLv();
public:
};
#endif