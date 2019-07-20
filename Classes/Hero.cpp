#include "Hero.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "ResBase.h"
#include "Equip.h"
#include "GongFa.h"
#include "MyRes.h"
#include "Quest.h"
#include "MainScene.h"
#include "NewGuideLayer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

Hero::Hero()
{
	m_sex = 0;
	m_state = 0;
	m_randattr = 0.0f;
	m_pos = 0;
	m_onchallengepos = 0;
	m_hp = INT32_MIN;
	m_lastmaxhp = -1;
	m_trainhour = 0;
	m_traintime = 0;
	m_power.setValue(100);
	m_powertime = 0;
	m_changecount = 0;
	m_lastlevel = 0;
	for (int i = 0; i < 6; i++)
	{
		takeOnEquip[i] = NULL;
	}
	m_skillingtype = -1;
	m_isdodge = false;
	m_fightround = 0;
	m_ftype = 0;
	m_isRndSkill = false;
}


Hero::~Hero()
{

}

Hero::Hero(Hero* hero)
{
	m_name = hero->getName();
	m_sex = hero->getSex();
	m_vocation = hero->getVocation();
	m_potential = hero->getPotential();
	m_randattr = hero->getRandAttr();
	m_state = hero->getState();
	m_pos = 0;
	m_onchallengepos = 0;
	m_hp = hero->getHp();
	m_lastmaxhp = hero->getMaxHp();
	m_trainhour = hero->getTrainHour();
	m_traintime = hero->getTrainTime();
	m_power = hero->getPower();
	m_powertime = hero->getPowerTime();
	m_changecount = hero->getChangeCount();
	m_lastlevel = hero->getLevel();
	m_id = hero->getId();
	for (int i = 0; i < 6; i++)
	{
		takeOnEquip[i] = NULL;
	}
	m_skillingtype = -1;
	m_isdodge = false;
	m_fightround = 0;
	m_ftype = 0;
	m_isRndSkill = false;
}

float Hero::getHp()
{
	if (m_hp < -100)
	{
		m_hp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[0] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];
	}
	else if (m_hp < 0)
	{
		m_hp = 0;
	}

	return m_hp;
}

void Hero::setHp(float hp)
{
	m_hp = hp;
}

int Hero::getMaxLevel()
{
	int size = BASEMAXHEROLV;//GlobalInstance::vec_herosAttr[m_vocation].vec_exp.size();
	
	for (int i = 0; i < sizeof(ADDMAXHEROLV)/sizeof(ADDMAXHEROLV[0]); i++)
	{
		if (i < GlobalInstance::getInstance()->getUnlockChapter())
			size += ADDMAXHEROLV[i];
	}
	
	return size;
}

int Hero::getLevel()
{
	int size = getMaxLevel();//GlobalInstance::vec_herosAttr[m_vocation].vec_exp.size();
	for (int i = 0; i < size; i++)
	{
		if (m_exp.getValue() < GlobalInstance::vec_herosAttr[m_vocation].vec_exp[i])
		{
			return i;
		}
	}
	if (m_exp.getValue() >= GlobalInstance::vec_herosAttr[m_vocation].vec_exp[size - 1])
	{
		return size - 1;
	}
	return 0;
}

void Hero::checkLevelQuest(int lv)
{
	if (m_lastlevel <= lv - 1)
	{
		Quest::setDailyTask(UPGRADE_HERO, lv - m_lastlevel);
		Quest::setAchieveTypeCount(UPGRADE_HERO, lv - m_lastlevel);
		float herohp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[lv] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];
		m_hp = herohp;
	}

	Quest::setAchieveTypeCount(HERO_LEVEL, lv + 1);

	m_lastlevel = lv;
}

