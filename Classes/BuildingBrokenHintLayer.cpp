#include "BuildingBrokenHintLayer.h"
#include "Resource.h"
#include "Const.h"
#include "GlobalInstance.h"

USING_NS_CC;

BuildingBrokenHintLayer::BuildingBrokenHintLayer()
{

}

BuildingBrokenHintLayer::~BuildingBrokenHintLayer()
{

}


BuildingBrokenHintLayer* BuildingBrokenHintLayer::create()
{
	BuildingBrokenHintLayer*pRet = new(std::nothrow)BuildingBrokenHintLayer();
	if (pRet && pRet->init())
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
bool BuildingBrokenHintLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");


	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("buildingBrokenHintLayer.csb"));
	this->addChild(csbnode);

	std::string namestr;
	std::map<std::string, S_BUILDINREPAIR>::iterator it;
	
	for (it = GlobalInstance::map_buildingrepairdata.begin(); it != GlobalInstance::map_buildingrepairdata.end(); it++)
	{
		if (it->second.state == 1)
		{
			if (namestr.length() > 0)
				namestr.append(ResourceLang::map_lang["dunhao"]);
			namestr.append(GlobalInstance::map_AllResources[it->first].name);

			GlobalInstance::map_buildingrepairdata[it->first].state = 2;
		}
	}

	cocos2d::ui::Text* titletext = (cocos2d::ui::Text*)csbnode->getChildByName("node")->getChildByName("guide_frame")->getChildByName("guidetext");
	std::string str = StringUtils::format(ResourceLang::map_lang["buildingbrokenhint"].c_str(), namestr.c_str());
	titletext->setString(str);
	
	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch * touch, Event * event)
	{
		removeSelf();
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void BuildingBrokenHintLayer::removeSelf()
{
	this->removeFromParentAndCleanup(true);
}