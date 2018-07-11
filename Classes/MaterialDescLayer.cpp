#include "MaterialDescLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
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

	m_resid = resid;
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("materialDescNode.csb"));
	this->addChild(csbnode);
	csbnode->setPosition(Vec2(437, 1092));

	m_text = (cocos2d::ui::Text*)csbnode->getChildByName("text");
	
	loadMaterialDesc();

	this->scheduleOnce(schedule_selector(MaterialDescLayer::delayRemoveSelf), 5);

	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return false;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void MaterialDescLayer::loadMaterialDesc()
{
	std::string str = "";
	int maxcount = 500;
	for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
	{
		ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
		if (rescreator->getName().compare(m_resid) == 0)
		{
			maxcount = rescreator->getMaxCap(rescreator->getLv().getValue()).getValue();
			break;
		}
	}

	int rcount = MyRes::getMyResCount(m_resid);
	std::string s = StringUtils::format("%s:", GlobalInstance::map_AllResources[m_resid].name.c_str());
	str.append(s);
	str.append(StringUtils::format("%d/%d", rcount, maxcount));

	m_text->setString(str);
}

void MaterialDescLayer::setResId(std::string resid)
{
	m_resid = resid;
	loadMaterialDesc();
	this->unschedule(schedule_selector(MaterialDescLayer::delayRemoveSelf));
	this->scheduleOnce(schedule_selector(MaterialDescLayer::delayRemoveSelf), 5);
}

void MaterialDescLayer::delayRemoveSelf(float dt)
{
	this->removeFromParentAndCleanup(true);
}

void MaterialDescLayer::onExit()
{
	Layer::onExit();
}