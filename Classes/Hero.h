/********************************************************************
* 英雄
*********************************************************************/
#ifndef _HERO_H_
#define _HERO_H_
#include "cocos2d.h"
#include "json.h"
#include "DynamicValue.h"
#include "Equipable.h"
#include "Npc.h"
USING_NS_CC;

#define HEROMAX 4

typedef enum
{
	HS_READY = 0, //待招募
	HS_OWNED,//已招募
	HS_TAKEON,//出城
	HS_ONCHALLENGE,//竞技
	HS_DEAD//死亡
}HEROSTATE;
class Hero :public Npc
{
public:
	Hero();
	~Hero();
	Hero(Hero* hero);

	CC_SYNTHESIZE(DynamicValueInt, m_exp, Exp); //经验值
	CC_SYNTHESIZE(int, m_sex, Sex);//性别
	CC_SYNTHESIZE(int, m_state, State);//英雄状态，0--待招募；1--招募；2--出城；3--竞技
	CC_SYNTHESIZE(int, m_breakupper, BreakUpper);//突破等级
	CC_SYNTHESIZE(float, m_randattr, RandAttr);//随机属性
	CC_SYNTHESIZE(int, m_pos, Pos);//出城时选择的位置，1开始，0表示不选择

	//血量
	float getHp();

	void setHp(float hp);

	//等级，根据经验值算等级
	int getLevel();

	//攻击
	float getAtk();

	//防御
	float getDf();

	//血量
	float getMaxHp();

	//攻击速度
	float getAtkSpeed();

	//暴击
	float getCrit();

	//闪避
	float getDodge();

	//随机生成一个英雄
	void generate();
private:
	//随机生成一个名字
	std::string generateName();
	Equipable* takeOnEquip[6];//6个装备，包括武功和内功
public:
	/********************************************************************
	* 根据类型获取对应的装备
	*********************************************************************/
	Equipable* getEquipable(int etype);
	void setEquipable(Equipable* edata, int etype);
};

#endif

