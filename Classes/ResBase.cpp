#include "ResBase.h"
#include "DataSave.h"
#include "Resource.h"
#include "CommonFuncs.h"

ResBase::ResBase()
{
	
}


ResBase::~ResBase()
{
}

void ResBase::use(DynamicValueInt count)
{
	DynamicValueInt dcount;
	dcount.setValue(getCount().getValue() - count.getValue());
	setCount(dcount);
	if (dcount.getValue() <= 0)
		remove();
}

void ResBase::add(DynamicValueInt count)
{
	DynamicValueInt dcount;
	dcount.setValue(getCount().getValue() + count.getValue());
	setCount(dcount);
}

void ResBase::remove()
{
	delete this;
}