/********************************************************************
* 建筑物
*********************************************************************/
#ifndef _BUILDING_H_
#define _BUILDING_H_
#include "cocos2d.h"
#include "json.h"
#include "DynamicValue.h"
USING_NS_CC;

class Building
{
public:
	Building();
	~Building();

	std::string name;//建筑物名称
	DynamicValueInt maxlevel; //建筑物最高等级
	DynamicValueInt level;// 建筑物等级
	std::vector<std::vector<std::map<std::string,int>>> lvupres;//建筑升级资源
	std::vector<std::vector<std::string>> vec_exdata;
public:
	/********************************************************************
	* 解析每个建筑物的数据
	@param jsonvalue buildings.json [b]数据中每项的值
	*********************************************************************/
	static void parseData();

	static void destroy();
	//建筑物json数据
	static std::map<std::string, Building*> map_buildingDatas;
};
#endif

