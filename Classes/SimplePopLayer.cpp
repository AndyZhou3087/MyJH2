#include "SimplePopLayer.h"
#include "Resource.h"
#include "Const.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "AnimationEffect.h"

USING_NS_CC;

SimplePopLayer::SimplePopLayer()
{

}

SimplePopLayer::~SimplePopLayer()
{

}


SimplePopLayer* SimplePopLayer::create(std::string content)
{
	SimplePopLayer *pRet = new(std::nothrow)SimplePopLayer();
	if (pRet && pRet->init(content))
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
bool SimplePopLayer::init(std::string content)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 120));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	content = CommonFuncs::replace_all(content, "\\n", "\n");
	Label* contentlbl = Label::createWithTTF(content, FONT_NAME, 25);
	contentlbl->setColor(Color3B(255, 255, 255));
	contentlbl->setHorizontalAlignment(TextHAlignment::LEFT);
	contentlbl->setVerticalAlignment(TextVAlignment::CENTER);
	contentlbl->enableShadow(Color4B::BLACK, Size(1, -1));
	contentlbl->setLineSpacing(10);
	//contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setMaxLineWidth(500);
	contentlbl->setPosition(Vec2(360, 720));
	this->addChild(contentlbl, 1);

	cocos2d::ui::Scale9Sprite* bg = cocos2d::ui::Scale9Sprite::create("images/smallbg.png");
	bg->setPosition(Vec2(360, 720));
	bg->setContentSize(Size(600, 245 + contentlbl->getStringNumLines() * 25));
	this->addChild(bg);

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