#include "Hero.h"
#include "Resource.h"
#include "CommonFuncs.h"

#define FIRSTNAMECOUNT 40
#define LASTNAMECOUNT 40

Hero::Hero()
{
	m_state = 0;
	m_breakupper = 0;
	m_randattr = 0.0f;
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
	m_state = 0;
	m_breakupper = 0;
}

int Hero::getLevel()
{
	return 0;
}

int Hero::getAtk()
{
	return 0;
}
int Hero::getDf()
{
	return 0;
}
int Hero::getHp()
{
	return 0;
}
int Hero::getAtkSpeed()
{
	return 0;
}
int Hero::getCrit()
{
	return 0;
}
int Hero::getDodge()
{
	return 0;
}

void Hero::generate()
{
	setVocation(rand() % HEROMAX);
	setPotential(rand() % 5);

	DynamicValueInt lvdint;
	setExp(lvdint);
	setSex(rand() % 2);
	setName(generateName());
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
				fseek(fp, randindex[i] * 14, 0);//12个字节名字（4个中文），回车换行2个字节
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