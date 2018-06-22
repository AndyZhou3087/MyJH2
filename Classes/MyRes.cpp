#include "MyRes.h"
#include "DataSave.h"

std::vector<ResBase* > MyRes::vec_MyResources;


ResBase* MyRes::getMyRes(std::string resid, int inwhere)
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
	ResBase* ResBase = getMyRes(resid, inwhere);
	if (ResBase != NULL)
		return ResBase->getCount().getValue();

	return 0;
}

int MyRes::getMyPackageCount()
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

void MyRes::Add(std::string resid, int count, int inwhere, int qu)
{
	std::string types[] = {"r","a","e","h","f","w","x","s","c","d","m","b","y"};
	int i = 0;
	for (; i < sizeof(types)/sizeof(types[0]);i++)
	{
		if (resid.compare(0, 1, types[i]) == 0)
			break;
	}

	if (i >= T_ARMOR && i <= T_FASHION)
	{
		Equip* res = new Equip();
		res->setId(resid);
		res->setType(i);
		DynamicValueInt dvalue;
		dvalue.setValue(count);
		DynamicValueInt quvalue;
		res->setQU(quvalue);
		res->add(dvalue);
		vec_MyResources.push_back(res);
	}
	else if (i >= T_WG && i <= T_NG)
	{
		GongFa* res = new GongFa();
		res->setId(resid);
		res->setType(i);
		DynamicValueInt dvalue;
		dvalue.setValue(count);
		DynamicValueInt quvalue;
		res->setQU(quvalue);
		res->add(dvalue);
		vec_MyResources.push_back(res);
	}
	else
	{
		DynamicValueInt dvalue;
		dvalue.setValue(count);
		ResBase* res = getMyRes(resid, inwhere);
		if (res == NULL)
		{
			res = new ResBase();
			res->setId(resid);
			res->setType(i);
			res->setWhere(inwhere);
			vec_MyResources.push_back(res);
		}
		res->add(dvalue);
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
		if (res->getType() >= T_ARMOR && res->getType() <= T_FASHION)
		{
			Equip* eres = (Equip*)res;
			std::string stonestr;
			int estonesize = eres->vec_stones.size();
			for (int i = 0; i < estonesize; i++)
			{
				if (i < estonesize - 1)
				{
					stonestr.append(eres->vec_stones[i] +",");
				}
				else
					stonestr.append(eres->vec_stones[i]);
			}
			onestr = StringUtils::format("%s-%d-%d-%d-%d-%s;", res->getId().c_str(), res->getCount().getValue(), res->getWhere(), eres->getQU().getValue(), eres->getLv().getValue(), stonestr.c_str());
		}
		else if (res->getType() >= T_WG && res->getType() <= T_NG)
		{
			GongFa* gres = (GongFa*)res;

			onestr = StringUtils::format("%s-%d-%d-%d-%d;", res->getId().c_str(), res->getCount().getValue(), res->getWhere(), gres->getQU().getValue(), gres->getLv().getValue());
		}
		else
		{
			onestr = StringUtils::format("%s-%d-%d;", res->getId().c_str(), res->getCount().getValue(), res->getWhere());
		}
		str.append(onestr);
	}
	DataSave::getInstance()->setMyRes(str.substr(0, str.length() - 1));
}
