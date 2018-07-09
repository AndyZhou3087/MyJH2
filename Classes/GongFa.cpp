#include "GongFa.h"
#include "GlobalInstance.h"

GongFa::GongFa()
{
	m_qu.setValue(0);
}


GongFa::~GongFa()
{
}

DynamicValueInt GongFa::getLv()
{
	DynamicValueInt dvint;
	dvint.setValue(0);
	return dvint;
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