﻿#include "Hero.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "ResBase.h"
#include "GlobalInstance.h"
#include "Equip.h"
#include "GongFa.h"
#include "MyRes.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

Hero::Hero()
{
	m_sex = 0;
	m_state = 0;
	m_breakupper = 0;
	m_randattr = 0.0f;
	m_pos = 0;
	m_hp = INT32_MIN;
	m_trainhour = 0;
	m_traintime = 0;
	for (int i = 0; i < 6; i++)
	{
		takeOnEquip[i] = NULL;
	}
}


Hero::~Hero()
{

}

Hero::Hero(Hero* hero)
{
	m_name = hero->getName();
	m_sex = hero->getSex();
	m_vocation = hero->getVocation();
	m_potential = hero->getPotential();
	m_randattr = hero->getRandAttr();
	m_state = hero->getState();
	m_breakupper = 0;
	m_pos = 0;
	m_hp = hero->getHp();
	m_trainhour = hero->getTrainHour();
	m_traintime = hero->getTrainTime();
	for (int i = 0; i < 6; i++)
	{
		takeOnEquip[i] = NULL;
	}
}

float Hero::getHp()
{
	if (m_hp < -100)
	{
		m_hp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[0] * POTENTIAL_BNS[m_potential];
	}
	else if (m_hp < 0)
	{
		m_hp = 0;
	}

	return m_hp;
}

void Hero::setHp(float hp)
{
	m_hp = hp;
}

int Hero::getLevel()
{
	int size = GlobalInstance::vec_herosAttr[m_vocation].vec_exp.size();
	for (int i = 0; i < size; i++)
	{
		if (m_exp.getValue() < GlobalInstance::vec_herosAttr[m_vocation].vec_exp[i])
			return i;
	}
	return 0;
}

float Hero::getAtk()
{
	float heroatk = GlobalInstance::vec_herosAttr[m_vocation].vec_atk[getLevel()] * POTENTIAL_BNS[m_potential];
	
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			heroatk += (takeOnEquip[i]->getAtk()*herobns);
		}
	}
	return heroatk;
}
float Hero::getDf()
{
	float herodf = GlobalInstance::vec_herosAttr[m_vocation].vec_df[getLevel()] * POTENTIAL_BNS[m_potential];
	
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				float herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
				Equip* equip = (Equip*)takeOnEquip[i];
				herodf += (equip->getDf()*herobns);
				if (GlobalInstance::map_EquipSuit[equip->getId()].vec_suit.size() >= 3)
				{
					if (MyRes::getMyPutOnResById(GlobalInstance::map_EquipSuit[equip->getId()].vec_suit[2], getName()) != NULL)
						herodf += equip->getSuitDf();
				}
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				float herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
				herodf += takeOnEquip[i]->getDf()*herobns;
			}
		}
	}

	return herodf;
}
float Hero::getMaxHp()
{
	float herohp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[getLevel()] * POTENTIAL_BNS[m_potential];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				float herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
				Equip* equip = (Equip*)takeOnEquip[i];
				herohp += (equip->getHp()*herobns);
				if (GlobalInstance::map_EquipSuit[equip->getId()].vec_suit.size() >= 2)
				{
					if (MyRes::getMyPutOnResById(GlobalInstance::map_EquipSuit[equip->getId()].vec_suit[1], getName()) != NULL)
						herohp += equip->getSuitHp();
				}
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				float herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
				herohp += takeOnEquip[i]->getHp() * herobns;
			}
		}
	}
	if (m_hp > herohp)
		m_hp = herohp;
	return herohp;
}
float Hero::getAtkSpeed()
{
	float heroatkspeed = GlobalInstance::vec_herosAttr[m_vocation].vec_atkspeed[getLevel()] * POTENTIAL_BNS[m_potential];

	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			heroatkspeed += (takeOnEquip[i]->getAtkSpeed()*herobns);
		}
	}
	return heroatkspeed;
}
float Hero::getCrit()
{
	float herocrit = GlobalInstance::vec_herosAttr[m_vocation].vec_crit[getLevel()] * POTENTIAL_BNS[m_potential];
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			herocrit += (takeOnEquip[i]->getCrit()*herobns);
		}
	}
	return herocrit;
}
float Hero::getDodge()
{
	float herododge = GlobalInstance::vec_herosAttr[m_vocation].vec_avoid[getLevel()] * POTENTIAL_BNS[m_potential];
	for (int i = 0; i < 6; i++)
	{
		if (takeOnEquip[i] != NULL)
		{
			float herobns;
			if (takeOnEquip[i]->getType() >= T_ARMOR && takeOnEquip[i]->getType() <= T_FASHION)
			{
				herobns = GlobalInstance::map_Equip[takeOnEquip[i]->getId()].vec_bns[m_vocation];
			}
			else if (takeOnEquip[i]->getType() >= T_WG && takeOnEquip[i]->getType() <= T_NG)
			{
				herobns = GlobalInstance::map_GF[takeOnEquip[i]->getId()].vec_herobns[m_vocation];
			}
			herododge += (takeOnEquip[i]->getDodge()*herobns);
		}
	}
	return herododge;
}

