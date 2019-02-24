#include "MarketRefreshLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "MarketLayer.h"
#include "AnimationEffect.h"
#include "BuyCoinLayer.h"

USING_NS_CC;


MarketRefreshLayer::MarketRefreshLayer()
{
	refreshCoin.setValue(COINREFRESH_MARKET_NUM);

	refreshSilver.setValue(SILVERREFRESH_MARKET_NUM);
}

MarketRefreshLayer::~MarketRefreshLayer()
{

}


MarketRefreshLayer* MarketRefreshLayer::create()
{
	MarketRefreshLayer *pRet = new(std::nothrow)MarketRefreshLayer();
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
bool MarketRefreshLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("marketRefreshLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//按钮
	cocos2d::ui::Widget* coinbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("coinbtn");
	coinbtn->addTouchEventListener(CC_CALLBACK_2(MarketRefreshLayer::onreBtnClick, this));
	coinbtn->setTag(10);

	cocos2d::ui::Widget* sliverbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("sliverbtn");
	sliverbtn->addTouchEventListener(CC_CALLBACK_2(MarketRefreshLayer::onreBtnClick, this));
	sliverbtn->setTag(100);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MarketRefreshLayer::closebtnClick, this));

	cocos2d::ui::Text* attactlb = (cocos2d::ui::Text*)csbnode->getChildByName("attactlb");
	attactlb->setString(ResourceLang::map_lang["marketrefreshdesc"]);

	cocos2d::ui::Text* cointext = (cocos2d::ui::Text*)csbnode->getChildByName("cointext");
	std::string str = StringUtils::format("x%d", COINREFRESH_MARKET_NUM);
	cointext->setString(str);

	cocos2d::ui::Text* slivertext = (cocos2d::ui::Text*)csbnode->getChildByName("slivertext");
	str = StringUtils::format("x%d", refreshSilver.getValue());
	slivertext->setString(str);

	//屏蔽下层点击
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

void MarketRefreshLayer::onreBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 10://元宝刷新
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= refreshCoin.getValue())
			{
				DynamicValueInt dval;
				dval.setValue(refreshCoin.getValue());
				GlobalInstance::getInstance()->costMyCoinCount(dval);
				MarketLayer* market = (MarketLayer*)this->getParent();
				if (market!=NULL)
				{
					market->resetStockRes();
				}
				AnimationEffect::closeAniEffect((Layer*)this);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
				Layer* layer = BuyCoinLayer::create(refreshCoin.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
			}
			break;
		case 100://银两刷新
			if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= refreshSilver.getValue())
			{
				DynamicValueInt dval;
				dval.setValue(refreshSilver.getValue());
				GlobalInstance::getInstance()->costMySoliverCount(dval);
				MarketLayer* market = (MarketLayer*)this->getParent();
				if (market != NULL)
				{
					market->resetStockRes();
				}
				AnimationEffect::closeAniEffect((Layer*)this);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomoresilver"]);
			}
			break;
		default:
			break;
		}
	}
}

void MarketRefreshLayer::closebtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void MarketRefreshLayer::onExit()
{
	Layer::onExit();
}