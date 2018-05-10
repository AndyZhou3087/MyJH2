#include "GlobalInstance.h"
#include "DataSave.h"
#include "Hero.h"
#include "CommonFuncs.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

std::map<std::string, Hero*> GlobalInstance::map_myHeros;
std::vector<Hero*> GlobalInstance::vec_rand3Heros;

GlobalInstance::GlobalInstance()
{

}
GlobalInstance::~GlobalInstance()
{

}

GlobalInstance* GlobalInstance::_Context = NULL;

GlobalInstance* GlobalInstance::getInstance() {
	if (_Context == NULL) {
		_Context = new GlobalInstance;
	}
	return _Context;
}

void GlobalInstance::saveHeros()
{
	std::map<std::string, Hero*>::iterator hit;

	int i = 0;
	for (hit = GlobalInstance::map_myHeros.begin(); hit != GlobalInstance::map_myHeros.end(); hit++)
	{
		std::string herokey = StringUtils::format("hero%d", i);
		Hero* hero = GlobalInstance::map_myHeros[hit->first];
		std::string datastr = StringUtils::format("%s-%d-%d-%d-%d;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex());
		DataSave::getInstance()->setHeroData(herokey, datastr);
	}
}

void GlobalInstance::loadHeros()
{
	for (int i = 0; i < 50; i++)
	{
		std::string herokey = StringUtils::format("hero%d", i);
		std::string herodatastr = DataSave::getInstance()->getHeroData(herokey);
		if (herodatastr.length() > 0)
		{
			Hero* hero = NULL;
			std::vector<std::string> vec_retstr;
			CommonFuncs::split(herodatastr, vec_retstr, ";");
			if (vec_retstr.size() > 0)//人物属性
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(vec_retstr[0], vec_tmp, "-");
				if (vec_tmp.size() >= 5)
				{
					hero = new Hero();
					hero->setName(vec_tmp[0]);
					DynamicValueInt expv;
					expv.setValue(atoi(vec_tmp[1].c_str()));
					hero->setExp(expv);
					hero->setVocation(atoi(vec_tmp[2].c_str()));
					hero->setPotential(atoi(vec_tmp[3].c_str()));
					hero->setSex(atoi(vec_tmp[4].c_str()));
				}
			}
			if (vec_retstr.size() > 1)//装备属性
			{
				if (hero != NULL)
				{

				}
			}
			if (hero != NULL)
				GlobalInstance::map_myHeros[hero->getName()] = hero;
		}
	}
}