void Hero::generate()
{
	setVocation(GlobalInstance::getInstance()->createRandomNum(BASEHEROMAX));
	setPotential(GlobalInstance::getInstance()->createRandomNum(5));

	DynamicValueInt lvdint;
	setExp(lvdint);
	//setSex(GlobalInstance::getInstance()->createRandomNum(2));
	std::string nickname;
	bool iscontinue = true;
	do
	{
		nickname = generateName();
	} while (GlobalInstance::getInstance()->checkifSameName(nickname));
	setName(nickname);

	m_hp = getMaxHp();
}

std::string Hero::generateName()
{
	int rfirst = GlobalInstance::getInstance()->createRandomNum(FIRSTNAMECOUNT);
	int rlast = GlobalInstance::getInstance()->GlobalInstance::createRandomNum(LASTNAMECOUNT);
	std::string namestr;
	std::string heronamefile[] = { "heroname/firstname.txt" , "heroname/lastname.txt" };
	int randindex[] = { rfirst ,rlast };
	for (int i = 0; i < 2; i++)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)//android apk是压缩包无法通过fopen操作Asset文件，通过cocos的文件操作，需要读取整个文件，文件可能有1w行，所有改用java文件处理
		std::string ret;
		JniMethodInfo methodInfo;
		if (JniHelper::getStaticMethodInfo(methodInfo, ANDOIRJNICLSNAME, "readAssetStringByLine", "(Ljava/lang/String;I)Ljava/lang/String;"))
		{
			jstring para1 = methodInfo.env->NewStringUTF(heronamefile[i].c_str());
			jstring jstr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, para1, randindex[i]);
			ret = methodInfo.env->GetStringUTFChars(jstr, 0);
		}
		CommonFuncs::trim(ret);
		namestr += ret;
#else
		std::string fileName = FileUtils::getInstance()->fullPathForFilename(ResourcePath::makePath(heronamefile[i]));
		if (fileName.length() > 0)
		{
			FILE *fp = fopen(fileName.c_str(), "rb");
			if (fp)
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				fseek(fp, randindex[i] * 14, 0);//12个字节名字（4个中文），WIN32回车换行2个字节
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				fseek(fp, randindex[i] * 13, 0);//12个字节名字（4个中文），IOS回车换行1个字节
#endif
				char szReadBuff[13] = { 0 };
				size_t readsize = fread(szReadBuff, 12, 1, fp);
				if (readsize > 0)
				{
					std::string name = StringUtils::format("%s", szReadBuff);
					CommonFuncs::trim(name);
					namestr += name;
				}
				fclose(fp);
				fp = NULL;
			}

		}
#endif
	}
	return namestr;
}

Equipable* Hero::getEquipable(int etype)
{
	return takeOnEquip[etype - 1];
}
void Hero::setEquipable(Equipable* edata, int etype)
{
	//因为类型是按顺序排的，第一个被基础资源占了，所有按类型-1
	takeOnEquip[etype - 1] = edata;
}