void Hero::setExpLimit(int vexp)
{
	bool isChangeLevel = false;
	m_lastlevel = getLevel();

	int size = getMaxLevel();//GlobalInstance::vec_herosAttr[m_vocation].vec_exp.size();
	int allexp = m_exp.getValue() + vexp;
	for (int i = 0; i < size; i++)
	{
		if (allexp < GlobalInstance::vec_herosAttr[m_vocation].vec_exp[i])
		{
			if ((i + 1) / 10 >= m_changecount + 1)
			{
				isChangeLevel = true;
			}
			break;
		}
	}
	if (isChangeLevel)
	{
		DynamicValueInt dal;
		dal.setValue(GlobalInstance::vec_herosAttr[m_vocation].vec_exp[(m_changecount+1)*10-2]);
		setExp(dal);
	}
	else
	{
		DynamicValueInt dal;
		dal.setValue(allexp);
		setExp(dal);
	}
	for (int i = 0; i < size; i++)
	{
		if (m_exp.getValue() < GlobalInstance::vec_herosAttr[m_vocation].vec_exp[i])
		{
			checkLevelQuest(i);
			break;
		}
	}

	if (m_exp.getValue() >= GlobalInstance::vec_herosAttr[m_vocation].vec_exp[size - 1])
	{
		DynamicValueInt dal;
		dal.setValue(GlobalInstance::vec_herosAttr[m_vocation].vec_exp[size - 1]);
		setExp(dal);
		checkLevelQuest(size - 1);
	}
}

float Hero::getAtk()
{
	float heroatk = GlobalInstance::vec_herosAttr[m_vocation].vec_atk[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];
	
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			heroatk += (takeOnEquip[i]->getAtk()*herobns);
		}
	}

	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[1];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[1];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[1];
			}
		}
	}
	float renationbns = heroatk * addpercent;

	float formationbns = heroatk * GlobalInstance::getInstance()->getFormationBns(1);

	return heroatk + renationbns + formationbns;
}
float Hero::getDf()
{
	float herodf = GlobalInstance::vec_herosAttr[m_vocation].vec_df[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];
	
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				float herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
				Equip* equip = (Equip*)takeOnEquip[i];
				herodf += (equip->getDf()*herobns);
				if (GlobalInstance::map_EquipSuit.find(equip->getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[equip->getId()].vec_suit.size() >= 3)
				{
					std::string suitid = GlobalInstance::map_EquipSuit[equip->getId()].vec_suit[2];
					int t = 0;
					for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
					{
						if (suitid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
							break;
					}
					if (t >= T_ARMOR && t <= T_FASHION)
					{
						if (getEquipable(t) != NULL)
							herodf += equip->getSuitDf();
					}
				}
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				float herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
				herodf += takeOnEquip[i]->getDf()*herobns;
			}
		}
	}

	float addpercent = 0.0f;

	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[2];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[2];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[2];
			}
		}
	}

	float renationbns = herodf * addpercent;

	float formationbns = herodf * GlobalInstance::getInstance()->getFormationBns(2);

	return herodf + renationbns + formationbns;
}
float Hero::getMaxHp()
{
	float herohp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				float herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
				Equip* equip = (Equip*)takeOnEquip[i];
				herohp += (equip->getHp()*herobns);
				if (GlobalInstance::map_EquipSuit.find(equip->getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[equip->getId()].vec_suit.size() >= 2)
				{
					std::string suitid = GlobalInstance::map_EquipSuit[equip->getId()].vec_suit[1];
					int t = 0;
					for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
					{
						if (suitid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
							break;
					}
					if (t >= T_ARMOR && t <= T_FASHION)
					{
						if (getEquipable(t) != NULL)
							herohp += equip->getSuitHp();
					}
				}
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				float herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
				herohp += takeOnEquip[i]->getHp() * herobns;
			}
		}
	}

	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;

	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[0];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[0];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[0];
			}
		}
	}

	float renationbns = herohp * addpercent;

	float formationbns = herohp * GlobalInstance::getInstance()->getFormationBns(0);

	herohp = herohp + renationbns + formationbns;

	if (herohp != m_lastmaxhp)
	{
		m_lastmaxhp = herohp;
		m_hp = herohp;
	}
	if (m_hp > herohp)
		m_hp = herohp;

	return herohp;
}
float Hero::getAtkSpeed()
{
	float heroatkspeed = GlobalInstance::vec_herosAttr[m_vocation].vec_atkspeed[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			heroatkspeed += (takeOnEquip[i]->getAtkSpeed()*herobns);
		}
	}

	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;

	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += heroatkspeed * GlobalInstance::map_npcrelation[nid].friendratio[5];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += heroatkspeed * GlobalInstance::map_npcrelation[nid].masterratio[5];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += heroatkspeed * GlobalInstance::map_npcrelation[nid].conpelratio[5];
			}
		}
	}

	float renationbns = heroatkspeed * addpercent;

	float formationbns = heroatkspeed * GlobalInstance::getInstance()->getFormationBns(5);

	heroatkspeed = heroatkspeed + formationbns + formationbns;
	return heroatkspeed > 3.0f ? 3.0f : heroatkspeed;
}
float Hero::getCrit()
{
	float herocrit = GlobalInstance::vec_herosAttr[m_vocation].vec_crit[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			herocrit += (takeOnEquip[i]->getCrit()*herobns);
		}
	}
	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[4];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[4];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[4];
			}
		}
	}

	float renationbns = herocrit * addpercent;

	float formationbns = herocrit * GlobalInstance::getInstance()->getFormationBns(4);

	return herocrit + renationbns + formationbns;
}
float Hero::getDodge()
{
	float herododge = GlobalInstance::vec_herosAttr[m_vocation].vec_avoid[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[m_changecount];
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			herododge += (takeOnEquip[i]->getDodge()*herobns);
		}
	}

	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[3];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[3];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[3];
			}
		}
	}
	float renationbns = herododge * addpercent;

	float formationbns = herododge * GlobalInstance::getInstance()->getFormationBns(3);

	herododge = herododge + renationbns + formationbns;
	return herododge>48?48:herododge;
}

