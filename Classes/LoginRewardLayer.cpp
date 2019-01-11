#include "LoginRewardLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"

USING_NS_CC;

LoginRewardLayer::LoginRewardLayer()
{

}

LoginRewardLayer::~LoginRewardLayer()
{

}


LoginRewardLayer* LoginRewardLayer::create()
{
	LoginRewardLayer *pRet = new(std::nothrow)LoginRewardLayer();
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
bool LoginRewardLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");


	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("loginRewardLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();


	int logindays = GlobalInstance::loginDays;

	if (logindays > 8)
		logindays = 8;

	for (int i = 0; i < 8;i++)
	{
		std::string name = StringUtils::format("day%d", i + 1);
		Node* daynode = csbnode->getChildByName(name);
		cocos2d::ui::Button* getbtn = (cocos2d::ui::Button*)csbnode->getChildByName("getbtn");

		cocos2d::ui::Text* daytext = (cocos2d::ui::Text*)csbnode->getChildByName("daytext");

		if (i < 7)
		{

		}

		if (i < logindays)
		{
			cocos2d::ui::ImageView* getbtntext = (cocos2d::ui::ImageView*)getbtn->getChildByName("text");
			getbtntext->ignoreContentAdaptWithSize(true);

			if (i == logindays - 1 && !GlobalInstance::isGetLoginRwd)
			{
				getbtn->addTouchEventListener(CC_CALLBACK_2(LoginRewardLayer::onBtnClick, this));
			}
			else
			{
				getbtn->setEnabled(false);
				getbtn->setBright(true);
				getbtntext->loadTexture(ResourcePath::makeTextImgPath("msggeted_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
			getbtntext->setScale(0.8f);
		}
		else
		{
			getbtn->setVisible(false);
		}
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

void LoginRewardLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
	}
}

void LoginRewardLayer::removeSelf()
{
	AnimationEffect::closeAniEffect((Layer*)this);

}