#include "GlobalInstance.h"
#include "DataSave.h"
#include "Hero.h"
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

}