float Hero::getAtkBy(int breakcount)
{
	float heroatk = GlobalInstance::vec_herosAttr[m_vocation].vec_atk[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[breakcount];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			heroatk += (takeOnEquip[i]->getAtk()*herobns);
		}
	}

	float addpercent = 0.0f;

	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[1];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[1];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[1];
			}
		}
	}

	float renationbns = heroatk * addpercent;

	return heroatk + renationbns;
}
float Hero::getDfBy(int breakcount)
{
	float herodf = GlobalInstance::vec_herosAttr[m_vocation].vec_df[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[breakcount];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				float herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
				Equip* equip = (Equip*)takeOnEquip[i];
				herodf += (equip->getDf()*herobns);
				if (GlobalInstance::map_EquipSuit.find(equip->getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[equip->getId()].vec_suit.size() >= 3)
				{
					std::string suitid = GlobalInstance::map_EquipSuit[equip->getId()].vec_suit[2];
					int t = 0;
					for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
					{
						if (suitid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
							break;
					}
					if (t >= T_ARMOR && t <= T_FASHION)
					{
						if (getEquipable(t) != NULL)
							herodf += equip->getSuitDf();
					}
				}
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				float herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
				herodf += takeOnEquip[i]->getDf()*herobns;
			}
		}
	}
	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[2];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[2];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[2];
			}
		}
	}
	float renationbns = herodf * addpercent;
	return herodf + renationbns;
}
float Hero::getMaxHpBy(int breakcount)
{
	float herohp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[breakcount];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				float herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
				Equip* equip = (Equip*)takeOnEquip[i];
				herohp += (equip->getHp()*herobns);
				if (GlobalInstance::map_EquipSuit.find(equip->getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[equip->getId()].vec_suit.size() >= 2)
				{
					std::string suitid = GlobalInstance::map_EquipSuit[equip->getId()].vec_suit[1];
					int t = 0;
					for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
					{
						if (suitid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
							break;
					}
					if (t >= T_ARMOR && t <= T_FASHION)
					{
						if (getEquipable(t) != NULL)
							herohp += equip->getSuitHp();
					}
				}
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				float herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
				herohp += takeOnEquip[i]->getHp() * herobns;
			}
		}
	}
	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;

	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[0];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[0];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[0];
			}
		}
	}
	float renationbns = herohp * addpercent;
	return herohp + renationbns;
}
float Hero::getAtkSpeedBy(int breakcount)
{
	float heroatkspeed = GlobalInstance::vec_herosAttr[m_vocation].vec_atkspeed[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[breakcount];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			heroatkspeed += (takeOnEquip[i]->getAtkSpeed()*herobns);
		}
	}

	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;

	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[5];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[5];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[5];
			}
		}
	}

	float renationbns = heroatkspeed * addpercent;
	heroatkspeed += renationbns;
	return heroatkspeed > 3.0f ? 3.0f : heroatkspeed;
}
float Hero::getCritBy(int breakcount)
{
	float herocrit = GlobalInstance::vec_herosAttr[m_vocation].vec_crit[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[breakcount];
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			herocrit += (takeOnEquip[i]->getCrit()*herobns);
		}
	}
	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[4];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[4];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[4];
			}
		}
	}
	float renationbns = herocrit * addpercent;
	return herocrit + renationbns;
}

