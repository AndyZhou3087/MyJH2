﻿/********************************************************************
* 地图块
*********************************************************************/
#ifndef _MAPBLOCK_H_
#define _MAPBLOCK_H_
#include "cocos2d.h"
#include "json.h"
USING_NS_CC;

typedef enum
{
	B_START = 0,//起点
	B_MONSTER,//怪物
	B_NPC,//NPC
	B_BOSS,//BOSS
	B_TBOSS//特殊BOSS
}BLOCKPOSTYPE;

/*************
3个属性的结构体
第一个属性string
第二个属性int
第三个属性int
*************/
typedef struct
{
	std::string sid;
	int intPara1;
	int intPara2;
}ThrProperty;

typedef struct
{
	std::string content;//内容
	std::vector<ThrProperty> vec_getRes;//获得的资源
	ThrProperty lostRes;//失去资源
	std::string effectNpc;//影响npc
	std::string result;//结果
}ChoiceData;

class MapBlock : public Sprite
{
public:
	MapBlock();
	~MapBlock();
	static MapBlock* create(int row, int col, std::string boardName, std::string builidname);
	bool init(int row, int col, std::string boardName, std::string builidname);
	CC_SYNTHESIZE(bool, m_walkable, Walkable);//是否可行走
	CC_SYNTHESIZE(int, m_postype, PosType);//每个地图块位置信息
	CC_SYNTHESIZE(std::string, m_posnpcid, PosNpcID);//每个地图块NPCID
	CC_SYNTHESIZE(int, m_posnpcrnd, PosNpcRnd);//每个地图块NPCID出现的概率
private:
public:
	std::vector<int> vec_eventrnd;//7个事件概率
	ThrProperty monsters[6];//6个怪物数据
	std::vector<ThrProperty> vec_RewardsRes;//获得的物品
	std::vector<ChoiceData> vec_choiceDatas;//任务选择获得的物品
	int Col;//所在列
	int Row;//转换成左上后所在行，编辑器是左上开始。比如编辑器0，0，应该对应 总行-1 - 0(编辑所在行)

};

#endif

