/********************************************************************
* 基础资源生产
*********************************************************************/
#ifndef _RESCREATOR_H_
#define _RESCREATOR_H_
#include "cocos2d.h"
#include "json.h"
#include "DynamicValue.h"
USING_NS_CC;

class ResCreator
{
public:
	ResCreator(std::string strid);
	~ResCreator();

	CC_SYNTHESIZE(DynamicValueInt, m_farmersCount, FarmersCount);//工人人数
	CC_SYNTHESIZE(DynamicValueInt, m_lv, Lv);//等级
	CC_SYNTHESIZE(std::string, m_name, Name);//名称ID
public:
	//当前等级最大的工人数量
	DynamicValueInt getMaxFarmersCount();

	DynamicValueInt getMaxFarmersCountByLv(int lv);

	//最大容量
	DynamicValueInt getMaxCap(int lv);

	//保存当前数据
	void saveData();
private:
	DynamicValueInt m_maxFarmersCount;//最大工人数
	DynamicValueInt m_maxCap;//最大容量
};
#endif

