#include "UserProtocolLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "UserTipsBoxLayer.h"
#include "AnimationEffect.h"
#include "LoadingScene.h"
#include "DataSave.h"

UserProtocolLayer::UserProtocolLayer()
{

}


UserProtocolLayer::~UserProtocolLayer()
{

}

UserProtocolLayer* UserProtocolLayer::create()
{
	UserProtocolLayer *pRet = new(std::nothrow)UserProtocolLayer();
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

bool UserProtocolLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("UserProtocolLayer.csb"));
	this->addChild(m_csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* usertitle = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("usertitle");
	usertitle->loadTexture(ResourcePath::makeTextImgPath("usertitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Button* agreebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("agreebtn");
	agreebtn->setTag(1);
	agreebtn->addTouchEventListener(CC_CALLBACK_2(UserProtocolLayer::onBtnClick, this));
	cocos2d::ui::ImageView* agreetext = (cocos2d::ui::ImageView*)agreebtn->getChildByName("text");
	agreetext->ignoreContentAdaptWithSize(true);
	agreetext->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	agreebtn->setPositionX(360);

	cocos2d::ui::Button* refusebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("refusebtn");
	refusebtn->setTag(0);
	refusebtn->addTouchEventListener(CC_CALLBACK_2(UserProtocolLayer::onBtnClick, this));
	cocos2d::ui::ImageView* refusetext = (cocos2d::ui::ImageView*)refusebtn->getChildByName("text");
	refusetext->loadTexture(ResourcePath::makeTextImgPath("refusetext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	refusetext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("refusetext", langtype))->getContentSize());

	refusebtn->setVisible(false);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("scrollView");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	cocos2d::experimental::ui::WebView * webView = cocos2d::experimental::ui::WebView::create();
	webView->setContentSize(scrollView->getContentSize());
	webView->setPosition(Vec2(scrollView->getContentSize().width/2, scrollView->getContentSize().height/2));
	webView->loadFile("userprotocol.html");
    scrollView->addChild(webView);
#endif
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	/*listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		
	};*/
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void UserProtocolLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)pSender;
		int tag = btn->getTag();

		switch (tag)
		{
		case 0:
		{
			UserTipsBoxLayer* layer = UserTipsBoxLayer::create();
			this->getParent()->addChild(layer);
			this->removeFromParentAndCleanup(true);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case 1:
		{
			//DataSave::getInstance()->setUserProtocal(tag);
			AnimationEffect::closeAniEffect((Layer*)this);
		}
			break;
		default:
			break;
		}
		
	}
}