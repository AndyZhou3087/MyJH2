#include "ResCreator.h"


ResCreator::ResCreator(std::string strid)
{
	m_name = strid;
}


ResCreator::~ResCreator()
{
}

DynamicValueInt ResCreator::getMaxFarmersCount()
{
	DynamicValueInt dvalue;
	dvalue.setValue(this->getLv().getValue() * 5);
	return dvalue;
}

DynamicValueInt ResCreator::getMaxCap()
{
	DynamicValueInt dvalue;
	dvalue.setValue(this->getLv().getValue() * 100 + 500);
	return dvalue;
}
