#include "Hero.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"

Hero::Hero()
{
	m_state = 0;
	m_breakupper = 0;
	m_randattr = 0.0f;
	m_pos = 0;
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
	float heroatk = GlobalInstance::vec_herosAttr[m_vocation].vec_atk[getLevel()];

	return heroatk;
}
float Hero::getDf()
{
	float herodf = GlobalInstance::vec_herosAttr[m_vocation].vec_df[getLevel()];
	return herodf;
}
float Hero::getMaxHp()
{
	float herohp = GlobalInstance::vec_herosAttr[m_vocation].vec_maxhp[getLevel()];
	return herohp;
}
float Hero::getAtkSpeed()
{
	float heroatkspeed = GlobalInstance::vec_herosAttr[m_vocation].vec_atkspeed[getLevel()];
	return heroatkspeed;
}
float Hero::getCrit()
{
	float herocrit = GlobalInstance::vec_herosAttr[m_vocation].vec_crit[getLevel()];
	return herocrit;
}
float Hero::getDodge()
{
	float herododge = GlobalInstance::vec_herosAttr[m_vocation].vec_avoid[getLevel()];
	return herododge;
}

void Hero::generate()
{
	setVocation(rand() % HEROMAX);
	setPotential(rand() % 5);

	DynamicValueInt lvdint;
	setExp(lvdint);
	setSex(rand() % 2);
	std::string nickname;
	bool iscontinue = true;
	do
	{
		nickname = generateName();
	} while (GlobalInstance::getInstance()->checkifSameName(nickname));
	setName(nickname);

	m_myhp = getMaxHp();
}

std::string Hero::generateName()
{
	int rfirst = rand() % FIRSTNAMECOUNT;
	int rlast = rand() % LASTNAMECOUNT;
	std::string namestr;
	std::string heronamefile[] = { "heroname/firstname.txt" , "heroname/lastname.txt" };
	int randindex[] = { rand() % FIRSTNAMECOUNT , rand() % FIRSTNAMECOUNT };
	for (int i = 0; i < 2; i++)
	{
		std::string fileName = FileUtils::getInstance()->fullPathForFilename(ResourcePath::makePath(heronamefile[i]));

		if (fileName.length() > 0)
		{
			FILE *fp = fopen(fileName.c_str(), "r");
			if (fp)
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				fseek(fp, randindex[i] * 14, 0);//12个字节名字（4个中文），WIN32回车换行2个字节
#else
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
			}
			fclose(fp);
			fp = NULL;
		}
	}
	return namestr;
}