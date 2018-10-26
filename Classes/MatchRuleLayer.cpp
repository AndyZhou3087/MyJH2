#include "MatchRuleLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"

USING_NS_CC;

MatchRuleLayer::MatchRuleLayer()
{

}

MatchRuleLayer::~MatchRuleLayer()
{

}


MatchRuleLayer* MatchRuleLayer::create()
{
	MatchRuleLayer *pRet = new(std::nothrow)MatchRuleLayer();
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
bool MatchRuleLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 160));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchRuleLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["matchruletitle"]);

	cocos2d::ui::Text* title2 = (cocos2d::ui::Text*)csbnode->getChildByName("title2");
	title2->setString(ResourceLang::map_lang["matchrulecontent"]);
	title2->setVisible(false);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");
	Label* contentlbl = Label::createWithTTF(ResourceLang::map_lang["matchruletext"], FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(255, 255, 255));
	contentlbl->enableShadow(Color4B::BLACK, Size(1, -1));
	contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setMaxLineWidth(scrollView->getContentSize().width);
	scrollView->addChild(contentlbl);
	int innerheight = contentlbl->getStringNumLines() * 25;//contentlbl->getHeight();
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));
	contentlbl->setPosition(Vec2(0, innerheight));
	

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