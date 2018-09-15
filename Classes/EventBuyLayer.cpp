#include "EventBuyLayer.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "Const.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "SoundManager.h"

EventBuyLayer::EventBuyLayer()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	buycount = 1;
}


EventBuyLayer::~EventBuyLayer()
{

}

EventBuyLayer* EventBuyLayer::create(FOURProperty fproper)
{
	EventBuyLayer *pRet = new(std::nothrow)EventBuyLayer();
	if (pRet && pRet->init(fproper))
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

bool EventBuyLayer::init(FOURProperty fproper)
{
	if (!Layer::init())
	{
		return false;
	}

	m_fproper = fproper;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("eventBuyLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* smallbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	std::string resid = fproper.sid;
	int qu = 0;
	std::string qustr = "ui/resbox.png";

	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		qu = fproper.intPara2;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	resbox->loadTexture(ResourcePath::makePath(qustr), cocos2d::ui::Widget::TextureResType::PLIST);

	std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

	cocos2d::ui::ImageView* resimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	resimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[resid].name);
	namelbl->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	//描述
	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(GlobalInstance::map_AllResources[resid].desc);

	cocos2d::ui::Text* pricetext = (cocos2d::ui::Text*)csbnode->getChildByName("pricetext");
	pricetext->setString(ResourceLang::map_lang["buypricetext"]);

	cocos2d::ui::Text* counttext = (cocos2d::ui::Text*)csbnode->getChildByName("counttext");
	counttext->setString(ResourceLang::map_lang["buycounttext"]);

	totalpricelbl = (cocos2d::ui::Text*)csbnode->getChildByName("totalprice");

	priceicon = (cocos2d::ui::ImageView*)csbnode->getChildByName("priceicon");
	priceicon->ignoreContentAdaptWithSize(true);

	if (GlobalInstance::map_AllResources[resid].coinval > 0)
		priceicon->loadTexture(ResourcePath::makePath("ui/main_coin.png"), cocos2d::ui::Widget::TextureResType::PLIST);

	rescountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("rescount");

	updateData();

	//按钮
	cocos2d::ui::Widget* buybtn = (cocos2d::ui::Widget*)csbnode->getChildByName("buybtn");
	buybtn->setTag(100);
	buybtn->addTouchEventListener(CC_CALLBACK_2(EventBuyLayer::onBtnClick, this));

	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	cocos2d::ui::ImageView* buybtntxt = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
	buybtntxt->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//取消按钮
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	cancelbtn->setTag(101);
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(EventBuyLayer::onBtnClick, this));

	//取消按钮文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//加号按钮
	cocos2d::ui::Widget* addbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("addbtn");
	addbtn->addTouchEventListener(CC_CALLBACK_2(EventBuyLayer::onAddBtnClick, this));
	addbtn->setTag(1001);
	//减号按钮
	cocos2d::ui::Widget* subbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("subbtn");
	subbtn->addTouchEventListener(CC_CALLBACK_2(EventBuyLayer::onSubBtnClick, this));
	subbtn->setTag(1002);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void EventBuyLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);

	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		switch (tag)
		{
		case 100://购买按钮
		{
			std::string showstr;

			int saleval = 0;
			DynamicValueInt myrich;
			bool iscoinsale = false;
			if (GlobalInstance::map_AllResources[m_fproper.sid].coinval > 0)
			{
				saleval = GlobalInstance::map_AllResources[m_fproper.sid].coinval;
				myrich.setValue(GlobalInstance::getInstance()->getMyCoinCount().getValue());
				iscoinsale = true;
			}
			else
			{
				saleval = GlobalInstance::map_AllResources[m_fproper.sid].silverval;
				myrich.setValue(GlobalInstance::getInstance()->getMySoliverCount().getValue());
			}

			if (myrich.getValue() < buycount * saleval)
			{
				if (iscoinsale)
					showstr = ResourceLang::map_lang["nomorecoin"];
				else
					showstr = ResourceLang::map_lang["nomoresilver"];
			}
			else
			{
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUYRES);
				m_fproper.intPara1 = buycount;
				DynamicValueInt dv;
				dv.setValue(buycount * saleval);
				if (iscoinsale)
					GlobalInstance::getInstance()->costMyCoinCount(dv);
				else
					GlobalInstance::getInstance()->costMySoliverCount(dv);

				EventBusinessLayer* layer = (EventBusinessLayer*)this->getParent();
				layer->buyRes(m_fproper);
				AnimationEffect::closeAniEffect((Layer*)this);
			}
			MovingLabel::show(showstr);
			break;
		}
		case 101:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}
	}
}

void EventBuyLayer::onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(EventBuyLayer::longTouchUpdate)))
			schedule(schedule_selector(EventBuyLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		addCount();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void EventBuyLayer::onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(EventBuyLayer::longTouchUpdate)))
			schedule(schedule_selector(EventBuyLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		subCount();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void EventBuyLayer::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		if (m_longTouchNode->getTag() % 1000 == 1)
			addCount();
		else if (m_longTouchNode->getTag() % 1000 == 2)
			subCount();
	}
}

void EventBuyLayer::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(EventBuyLayer::longTouchUpdate));
}

void EventBuyLayer::addCount()
{
	if (buycount >= m_fproper.intPara1)
	{
		MovingLabel::show(ResourceLang::map_lang["outofstock"]);
	}
	else
	{
		buycount++;
		updateData();
	}
}

void EventBuyLayer::subCount()
{
	if (buycount > 1)
	{
		buycount--;
		updateData();
	}
}

void EventBuyLayer::updateData()
{	
	int saleval = 0;
	DynamicValueInt myrich;
	std::string resid = m_fproper.sid;
	if (GlobalInstance::map_AllResources[resid].coinval > 0)
	{
		saleval = GlobalInstance::map_AllResources[resid].coinval;
		myrich.setValue(GlobalInstance::getInstance()->getMyCoinCount().getValue());
	}
	else
	{
		saleval = GlobalInstance::map_AllResources[resid].silverval;
		myrich.setValue(GlobalInstance::getInstance()->getMySoliverCount().getValue());
	}

	if (myrich.getValue() < buycount * saleval)
	{
		totalpricelbl->setColor(Color3B(255, 0, 0));
	}
	else
	{
		totalpricelbl->setColor(Color3B(255, 191, 0));
	}

	std::string salestr = StringUtils::format("%d", buycount * saleval);
	totalpricelbl->setString(salestr);

	std::string countstr = StringUtils::format("%d/%d", buycount, m_fproper.intPara1);
	rescountlbl->setString(countstr);
}