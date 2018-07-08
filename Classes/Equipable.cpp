#include "Equipable.h"

Equipable::Equipable()
{
	m_qu.setValue(-1);
}


Equipable::~Equipable()
{
}

DynamicValueInt Equipable::getLv()
{
	DynamicValueInt dv;
	return dv;
}

//攻击
float Equipable::getAtk()
{
	return 0;
}

//防御
float Equipable::getDf()
{
	return 0;
}
//血量
float Equipable::getHp()
{
	return 0;
}
//攻击速度
float Equipable::getAtkSpeed()
{
	return 0;
}
//暴击
float Equipable::getCrit()
{
	return 0;
}
//闪避
float Equipable::getDodge()
{
	return 0;
}