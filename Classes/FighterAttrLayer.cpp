#include "FighterAttrLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"

USING_NS_CC;

FighterAttrLayer::FighterAttrLayer()
{

}

FighterAttrLayer::~FighterAttrLayer()
{

}


FighterAttrLayer* FighterAttrLayer::create(Npc* npcdata)
{
	FighterAttrLayer *pRet = new(std::nothrow)FighterAttrLayer();
	if (pRet && pRet->init(npcdata))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

// on "init" you need to initialize your instance
bool FighterAttrLayer::init(Npc* npcdata)
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	csbnode = CSLoader::createNode(ResourcePath::makePath("fighterAttrLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(false);

	std::vector<float> vec_attrval;

	vec_attrval.push_back(npcdata->getHp());
	vec_attrval.push_back(npcdata->getAtk());
	vec_attrval.push_back(npcdata->getDf());
	vec_attrval.push_back(npcdata->getAtkSpeed());
	vec_attrval.push_back(npcdata->getCrit());
	vec_attrval.push_back(npcdata->getDodge());

	cocos2d::ui::Text* lvlbl = (cocos2d::ui::Text*)smallbg->getChildByName("lvtext");
	std::string lvstr = StringUtils::format(ResourceLang::map_lang["lvdetails"].c_str(), npcdata->getLevel() + 1);
	lvlbl->setString(lvstr);

	for (int i = 0; i <= 5; i++)
	{
		std::string str = StringUtils::format("attrtext_%d",i);
		cocos2d::ui::Text* attrlbl = (cocos2d::ui::Text*)smallbg->getChildByName(str);
		str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), vec_attrval[i]);
		attrlbl->setString(str);
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		this->removeFromParentAndCleanup(true);
	};
	listener->setSwallowTouches(false);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void FighterAttrLayer::setContentPos(Vec2 pos)
{
	smallbg->setPosition(pos);
}

void FighterAttrLayer::onExit()
{
	Layer::onExit();
}