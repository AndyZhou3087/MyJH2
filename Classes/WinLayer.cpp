#include "WinLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "FightHeroNode.h"

USING_NS_CC;

WinLayer::WinLayer()
{

}

WinLayer::~WinLayer()
{

}


WinLayer* WinLayer::create(std::vector<FOURProperty> reward_res, int winexp)
{
	WinLayer *pRet = new(std::nothrow)WinLayer();
	if (pRet && pRet->init(reward_res, winexp))
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
bool WinLayer::init(std::vector<FOURProperty> reward_res, int winexp)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("winLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//°´Å¥
	cocos2d::ui::Widget* escapebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	escapebtn->addTouchEventListener(CC_CALLBACK_2(WinLayer::onBtnClick, this));

	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)escapebtn->getChildByName("text");

	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("continuebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(145 + i%3*215, 680 -  i/3*280);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i], F_HERO, FS_SUCC);
		addChild(fightHeroNode, 0, i);
		if (GlobalInstance::myCardHeros[i] != NULL)
			fightHeroNode->setWinState(100);
	}

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

void WinLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		
	}
}

void WinLayer::onExit()
{
	Layer::onExit();
}