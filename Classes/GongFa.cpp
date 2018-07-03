#include "GongFa.h"

GongFa::GongFa()
{
	m_id = "";
	m_type = 0;
	m_where = MYSTORAGE;
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