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
	dvalue.setValue((this->getLv().getValue()+1) * 5);
	return dvalue;
}

DynamicValueInt ResCreator::getMaxCap(int lv)
{
	DynamicValueInt dvalue;
	if (lv == 0)
	{
		dvalue.setValue(500);
		return dvalue;
	}
	else
	{
		DynamicValueInt lastmaxcap = getMaxCap(lv - 1);
		dvalue.setValue(100*(lv+1) + lastmaxcap.getValue());
	}
	return dvalue;
}
