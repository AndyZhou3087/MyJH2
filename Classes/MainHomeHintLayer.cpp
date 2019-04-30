#include "MainHomeHintLayer.h"
#include "Resource.h"
#include "Const.h"
#include "GlobalInstance.h"

USING_NS_CC;

MainHomeHintLayer::MainHomeHintLayer()
{

}

MainHomeHintLayer::~MainHomeHintLayer()
{

}


MainHomeHintLayer* MainHomeHintLayer::create(int type)
{
	MainHomeHintLayer*pRet = new(std::nothrow)MainHomeHintLayer();
	if (pRet && pRet->init(type))
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
bool MainHomeHintLayer::init(int type)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");


	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mainhomeHintLayer.csb"));
	this->addChild(csbnode);
	cocos2d::ui::Text* titletext = (cocos2d::ui::Text*)csbnode->getChildByName("node")->getChildByName("guide_frame")->getChildByName("guidetext");

	std::string str;
	if (type == 0)
	{
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


		str = StringUtils::format(ResourceLang::map_lang["buildingbrokenhint"].c_str(), namestr.c_str());
	}
	else if (type == 1)
		str = ResourceLang::map_lang["beggarhint"];
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

void MainHomeHintLayer::removeSelf()
{
	this->removeFromParentAndCleanup(true);
}