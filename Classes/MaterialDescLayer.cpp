#include "MaterialDescLayer.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"

USING_NS_CC;

MaterialDescLayer::MaterialDescLayer()
{
	
}

MaterialDescLayer::~MaterialDescLayer()
{

}


MaterialDescLayer* MaterialDescLayer::create(std::string resid)
{
	MaterialDescLayer *pRet = new(std::nothrow)MaterialDescLayer();
	if (pRet && pRet->init(resid))
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
bool MaterialDescLayer::init(std::string resid)
{
	if (!Layer::init())
	{
		return false;
	}

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("materialDescNode.csb"));
	this->addChild(csbnode);
	csbnode->setPosition(Vec2(437, 1092));

	cocos2d::ui::Text* text = (cocos2d::ui::Text*)csbnode->getChildByName("text");
	
	std::string str = "";
	int maxcount = 500;
	for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
	{
		ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
		if (rescreator->getName().compare(resid) == 0)
		{
			maxcount = rescreator->getMaxCap(rescreator->getLv().getValue()).getValue();
			break;
		}
	}

	int rcount = MyRes::getMyResCount(resid);
	std::string s = StringUtils::format("%s:", GlobalInstance::map_AllResources[resid].name.c_str());
	str.append(s);
	str.append(StringUtils::format("%d/%d", rcount, maxcount));

	text->setString(str);

	this->scheduleOnce(schedule_selector(MaterialDescLayer::delayRemoveSelf), 3);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		delayRemoveSelf(0);
	};
	listener->setSwallowTouches(false);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}


void MaterialDescLayer::delayRemoveSelf(float dt)
{
	this->removeFromParentAndCleanup(true);
}

void MaterialDescLayer::onExit()
{
	Layer::onExit();
}