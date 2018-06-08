#include "MyRes.h"
#include "DataSave.h"

std::vector<ResBase* > MyRes::vec_MyResources;

ResBase* MyRes::getMyResource(std::string resid, int inwhere)
{
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		if (vec_MyResources[i]->getId().compare(resid) == 0 && vec_MyResources[i]->getWhere() == inwhere)
		{
			return vec_MyResources[i];
		}
	}
	return NULL;
}

int MyRes::getMyResCount(std::string resid, int inwhere)
{
	ResBase* res = getMyResource(resid, inwhere);
	if (res == NULL)
		return 0;
	return res->getCount().getValue();
}

int MyRes::getMyPackageResCount()
{
	int count = 0;
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		if (vec_MyResources[i]->getWhere() == MYPACKAGE)
		{
			count += vec_MyResources[i]->getCount().getValue();
		}
	}
	return count;
}

void MyRes::Add(std::string resid, int count, int inwhere)
{
	if (resid.compare(0, 1, "r") == 0)
	{
		ResBase* res = getMyResource(resid, inwhere);
		if (res == NULL)
		{
			res = new ResBase();
			res->setId(resid);
			DynamicValueInt dvalue;
			dvalue.setValue(count);
			res->setCount(dvalue);
			res->setWhere(inwhere);
			vec_MyResources.push_back(res);
		}
		else
		{
			DynamicValueInt dvalue;
			dvalue.setValue(res->getCount().getValue() + count);
			res->setCount(dvalue);
		}
	}
	saveData();
}


void MyRes::saveData()
{
	std::string str;
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		std::string onestr;
		ResBase* res = vec_MyResources[i];
		if (res->getId().compare(0, 1, "r") == 0)
		{
			onestr = StringUtils::format("%s-%d-%d;", res->getId().c_str(), res->getCount().getValue(), res->getWhere());

		}
		else
		{

		}
		str.append(onestr);
	}
	DataSave::getInstance()->setMyRes(str.substr(0, str.length() - 1));
}
