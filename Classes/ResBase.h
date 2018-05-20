/********************************************************************
* 基础资源类
*********************************************************************/
#ifndef _RESBASE_H_
#define _RESBASE_H_
#include "cocos2d.h"
#include "json.h"
#include "DynamicValue.h"
USING_NS_CC;

class ResBase
{
public:
	ResBase();
	~ResBase();

	CC_SYNTHESIZE(std::string, m_id, Id);//ID
	CC_SYNTHESIZE(std::string, m_name, Name);//名称
	CC_SYNTHESIZE(DynamicValueInt, m_type, Type);//类型
	CC_SYNTHESIZE(DynamicValueInt, m_count, Count);//个数
public:
	//使用，count：个数
	void use(DynamicValueInt count);
	//增加
	void add(DynamicValueInt count);
	void remove();
};
#endif

