#include "MatchVSLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"

USING_NS_CC;

MatchVSLayer::MatchVSLayer()
{

}

MatchVSLayer::~MatchVSLayer()
{

}


MatchVSLayer* MatchVSLayer::create()
{
	MatchVSLayer *pRet = new(std::nothrow)MatchVSLayer();
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
bool MatchVSLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	csbnode = CSLoader::createNode(ResourcePath::makePath("matchVSLayer.csb"));
	this->addChild(csbnode);
	//auto action = CSLoader::createTimeline(ResourcePath::makePath("matchVSLayer.csb"));
	//effectnode->runAction(action);
	//action->gotoFrameAndPlay(0, false);

	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void MatchVSLayer::loadData()
{
	cocos2d::ui::ImageView* myheroboxs[6];
	cocos2d::ui::ImageView* myheroheads[6];

	cocos2d::ui::ImageView* pairheroboxs[6];
	cocos2d::ui::ImageView* pairheroheads[6];

	for (int i = 0; i < 6; i++)
	{
		std::string str = StringUtils::format("mhero%d", i);
		myheroboxs[i] = (cocos2d::ui::ImageView*)csbnode->getChildByName("matchbox0")->getChildByName(str);
		myheroheads[i] = (cocos2d::ui::ImageView*)myheroboxs[i]->getChildByName("headimg");

		Hero* myhero = GlobalInstance::myOnChallengeHeros[i];
		if (myhero != NULL)
		{
			str = StringUtils::format("ui/h_%d_%d.png", myhero->getVocation(), myhero->getSex());
			myheroheads[i]->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

			str = StringUtils::format("ui/herobox_%d.png", myhero->getPotential());
			myheroboxs[i]->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			myheroboxs[i]->setVisible(false);
		}

		str = StringUtils::format("phero%d", i);
		pairheroboxs[i] = (cocos2d::ui::ImageView*)csbnode->getChildByName("matchbox1")->getChildByName(str);
		pairheroheads[i] = (cocos2d::ui::ImageView*)myheroboxs[i]->getChildByName("headimg");
	}


}