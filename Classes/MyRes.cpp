#include "MyRes.h"
#include "DataSave.h"
#include "Equipable.h"

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

ResBase* MyRes::getMyPutOnResById(std::string sid, std::string whos)
{
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		ResBase* res = vec_MyResources[i];
		if (res->getId().compare(sid) == 0 && res->getWhere() == MYEQUIP)
		{
			if (res->getType() >= T_ARMOR && res->getType() <= T_NG)
			{
				Equipable* e = (Equipable*)res;
				if (e->getWhos().compare(whos) == 0)
					return e;
			}
		}
	}
	return NULL;
}

ResBase* MyRes::getMyPutOnResByType(int type, std::string whos)
{
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		ResBase* res = vec_MyResources[i];
		if (res->getType() == type && res->getWhere() == MYEQUIP)
		{
			if (res->getType() >= T_ARMOR && res->getType() <= T_NG)
			{
				Equipable* e = (Equipable*)res;
				if (e->getWhos().compare(whos) == 0)
					return e;
			}
		}
	}
	return NULL;
}

void MyRes::Add(std::string resid, int count, int inwhere, int qu, int stonescount)
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
		res->setCount(dvalue);

		DynamicValueInt quvalue;
		quvalue.setValue(qu);
		res->setQU(quvalue);

		if (stonescount > 0)
		{
			for (int n = 0; n < stonescount; n++)
			res->vec_stones.push_back("o");//用一个占位
		}
		vec_MyResources.push_back(res);
	}
	else if (i >= T_WG && i <= T_NG)
	{
		GongFa* res = new GongFa();
		res->setId(resid);
		res->setType(i);
		DynamicValueInt dvalue;
		dvalue.setValue(count);
		res->setCount(dvalue);

		DynamicValueInt quvalue;
		quvalue.setValue(qu);
		res->setQU(quvalue);

		vec_MyResources.push_back(res);
	}
	else
	{
		ResBase* res = getMyRes(resid, inwhere);
		if (res == NULL)
		{
			res = new ResBase();
			res->setId(resid);
			res->setType(i);
			res->setWhere(inwhere);
			vec_MyResources.push_back(res);
		}

		DynamicValueInt dvalue;
		dvalue.setValue(res->getCount().getValue() + count);
		res->setCount(dvalue);
	}
	saveData();
}

void MyRes::Add(ResBase* res, int count, int inwhere)
{
	if (count == 0)
		count = res->getCount().getValue();

	int type = res->getType();
	if (type >= T_ARMOR && type <= T_FASHION || (type >= T_WG && type <= T_NG))
	{
		DynamicValueInt dv;
		dv.setValue(count);
		res->setCount(dv);

		res->setWhere(inwhere);
		vec_MyResources.push_back(res);
	}
	else
	{
		Add(res->getId(), count, inwhere);
	}
	saveData();
}

int MyRes::Use(int iterindex, int count)
{
	ResBase* res = vec_MyResources[iterindex];
	if (res->getCount().getValue() < count)
	{
		return 0;
	}
	else if (res->getCount().getValue() == count)
	{
		delete res;
		vec_MyResources.erase(vec_MyResources.begin() + iterindex);
	}
	else
	{
		DynamicValueInt dcount;
		dcount.setValue(res->getCount().getValue() - count);
		res->setCount(dcount);
	}
	saveData();
	return 1;
}

int MyRes::Use(ResBase* res, int count, int where)
{
	int restype = res->getType();

	unsigned int i = 0;
	for (i = 0; i < vec_MyResources.size(); i++)
	{
		if (vec_MyResources[i]->getType() == restype && vec_MyResources[i]->getId().compare(res->getId()) == 0 && vec_MyResources[i]->getWhere() == where)
		{
			if (restype >= T_ARMOR && restype <= T_FASHION)
			{
				Equip* equip = (Equip*)(res);
				Equip* myequip = (Equip*)(vec_MyResources[i]);

				if (equip->getLv().getValue() == myequip->getLv().getValue() && equip->getQU().getValue() == myequip->getQU().getValue() && equip->vec_stones == myequip->vec_stones)
				{
					break;
				}
			}
			else if (restype >= T_WG && restype <= T_NG)
			{
				GongFa* gf = (GongFa*)res;
				GongFa* mygf = (GongFa*)vec_MyResources[i];
				if (gf->getExp().getValue() == mygf->getExp().getValue() && gf->getQU().getValue() == gf->getQU().getValue())
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	return Use(i, count);
}

int MyRes::Use(std::string resid, int count, int inwhere)
{
	std::string types[] = { "r","a","e","h","f","w","x","s","c","d","m","b","y" };
	int i = 0;
	for (; i < sizeof(types) / sizeof(types[0]); i++)
	{
		if (resid.compare(0, 1, types[i]) == 0)
			break;
	}

	CC_ASSERT(!(i >= T_ARMOR && i <= T_FASHION) || (i >= T_WG && i <= T_NG));

	ResBase* res = getMyRes(resid, inwhere);
	if (res == NULL)
		return 0;

	return Use(res, count, inwhere);
}

void MyRes::putMyPackagesToStorage()
{
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		ResBase* res = vec_MyResources[i];
		if (res->getWhere() == MYPACKAGE)
		{
			Add(res, res->getCount().getValue(), MYSTORAGE);
		}
	}

	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		ResBase* res = vec_MyResources[i];
		if (res->getWhere() == MYPACKAGE)
		{
			Use(res, res->getCount().getValue(), MYPACKAGE);
		}
	}

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
			onestr = StringUtils::format("%s-%d-%d-%d-%d-%s-%s;", res->getId().c_str(), res->getCount().getValue(), res->getWhere(), eres->getQU().getValue(), eres->getLv().getValue(), stonestr.c_str(), eres->getWhos().c_str());
		}
		else if (res->getType() >= T_WG && res->getType() <= T_NG)
		{
			GongFa* gres = (GongFa*)res;

			onestr = StringUtils::format("%s-%d-%d-%d-%d-%s;", res->getId().c_str(), res->getCount().getValue(), res->getWhere(), gres->getQU().getValue(), gres->getExp().getValue(), gres->getWhos().c_str());
		}
		else
		{
			onestr = StringUtils::format("%s-%d-%d;", res->getId().c_str(), res->getCount().getValue(), res->getWhere());
		}
		str.append(onestr);
	}
	DataSave::getInstance()->setMyRes(str.substr(0, str.length() - 1));
}
