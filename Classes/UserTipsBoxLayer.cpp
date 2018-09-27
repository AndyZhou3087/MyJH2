#include "UserTipsBoxLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "UserProtocolLayer.h"
#include "LoadingScene.h"
#include "AnimationEffect.h"

UserTipsBoxLayer::UserTipsBoxLayer()
{

}


UserTipsBoxLayer::~UserTipsBoxLayer()
{

}

UserTipsBoxLayer* UserTipsBoxLayer::create()
{
	UserTipsBoxLayer *pRet = new(std::nothrow)UserTipsBoxLayer();
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

bool UserTipsBoxLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("UserTipsBoxLayer.csb"));
	this->addChild(m_csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* usertitle = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("usertitle");
	usertitle->loadTexture(ResourcePath::makeTextImgPath("usertitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* titletext = (cocos2d::ui::Text*)m_csbnode->getChildByName("titletext");
	titletext->setString(ResourceLang::map_lang["refuseusertext"]);

	cocos2d::ui::Button* agreebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("agreebtn");
	agreebtn->setTag(1);
	agreebtn->addTouchEventListener(CC_CALLBACK_2(UserTipsBoxLayer::onBtnClick, this));
	cocos2d::ui::ImageView* agreetext = (cocos2d::ui::ImageView*)agreebtn->getChildByName("text");
	agreetext->loadTexture(ResourcePath::makeTextImgPath("agreetext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	agreetext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("agreetext", langtype))->getContentSize());

	cocos2d::ui::Button* refusebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("refusebtn");
	refusebtn->setTag(0);
	refusebtn->addTouchEventListener(CC_CALLBACK_2(UserTipsBoxLayer::onBtnClick, this));
	cocos2d::ui::ImageView* refusetext = (cocos2d::ui::ImageView*)refusebtn->getChildByName("text");
	refusetext->loadTexture(ResourcePath::makeTextImgPath("refusetext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	refusetext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("refusetext", langtype))->getContentSize());


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

void UserTipsBoxLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)pSender;
		int tag = btn->getTag();

		UserProtocolLayer* layer = (UserProtocolLayer*)this->getParent();
		LoadingScene* load = (LoadingScene*)layer->getParent();
		load->setUserProtocol(tag);
		AnimationEffect::closeAniEffect((Layer*)layer);

	}
}