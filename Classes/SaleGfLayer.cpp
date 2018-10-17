#include "SaleGfLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "StoreHouseLayer.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "EquipDescLayer.h"
#include "DynamicValue.h"
#include "StoreHouseLayer.h"

USING_NS_CC;

SaleGfLayer::SaleGfLayer()
{

}

SaleGfLayer::~SaleGfLayer()
{

}


SaleGfLayer* SaleGfLayer::create(ResBase* res)
{
	SaleGfLayer *pRet = new(std::nothrow)SaleGfLayer();
	if (pRet && pRet->init(res))
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
bool SaleGfLayer::init(ResBase* res)
{
	if (!Layer::init())
	{
		return false;
	}

	m_res = res;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("resDescLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");

	int s = GlobalInstance::map_GF[m_res->getId()].qu;
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", s);
	CommonFuncs::playResBoxEffect(resbox, s);
	resbox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", res->getId().c_str());
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);


	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[res->getId()].name);

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desclbl");
	desclbl->setString(ResourceLang::map_lang["salegftext"]);

	str = StringUtils::format(ResourceLang::map_lang["salegftitle"].c_str(), GlobalInstance::map_AllResources[res->getId()].silverval);
	cocos2d::ui::Text* coutlbl = (cocos2d::ui::Text*)csbnode->getChildByName("coutlbl");
	coutlbl->setVisible(false);
	cocos2d::ui::Text* salecount = (cocos2d::ui::Text*)csbnode->getChildByName("salecount");
	salecount->setString(str);
	salecount->setVisible(true);

	//°´Å¥
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(SaleGfLayer::onBtnClick, this));
	actionbtn->setPositionX(490);
	actionbtn->setTag(1);
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntxt->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("okbtn_text", langtype))->getContentSize());
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(SaleGfLayer::onBtnClick, this));
	closebtn->setTag(0);
	closebtn->setVisible(true);
	cocos2d::ui::ImageView* closebtntxt = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	closebtntxt->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	closebtntxt->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("backbtn_text", langtype))->getContentSize());


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

void SaleGfLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag == 0)
		{
			AnimationEffect::closeAniEffect((Layer*)this);
		}
		else if (tag == 1)
		{
			DynamicValueInt dal;
			dal.setValue(GlobalInstance::map_AllResources[m_res->getId()].silverval);
			GlobalInstance::getInstance()->addMySoliverCount(dal);

			MyRes::Use(m_res);
			if (g_mainScene != NULL)
			{
				StoreHouseLayer* mlayer = (StoreHouseLayer*)g_mainScene->getChildByName("3storehouse");
				if (mlayer != NULL)
				{
					mlayer->updateUI();
				}
			}

			MovingLabel::show(ResourceLang::map_lang["salegftips"]);

			AnimationEffect::closeAniEffect((Layer*)this->getParent());
		}
	}
}

void SaleGfLayer::onExit()
{
	Layer::onExit();
}