float Hero::getDodgeBy(int breakcount)
{
	float herododge = GlobalInstance::vec_herosAttr[m_vocation].vec_avoid[getLevel()] * POTENTIAL_BNS[m_potential] * BREAK_BNS[breakcount];
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			herododge += (takeOnEquip[i]->getDodge()*herobns);
		}
	}
	float addpercent = 0.0f;
	std::map<std::string, NpcFriendly> map_friendly;
	std::map<std::string, NpcFriendly>::iterator it;
	if (m_ftype == 0)
	{
		map_friendly = GlobalInstance::map_myfriendly;
	}
	else if (m_ftype == 1)
	{
		map_friendly = GlobalInstance::myMatchInfo.map_pairfriendly;
	}

	for (it = map_friendly.begin(); it != map_friendly.end(); ++it)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].friendratio[3];
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].masterratio[3];
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				addpercent += GlobalInstance::map_npcrelation[nid].conpelratio[3];
			}
		}
	}
	float renationbns = herododge * addpercent;
	return herododge + renationbns;
}

void Hero::generate()
{
	setVocation(GlobalInstance::getInstance()->createRandomNum(BASEHEROMAX));

	setPotential(GlobalInstance::getInstance()->generateHeroPotential());

	DynamicValueInt lvdint;
	setExp(lvdint);
	//setSex(GlobalInstance::getInstance()->createRandomNum(2));
	std::string nickname;
	bool iscontinue = true;
	do
	{
		nickname = generateName();
	} while (GlobalInstance::getInstance()->checkifSameName(nickname));
	setName(nickname);

	//m_hp = getMaxHp();
}

std::string Hero::generateName()
{
//	int rfirst = GlobalInstance::getInstance()->createRandomNum(FIRSTNAMECOUNT);
//	int lastnamecount = LASTNAMECOUNT0;
//	std::string lastnamefile = "heroname/lastname0.txt";
//	if (m_vocation == 3)//女性名字
//	{
//		lastnamecount = LASTNAMECOUNT1;
//		lastnamefile = "heroname/lastname1.txt";
//	}
//
//	int rlast = GlobalInstance::getInstance()->GlobalInstance::createRandomNum(lastnamecount);
//	std::string namestr;
//	std::string heronamefile[] = { "heroname/firstname.txt" , lastnamefile };
//	int randindex[] = { rfirst ,rlast };
//	for (int i = 0; i < 2; i++)
//	{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)//android apk是压缩包无法通过fopen操作Asset文件，通过cocos的文件操作，需要读取整个文件，文件可能有1w行，所以改用java文件处理
//		std::string ret;
//		JniMethodInfo methodInfo;
//		std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
//		if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "readAssetStringByLine", "(Ljava/lang/String;I)Ljava/lang/String;"))
//		{
//			jstring para1 = methodInfo.env->NewStringUTF(heronamefile[i].c_str());
//			jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, para1, randindex[i]);
//			ret = methodInfo.env->GetStringUTFChars(jstr, 0);
//		}
//		CommonFuncs::trim(ret);
//		namestr += ret;
//#else
//		std::string fileName = FileUtils::getInstance()->fullPathForFilename(ResourcePath::makePath(heronamefile[i]));
//		if (fileName.length() > 0)
//		{
//			FILE *fp = fopen(fileName.c_str(), "rb");
//			if (fp)
//			{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//				fseek(fp, randindex[i] * 14, 0);//12个字节名字（4个中文），WIN32回车换行2个字节
//#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//				fseek(fp, randindex[i] * 13, 0);//12个字节名字（4个中文），IOS回车换行1个字节
//#endif
//				char szReadBuff[13] = { 0 };
//				size_t readsize = fread(szReadBuff, 12, 1, fp);
//				if (readsize > 0)
//				{
//					std::string name = StringUtils::format("%s", szReadBuff);
//					CommonFuncs::trim(name);
//					namestr += name;
//				}
//				fclose(fp);
//				fp = NULL;
//			}
//
//		}
//#endif
//	}

	int namerndindex = 0;
	std::string namefile;

	if (m_vocation == 3)//女性名字
	{
		namerndindex = GlobalInstance::getInstance()->createRandomNum(FEMALENAMECOUNT);
		namefile = "heroname/name1.txt";
	}
	else
	{
		namerndindex = GlobalInstance::getInstance()->createRandomNum(MANNAMECOUNT);
		namefile = "heroname/name0.txt";
	}

	std::string namestr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)//android apk是压缩包无法通过fopen操作Asset文件，通过cocos的文件操作，需要读取整个文件，文件可能有1w行，所以改用java文件处理
		std::string ret;
		JniMethodInfo methodInfo;
		std::string clsname = StringUtils::format("%s/Utils", ANDOIRJNICLS);
		if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "readAssetStringByLine", "(Ljava/lang/String;I)Ljava/lang/String;"))
		{
			jstring para1 = methodInfo.env->NewStringUTF(namefile.c_str());
			jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, para1, namerndindex);
			ret = methodInfo.env->GetStringUTFChars(jstr, 0);
		}
		CommonFuncs::trim(ret);
		namestr += ret;
