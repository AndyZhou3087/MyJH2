#include "MyRes.h"
#include "DataSave.h"
#include "Equipable.h"
#include "Const.h"
#include "GlobalInstance.h"
#include "Quest.h"

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

bool MyRes::hasResByType(int type, int inwhere)
{
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		if (vec_MyResources[i]->getType() == type && vec_MyResources[i]->getWhere() == inwhere)
		{
			return true;
		}
	}
	return false;
}

int MyRes::getMyResCount(std::string resid, int inwhere)
{
	ResBase* ResBase = getMyRes(resid, inwhere);
	if (ResBase != NULL)
		return ResBase->getCount().getValue();

	return 0;
}

int MyRes::getEquipableCount(std::string resid, int inwhere)
{
	int count = 0;
	for (unsigned int i = 0; i < vec_MyResources.size(); i++)
	{
		if (vec_MyResources[i]->getWhere() == inwhere && vec_MyResources[i]->getId().compare(resid) == 0)
		{
			count += vec_MyResources[i]->getCount().getValue();
		}
	}
	return count;
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

ResBase* MyRes::Add(std::string resid, int count, int inwhere, int qu, int stonescount)
{
	int i = 0;
	for (; i < sizeof(RES_TYPES_CHAR)/sizeof(RES_TYPES_CHAR[0]);i++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[i]) == 0)
			break;
	}

	ResBase* retres = NULL;
	if (i >= T_ARMOR && i <= T_FASHION)
	{
		for (int m = 0; m < count; m++)
		{
			Equip* res = new Equip();
			res->setId(resid);
			res->setType(i);
			DynamicValueInt dvalue;
			dvalue.setValue(1);
			res->setCount(dvalue);

			DynamicValueInt quvalue;
			quvalue.setValue(qu);
			res->setQU(quvalue);
			res->setWhere(inwhere);

			if (stonescount > 0)
			{
				for (int n = 0; n < stonescount; n++)
					res->vec_stones.push_back("o");//用一个占位
			}
			vec_MyResources.push_back(res);
			retres = res;
		}
	}
	else if (i >= T_WG && i <= T_NG)
	{
		for (int m = 0; m < count; m++)
		{
			GongFa* res = new GongFa();
			res->setId(resid);
			res->setType(i);
			DynamicValueInt dvalue;
			dvalue.setValue(1);
			res->setCount(dvalue);

			DynamicValueInt quvalue;
			quvalue.setValue(qu);
			res->setQU(quvalue);
			res->setWhere(inwhere);

			vec_MyResources.push_back(res);
			retres = res;
		}
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
		retres = res;
	}
	Quest::setAchieveTypeCount(ACHIEVE_GOODS, count, resid);
	saveData();
	
	return retres;
}

