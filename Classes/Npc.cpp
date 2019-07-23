#include "Npc.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "GlobalInstance.h"

Npc::Npc()
{
	m_isdodge = false;
	m_isRndSkill = false;
}


Npc::~Npc()
{

}

float Npc::getHp()
{
	return m_hp;
}

void Npc::setHp(float hp)
{
	m_hp = hp;
}

int Npc::getLevel()
{
	return m_lv;
}

void Npc::setLevel(int val)
{
	m_lv = val;
}

float Npc::getAtk()
{
	float npcatk = GlobalInstance::map_NpcAttrData[m_vocation].vec_atk[m_lv] * POTENTIAL_BNS[m_potential];
	
	return npcatk;
}

float Npc::getDf()
{
	float npcdf = GlobalInstance::map_NpcAttrData[m_vocation].vec_df[m_lv] * POTENTIAL_BNS[m_potential];

	return npcdf;
}
float Npc::getMaxHp()
{
	float npcmaxhp = GlobalInstance::map_NpcAttrData[m_vocation].vec_maxhp[m_lv] * POTENTIAL_BNS[m_potential];

	return npcmaxhp * GlobalInstance::supperbossinfo.bosshps;
}
float Npc::getAtkSpeed()
{
	float npcatkspeed = GlobalInstance::map_NpcAttrData[m_vocation].vec_speed[m_lv] * POTENTIAL_BNS[m_potential];

	return npcatkspeed;
}
float Npc::getCrit()
{
	float npccrit = GlobalInstance::map_NpcAttrData[m_vocation].vec_crit[m_lv] * POTENTIAL_BNS[m_potential];

	return npccrit;
}
float Npc::getDodge()
{
	float npcdodge = GlobalInstance::map_NpcAttrData[m_vocation].vec_avoid[m_lv] * POTENTIAL_BNS[m_potential];

	return npcdodge;
}

GongFa* Npc::checkSkillWg()
{
	return NULL;
}

void Npc::clearSkill(GongFa* gf)
{
	setIsDodge(false);
	if (gf != NULL)
		gf->setSkillCount(0);
}

void Npc::setFightRound(int count)
{
	m_fightround = count;
}

int Npc::getFightRound()
{
	return m_fightround;
}