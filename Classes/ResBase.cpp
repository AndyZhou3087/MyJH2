#include "ResBase.h"
#include "DataSave.h"
#include "Resource.h"
#include "CommonFuncs.h"

ResBase::ResBase()
{
	m_type = 0;
	m_where = MYSTORAGE;
	m_qu.setValue(-1);
}


ResBase::~ResBase()
{
}