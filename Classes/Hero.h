/********************************************************************
* 英雄
*********************************************************************/
#ifndef _HERO_H_
#define _HERO_H_
#include "cocos2d.h"
#include "json.h"
#include "DynamicValue.h"
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
class Hero
{
public:
	Hero();
	~Hero();
	Hero(Hero* hero);
	CC_SYNTHESIZE(std::string, m_name, Name);//英雄名称，唯一不重复
	CC_SYNTHESIZE(DynamicValueInt, m_exp, Exp); //经验值
	CC_SYNTHESIZE(int, m_vocation, Vocation);// 职业
	CC_SYNTHESIZE(int, m_potential, Potential);//潜力，品质
	CC_SYNTHESIZE(int, m_sex, Sex);//性别
	CC_SYNTHESIZE(int, m_state, State);//英雄状态，0--待招募；1--招募；2--出城；3--竞技
	CC_SYNTHESIZE(int, m_breakupper, BreakUpper);//突破等级
	CC_SYNTHESIZE(float, m_randattr, RandAttr);//随机属性
	CC_SYNTHESIZE(int, m_pos, Pos);//出城时选择的位置，1开始，0表示不选择
	CC_SYNTHESIZE(float, m_hp, Hp);//血量
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
public:
	/********************************************************************
	* 
	*********************************************************************/
};

#endif

