#include "GongFa.h"
#include "GlobalInstance.h"
#include "Quest.h"

GongFa::GongFa()
{
	m_qu.setValue(0);
	m_skillcount = 0;
	m_lastlevel = 0;
}


GongFa::~GongFa()
{
}

DynamicValueInt GongFa::getLv()
{
	DynamicValueInt dvint;
	dvint.setValue(0);

	int size = GlobalInstance::map_GF[getId()].vec_exp.size();
	int curlvExp = 0;
	for (int i = 0; i < size; i++)
	{
		curlvExp += GlobalInstance::map_GF[getId()].vec_exp[i];
		if (m_exp.getValue() < curlvExp)
		{
			dvint.setValue(i);
			checkLevelQuest(i);
			return dvint;
		}
	}

	return dvint;
}

void GongFa::checkLevelQuest(int lv)
{
	if (m_lastlevel <= lv - 1)
	{
		Quest::setDailyTask(STRENG_WG, 1);
		Quest::setAchieveTypeCount(STRENG_WG, 1);
	}
	m_lastlevel = lv;
}

int GongFa::getMaxLv()
{
	return GlobalInstance::map_GF[getId()].vec_atk.size() - 1;
}

//攻击
float GongFa::getAtk()
{
	return GlobalInstance::map_GF[getId()].vec_atk[getLv().getValue()];
}

//防御
float GongFa::getDf()
{
	return GlobalInstance::map_GF[getId()].vec_df[getLv().getValue()];
}

//血量
float GongFa::getHp()
{
	return GlobalInstance::map_GF[getId()].vec_hp[getLv().getValue()];
}

//攻击速度
float GongFa::getAtkSpeed()
{
	return GlobalInstance::map_GF[getId()].vec_speed[getLv().getValue()];
}

//暴击
float GongFa::getCrit()
{
	return GlobalInstance::map_GF[getId()].vec_crit[getLv().getValue()];
}

//闪避
float GongFa::getDodge()
{
	return GlobalInstance::map_GF[getId()].vec_avoid[getLv().getValue()];
}