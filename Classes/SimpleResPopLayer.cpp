#include "SimpleResPopLayer.h"
#include "Resource.h"
#include "Const.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "AnimationEffect.h"

USING_NS_CC;

SimpleResPopLayer::SimpleResPopLayer()
{

}

SimpleResPopLayer::~SimpleResPopLayer()
{

}


SimpleResPopLayer* SimpleResPopLayer::create(std::string resid)
{
	SimpleResPopLayer *pRet = new(std::nothrow)SimpleResPopLayer();
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
bool SimpleResPopLayer::init(std::string resid)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 120));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("simpleResPopLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	std::string resboxstr;
	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	int qu = 0;
	std::string qustr = "ui/resbox.png";

	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	resbox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desclbl");
	desclbl->setString(GlobalInstance::map_AllResources[resid].desc);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[resid].name);


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