/********************************************************************
* 基础资源类
*********************************************************************/
#ifndef _RESBASE_H_
#define _RESBASE_H_
#include "cocos2d.h"
#include "json.h"
#include "DynamicValue.h"
USING_NS_CC;

typedef enum
{
	MYPACKAGE = 0,
	MYSTORAGE,
	MYEQUIP
}RESWHERE;

typedef enum
{
	T_RES_0 = 0,//基础资源
	T_ARMOR,//武器
	T_EQUIP,//防具
	T_HANDARMOR,//护手
	T_WG,//外功
	T_NG,//内功
	T_JSTONE,//首饰，宝石
	T_OTHER
}RESTYPE;
class ResBase
{
public:
	ResBase();
	~ResBase();

	CC_SYNTHESIZE(std::string, m_id, Id);//ID
	CC_SYNTHESIZE(int, m_type, Type);//类型
	CC_SYNTHESIZE(DynamicValueInt, m_count, Count);//个数
	CC_SYNTHESIZE(int, m_where, Where);//在哪里
public:
	//使用，count：个数
	void use(DynamicValueInt count);
	//增加
	void add(DynamicValueInt count);

	void remove();
};
#endif