void MyRes::Add(ResBase* res, int count, int inwhere)
{
	if (count == 0)
		count = res->getCount().getValue();

	int type = res->getType();
	if (type >= T_ARMOR && type <= T_FASHION)
	{
		for (int m = 0; m < count; m++)
		{
			Equip* ores = (Equip*)res;
			Equip* eres = new Equip();
			eres->setId(res->getId());
			eres->setType(type);
			DynamicValueInt dvalue;
			dvalue.setValue(1);
			eres->setCount(dvalue);

			DynamicValueInt dlv;
			dlv.setValue(ores->getLv().getValue());
			eres->setLv(dlv);

			DynamicValueInt quvalue;
			quvalue.setValue(res->getQU().getValue());
			eres->setQU(quvalue);
			eres->setWhere(inwhere);
			int ssize = ores->vec_stones.size();
			if (ssize > 0)
			{
				for (int n = 0; n < ssize; n++)
					eres->vec_stones.push_back(ores->vec_stones[n]);
			}
			vec_MyResources.push_back(eres);
		}
	}
	else if (type >= T_WG && type <= T_NG)
	{
		for (int m = 0; m < count; m++)
		{
			GongFa* gres = new GongFa();
			gres->setId(res->getId());
			gres->setType(type);
			DynamicValueInt dvalue;
			dvalue.setValue(1);
			gres->setCount(dvalue);

			DynamicValueInt dexp;
			dexp.setValue(((GongFa*)res)->getExp().getValue());
			gres->setExp(dexp);

			DynamicValueInt quvalue;
			quvalue.setValue(res->getQU().getValue());
			gres->setQU(quvalue);
			gres->setWhere(inwhere);
			vec_MyResources.push_back(gres);
		}
	}
	else
	{
		ResBase* ores = getMyRes(res->getId(), inwhere);
		if (ores == NULL)
		{
			ores = new ResBase();
			ores->setId(res->getId());
			ores->setType(type);
			ores->setWhere(inwhere);
			vec_MyResources.push_back(ores);
		}

		DynamicValueInt dvalue;
		dvalue.setValue(ores->getCount().getValue() + count);
		ores->setCount(dvalue);
	}
	Quest::setAchieveTypeCount(ACHIEVE_GOODS, count, res->getId());
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

	int index = -1;
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
					index = i;
					break;
				}
			}
			else if (restype >= T_WG && restype <= T_NG)
			{
				GongFa* gf = (GongFa*)res;
				GongFa* mygf = (GongFa*)vec_MyResources[i];
				if (gf->getExp().getValue() == mygf->getExp().getValue() && gf->getQU().getValue() == gf->getQU().getValue())
				{
					index = i;
					break;
				}
			}
			else
			{
				index = i;
				break;
			}
		}
	}
	if (index >= 0)
		return Use(i, count);
	else
		return 0;
}

int MyRes::Use(std::string resid, int count, int inwhere)
{
	int i = 0;
	for (; i < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); i++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[i]) == 0)
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
		int type = res->getType();
		if (res->getWhere() == MYPACKAGE)
		{
			int addcount = res->getCount().getValue();
			for (unsigned int n = 0; n < GlobalInstance::vec_resCreators.size(); n++)
			{
				if (res->getId().compare(GlobalInstance::vec_resCreators[n]->getName()) == 0)
				{
					int max = GlobalInstance::vec_resCreators[n]->getMaxCap(GlobalInstance::vec_resCreators[n]->getLv().getValue()).getValue();

					if (MyRes::getMyResCount(res->getId()) >= max)
					{
						addcount = 0;
					}
					else
					{
						int left = max - MyRes::getMyResCount(res->getId());
						if (left < addcount)
							addcount = left;
					}
					break;
				}
			}
			if (addcount != 0)
				Add(res, res->getCount().getValue(), MYSTORAGE);
		}
	}
	clearMyPackages();
}

void MyRes::clearMyPackages()
{
	unsigned int i = 0;
	for (i = 0; i < vec_MyResources.size(); i++)
	{
		ResBase* res = vec_MyResources[i];
		if (res->getWhere() == MYPACKAGE)
		{
			Use(res, res->getCount().getValue(), MYPACKAGE);
			i = 0;
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
    if (str.length() > 0)
        DataSave::getInstance()->setMyRes(str.substr(0, str.length() - 1));
}

void MyRes::removeSivlerAndCoin()
{
	std::vector<ResBase*>::iterator mit;
	for (mit = MyRes::vec_MyResources.begin(); mit != MyRes::vec_MyResources.end();)
	{
		ResBase* res = *mit;
		if (res->getId().compare("r006") == 0 || res->getId().compare("r012") == 0)
		{
			DynamicValueInt dvint;
			dvint.setValue(res->getCount().getValue());

			if (res->getId().compare("r006") == 0)
				GlobalInstance::getInstance()->addMySoliverCount(dvint);
			else
				GlobalInstance::getInstance()->addMyCoinCount(dvint);
			mit = MyRes::vec_MyResources.erase(mit);
		}
		else
		{
			mit++;
		}
	}
}
