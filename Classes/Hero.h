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

#define BASEHEROMAX 4

typedef enum
{
	HS_READY = 0, //待招募
	HS_OWNED,//已招募
	HS_TAKEON,//出城
	HS_ONCHALLENGE,//竞技
	HS_DEAD,//死亡
	HS_TRAINING//训练
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
	CC_SYNTHESIZE(float, m_randattr, RandAttr);//随机属性
	CC_SYNTHESIZE(int, m_pos, Pos);//出城时选择的位置，1开始，0表示不选择
	CC_SYNTHESIZE(int, m_trainhour, TrainHour);//选择训练的时间，0--未训练，3600--一小时，18000--两小时，28800--三小时
	CC_SYNTHESIZE(int, m_traintime, TrainTime);//训练的时间
	CC_SYNTHESIZE(DynamicValueInt, m_power, Power);//精力
	CC_SYNTHESIZE(int, m_powertime, PowerTime);//精力时间
	CC_SYNTHESIZE(int, m_changecount, ChangeCount);//转职，突破
	CC_SYNTHESIZE(bool, m_isSkilling, IsSkilling);//是否触发技能
	CC_SYNTHESIZE(int, m_whosufferskill, WhoSufferskill);//施放给谁

	//血量
	float getHp();

	void setHp(float hp);

	//获取最高等级
	int getMaxLevel();

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

	//升级上限
	void setExpLimit(int vexp);
private:
	//随机生成一个名字
	std::string generateName();
	Equipable* takeOnEquip[6];//6个装备，包括武功和内功
	void setMyLevel(int lv);

private:
	int m_level;//等级
public:
	/********************************************************************
	* 根据类型获取对应的装备
	*********************************************************************/
	Equipable* getEquipable(int etype);
	void setEquipable(Equipable* edata, int etype);
};

#endif

