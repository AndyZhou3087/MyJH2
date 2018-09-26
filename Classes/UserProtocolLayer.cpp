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

UserProtocolLayer* userProlayer = NULL;
UserProtocolLayer::UserProtocolLayer()
{

}


UserProtocolLayer::~UserProtocolLayer()
{
	userProlayer = NULL;
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

	cocos2d::ui::Text* titletext = (cocos2d::ui::Text*)m_csbnode->getChildByName("titletext");
	titletext->setString(ResourceLang::map_lang["usertile"]);

	cocos2d::ui::Button* agreebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("agreebtn");
	agreebtn->setTag(1);
	agreebtn->addTouchEventListener(CC_CALLBACK_2(UserProtocolLayer::onBtnClick, this));
	cocos2d::ui::ImageView* agreetext = (cocos2d::ui::ImageView*)agreebtn->getChildByName("text");
	agreetext->loadTexture(ResourcePath::makeTextImgPath("agreetext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	agreetext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("agreetext", langtype))->getContentSize());

	cocos2d::ui::Button* refusebtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("refusebtn");
	refusebtn->setTag(0);
	refusebtn->addTouchEventListener(CC_CALLBACK_2(UserProtocolLayer::onBtnClick, this));
	cocos2d::ui::ImageView* refusetext = (cocos2d::ui::ImageView*)refusebtn->getChildByName("text");
	refusetext->loadTexture(ResourcePath::makeTextImgPath("refusetext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	refusetext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("refusetext", langtype))->getContentSize());

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("scrollView");

	std::string wordstr = ResourceLang::map_lang["usercontent"];
	Label* m_wordlbl = Label::createWithTTF(wordstr, FONT_NAME, 26);
	m_wordlbl->setLineBreakWithoutSpace(true);
	m_wordlbl->setColor(Color3B(92, 92, 92));
	m_wordlbl->setAnchorPoint(Vec2(0, 1));
	m_wordlbl->setMaxLineWidth(scrollView->getContentSize().width);
	scrollView->addChild(m_wordlbl, 0, "talklbl");

	int innerheight = m_wordlbl->getStringNumLines() * 26;//contentlbl->getHeight();
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));
	m_wordlbl->setPosition(Vec2(0, innerheight));


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
		case 0://¾Ü¾ø
		{
			UserTipsBoxLayer* layer = UserTipsBoxLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case 1://Í¬Òâ
		{
			LoadingScene* load = (LoadingScene*)this->getParent();
			load->setUserProtocol(tag);
			AnimationEffect::closeAniEffect((Layer*)this);
		}
			break;
		default:
			break;
		}
		
	}
}