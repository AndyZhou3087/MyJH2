#include "UnlockChapterLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"

USING_NS_CC;

UnlockChapterLayer::UnlockChapterLayer()
{

}

UnlockChapterLayer::~UnlockChapterLayer()
{

}


UnlockChapterLayer* UnlockChapterLayer::create(int unlockchapter)
{
	UnlockChapterLayer *pRet = new(std::nothrow)UnlockChapterLayer();
	if (pRet && pRet->init(unlockchapter))
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
bool UnlockChapterLayer::init(int unlockchapter)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("unlockChapterLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	std::string str;
	std::map<std::string, S_MainMap>::iterator it;
	for (it = GlobalInstance::map_mapsdata.begin(); it != GlobalInstance::map_mapsdata.end(); it++)
	{
		std::string mainmapid = it->first;
		int c = atoi(mainmapid.substr(1, mainmapid.find_first_of("-") - 1).c_str());
		if (c == unlockchapter)
		{
			str.append(GlobalInstance::map_AllResources[mainmapid].name);
			str.append(ResourceLang::map_lang["zhdunhao"]);
		}
	}
	str = StringUtils::format(ResourceLang::map_lang["unlocktext"].c_str(), unlockchapter, str.c_str());
	
	cocos2d::ui::Text* content = (cocos2d::ui::Text*)csbnode->getChildByName("content");
	content->setString(str);


	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}