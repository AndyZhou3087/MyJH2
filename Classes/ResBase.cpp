#include "ResBase.h"
#include "DataSave.h"
#include "Resource.h"
#include "CommonFuncs.h"

ResBase::ResBase()
{
	m_id = "";
	m_type = 0;
	m_where = MYSTORAGE;
}


ResBase::~ResBase()
{
}

void ResBase::add(DynamicValueInt count)
{
	DynamicValueInt dcount;
	dcount.setValue(getCount().getValue() + count.getValue());
	setCount(dcount);
	if (dcount.getValue() <= 0)
		remove();
}

void ResBase::remove()
{
	delete this;
}