#else
		std::string fileName = FileUtils::getInstance()->fullPathForFilename(ResourcePath::makePath(namefile));
		if (fileName.length() > 0)
		{
			FILE *fp = fopen(fileName.c_str(), "rb");
			if (fp)
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				fseek(fp, namerndindex * 17, 0);//15个字节名字（5个中文），WIN32回车换行2个字节
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				fseek(fp, namerndindex * 16, 0);//15个字节名字（5个中文），IOS回车换行1个字节
#endif
				char szReadBuff[16] = { 0 };
				size_t readsize = fread(szReadBuff, 15, 1, fp);
				if (readsize > 0)
				{
					std::string name = StringUtils::format("%s", szReadBuff);
					CommonFuncs::trim(name);
					namestr.append(name);
				}
				fclose(fp);
				fp = NULL;
			}

		}
#endif
	return namestr;
}

Equipable* Hero::getEquipable(int etype)
{
	return takeOnEquip[etype - 1];
}
void Hero::setEquipable(Equipable* edata, int etype)
{
	//因为类型是按顺序排的，第一个被基础资源占了，所以类型需要-1
	takeOnEquip[etype - 1] = edata;
}

GongFa* Hero::checkSkillWg()
{

	int t[] = { T_WG ,T_NG };
	if (m_isRndSkill)
	{
		for (int i = 0; i < 2; i++)
		{
			GongFa* gf = (GongFa*)getEquipable(t[i]);

			if (gf != NULL && gf->getSkillCount() > 0)
			{
				return gf;
			}
		}
		return NULL;
	}

	m_isRndSkill = true;


	/*******测试技能start*********/
	//if (m_ftype == 1)
	//{
	//	std::string gfid = "w033";
	//	GongFa* res = new GongFa();
	//	res->setId(gfid);
	//	res->setType(T_WG);
	//	DynamicValueInt dvalue;
	//	dvalue.setValue(1);
	//	res->setCount(dvalue);

	//	DynamicValueInt quvalue;
	//	quvalue.setValue(GlobalInstance::map_GF[gfid].qu);
	//	res->setQU(quvalue);
	//	res->setWhere(MYEQUIP);
	//	setEquipable(res, T_WG);
	//}
	/*******测试技能end*********/
	for (int i = 0; i < 2; i++)
	{
		GongFa* gf = (GongFa*)getEquipable(t[i]);

		if (gf != NULL && gf->getSkillCount() > 0)
		{
			return gf;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		GongFa* gf = (GongFa*)getEquipable(t[i]);

		/*******测试技能start*********/
		//if (gf != NULL /*&& GlobalInstance::map_GF[gf->getId()].vec_skillbns[getVocation()] == 1*/)
		/*******测试技能end*********/
		if (gf != NULL && GlobalInstance::map_GF[gf->getId()].vec_skillbns[getVocation()] == 1)
		{
			//if (i == 0)
			//{
			//	/*******测试技能start*********/
			//	//if (m_fightround >= 0)
			//	/*******测试技能end*********/
			//	if (m_fightround >= GlobalInstance::map_GF[gf->getId()].skillrnd)
			//	{
			//		m_fightround = 0;
			//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
			//		return gf;
			//	}
			//}
			//else
			//{
				int r = GlobalInstance::getInstance()->createRandomNum(100);
				/*******测试技能start*********/
				//if (r < 100)
				/*******测试技能end*********/
				if (r < GlobalInstance::map_GF[gf->getId()].skillrnd)
				{
					gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
					return gf;
				}
			//}
		}
	}
	return NULL;
}

void Hero::clearSkill(GongFa* gf)
{
	gf->setSkillCount(0);
	setIsDodge(false);
	vec_whosufferskill.clear();
	setSkillingType(-1);
}
