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
	T_FASHION,//首饰
	T_WG,//外功
	T_NG,//内功
	T_RENS,//人参，提升经验值
	T_FU,//闭关用的符
	T_DAN,//突破用的丹
	T_MIJI,//参悟秘籍，升级功法
	T_BOX,//宝箱
	T_STONE,//玉，宝石，镶嵌用的
	T_VSION,//视野
	T_GOBACK,//回城卷轴
	T_TLMED,//体力药水
	T_HEROCARD,//抽英雄卷
	T_ARMCARD,//抽武器卷
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
	CC_SYNTHESIZE(DynamicValueInt, m_qu, QU);//品质
public:
};
#endif