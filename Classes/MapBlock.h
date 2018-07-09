/********************************************************************
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
第四个属性float ,概率之类的
*************/
typedef struct
{
	std::string sid;
	int intPara1;
	int intPara2;
	float floatPara3;
}FOURProperty;

typedef struct
{
	std::string content;//内容
	std::vector<FOURProperty> vec_getRes;//获得的资源
	FOURProperty lostRes;//失去资源
	std::string effectNpc;//影响npc
	std::string result;//结果
}ChoiceData;


typedef enum
{
	POS_NOTHING = -1,
	POS_START = 0,
	POS_MONSTER,
	POS_NPC,
	POS_BOSS,
	POS_TBOSS
}POSTYPE;

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
	CC_SYNTHESIZE(int, m_posnpcrnd, PosNpcRnd);//每个地图块NPC或者BOSS出现的概率
	CC_SYNTHESIZE(bool, m_iscansee, IsCanSee);//视野是否可见

private:
	/******************************************************
	此方法override Sprite setTextureCoords方法
	拼图时，上下会有一条黑线，移动时也有明显的分割线
	该方法中打开 宏CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL即可
	*******************************************************/
	void setTextureCoords(const Rect& rectInPoints, V3F_C4B_T2F_Quad* outQuad);
public:
	std::map<int, int> map_eventrnd;//7个事件概率
	FOURProperty npcs[6];//6个怪物数据
	std::vector<FOURProperty> vec_RewardsRes;//获得的物品
	std::vector<ChoiceData> vec_choiceDatas;//任务选择获得的物品
	int Col;//所在列
	int Row;//转换成左上后所在行，编辑器是左上开始。比如编辑器0，0，应该对应 总行-1 - 0(编辑所在行)
	static std::vector<FOURProperty> vec_randMonsters;
	static std::vector<FOURProperty> vec_randMonstersRes;
	static int randMonstersMinCount;
	static int randMonstersMaxCount;
};

#endif

