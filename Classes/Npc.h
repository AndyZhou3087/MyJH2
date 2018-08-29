/********************************************************************
* NPC
*********************************************************************/
#ifndef _NPC_H_
#define _NPC_H_
#include "cocos2d.h"
#include "json.h"
#include "Gongfa.h"
USING_NS_CC;

class Npc
{
public:
	Npc();
	~Npc();
	CC_SYNTHESIZE(std::string, m_name, Name);//名称，唯一不重复
	CC_SYNTHESIZE(int, m_vocation, Vocation);// 职业
	CC_SYNTHESIZE(int, m_potential, Potential);//潜力，品质
	CC_SYNTHESIZE(int, m_lv, Level);//等级
	CC_SYNTHESIZE(float, m_hp, Hp);//血量
	CC_SYNTHESIZE(std::string, m_id, Id);//id
	CC_SYNTHESIZE(bool, m_isdodge, IsDodge);//是否闪避

	//攻击
	virtual float getAtk();

	//防御
	virtual float getDf();

	//血量
	virtual float getMaxHp();

	//攻击速度
	virtual float getAtkSpeed();

	//暴击
	virtual float getCrit();

	//闪避
	virtual float getDodge();

	//返回触发了技能的功法，没有返回NULL
	virtual GongFa* checkSkillWg();

	virtual void clearSkill(GongFa* gf);


public:
	std::vector<int> vec_whosufferskill;//施放给谁
};

#endif

