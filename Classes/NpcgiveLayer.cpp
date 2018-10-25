#include "NpcgiveLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "Const.h"
#include "DataSave.h"
#include "MyRes.h"
#include "NpcLayer.h"

USING_NS_CC;

NpcgiveLayer::NpcgiveLayer()
{
	lastSelectIndex = 0;
	lastIndexCount = 0;
	m_isLongPress = false;
	m_longTouchNode = NULL;
}

NpcgiveLayer::~NpcgiveLayer()
{

}


NpcgiveLayer* NpcgiveLayer::create(std::string npcid)
{
	NpcgiveLayer *pRet = new(std::nothrow)NpcgiveLayer();
	if (pRet && pRet->init(npcid))
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
bool NpcgiveLayer::init(std::string npcid)
{
	if (!Layer::init())
	{
		return false;
	}

	m_npcid = npcid;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("npcgiveLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["npcgivetitle"]);

	cocos2d::ui::Text* tips = (cocos2d::ui::Text*)csbnode->getChildByName("tips");
	tips->setString(ResourceLang::map_lang["npcgivetips"]);

	cocos2d::ui::Text* text0 = (cocos2d::ui::Text*)csbnode->getChildByName("text0");
	text0->setString(ResourceLang::map_lang["npcgivetext0"]);
	cocos2d::ui::Text* text1 = (cocos2d::ui::Text*)csbnode->getChildByName("text1");
	text1->setString(ResourceLang::map_lang["npcgivetext1"]);

	m_friendly = (cocos2d::ui::Text*)csbnode->getChildByName("friendly");
	std::string str = "0";
	m_friendly->setString(str);

	m_count = (cocos2d::ui::Text*)csbnode->getChildByName("count");

	cocos2d::ui::Button* subbtn = (cocos2d::ui::Button*)csbnode->getChildByName("subbtn");
	subbtn->addTouchEventListener(CC_CALLBACK_2(NpcgiveLayer::onSubBtnClick, this));
	subbtn->setTag(20000);
	cocos2d::ui::Button* addbtn = (cocos2d::ui::Button*)csbnode->getChildByName("addbtn");
	addbtn->addTouchEventListener(CC_CALLBACK_2(NpcgiveLayer::onAddBtnClick, this));
	addbtn->setTag(10000);

	cocos2d::ui::Button* cancelbtn = (cocos2d::ui::Button*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(NpcgiveLayer::onBtnClick, this));
	cancelbtn->setTag(1000);
	cocos2d::ui::Button* givebtn = (cocos2d::ui::Button*)csbnode->getChildByName("givebtn");
	givebtn->addTouchEventListener(CC_CALLBACK_2(NpcgiveLayer::onBtnClick, this));
	givebtn->setTag(1001);
	cocos2d::ui::ImageView* canceltext = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	canceltext->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	cocos2d::ui::ImageView* givetext = (cocos2d::ui::ImageView*)givebtn->getChildByName("text");
	givetext->loadTexture(ResourcePath::makeTextImgPath("npcgive_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	int startx[] = { 360, 270 ,210 };
	int offsetx[] = { 0, 180, 150 };
	int rewardsize = GlobalInstance::map_npcrelation[npcid].res.size();
	vec_rewards = GlobalInstance::map_npcrelation[npcid].res;

	for (int i = 0; i < 3; i++)
	{
		std::string str = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
		resbox->addTouchEventListener(CC_CALLBACK_2(NpcgiveLayer::onBtnClick, this));
		resbox->setTag(i);
		str = StringUtils::format("select_%d", i);
		cocos2d::ui::Widget* select = (cocos2d::ui::Widget*)csbnode->getChildByName(str);
		selectArr[i] = select;
		select->setVisible(false);
		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
		cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");

		if (i < rewardsize)
		{
			std::string resid = vec_rewards[i];
			resbox->setPositionX(startx[rewardsize - 1] + offsetx[rewardsize - 1] * i);
			std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

			select->setPosition(resbox->getPosition());

			namelbl->setString(GlobalInstance::map_AllResources[vec_rewards[i]].name);
		}
		else
		{
			resbox->setVisible(false);
			select->setVisible(false);
		}
	}

	selectArr[lastSelectIndex]->setVisible(true);
	str = StringUtils::format("%d/%d", 0, MyRes::getMyResCount(vec_rewards[lastSelectIndex]));
	if (vec_rewards[lastSelectIndex].compare("r006") == 0)
	{
		str = StringUtils::format("%d/%d", 0, GlobalInstance::getInstance()->getMySoliverCount().getValue());
	}
	else if (vec_rewards[lastSelectIndex].compare("r012") == 0)
	{
		str = StringUtils::format("%d/%d", 0, GlobalInstance::getInstance()->getMyCoinCount().getValue());
	}
	m_count->setString(str);

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

void NpcgiveLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://¹Ø±Õ
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1001://ÔùËÍ
		{
			if (lastIndexCount <= 0)
			{
				MovingLabel::show(ResourceLang::map_lang["selectnpcgive"]);
				return;
			}
			int friendly = GlobalInstance::map_AllResources[vec_rewards[lastSelectIndex]].friendly * lastIndexCount;
			GlobalInstance::map_myfriendly[m_npcid].friendly += friendly;
			if (GlobalInstance::map_myfriendly[m_npcid].friendly > GlobalInstance::map_npcrelation[m_npcid].friendmax)
			{
				GlobalInstance::map_myfriendly[m_npcid].friendly = GlobalInstance::map_npcrelation[m_npcid].friendmax;
			}
			NpcLayer* npclayer = (NpcLayer*)this->getParent();
			if (npclayer != NULL)
			{
				npclayer->loadFriendlyPro();
			}
			if (vec_rewards[lastSelectIndex].compare("r006") == 0)
			{
				DynamicValueInt dal;
				dal.setValue(lastIndexCount);
				GlobalInstance::getInstance()->costMySoliverCount(dal);
			}
			else if (vec_rewards[lastSelectIndex].compare("r012") == 0)
			{
				DynamicValueInt dal;
				dal.setValue(lastIndexCount);
				GlobalInstance::getInstance()->costMyCoinCount(dal);
			}
			else
			{
				MyRes::Use(vec_rewards[lastSelectIndex], lastIndexCount);
			}
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		}
		case 0:
		{
			lastSelectIndex = 0;
			selectArr[0]->setVisible(true);
			selectArr[1]->setVisible(false);
			selectArr[2]->setVisible(false);
			break;
		}
		case 1:
		{
			lastSelectIndex = 1;
			selectArr[1]->setVisible(true);
			selectArr[0]->setVisible(false);
			selectArr[2]->setVisible(false);
			break;
		}
		case 2:
		{
			lastSelectIndex = 2;
			selectArr[2]->setVisible(true);
			selectArr[0]->setVisible(false);
			selectArr[1]->setVisible(false);
			break;
		}
		default:
			break;
		}

		if (tag >= 0 && tag <= 2)
		{
			lastIndexCount = 0;
			updateCaryyCountLbl();
		}
	}
}

void NpcgiveLayer::onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(NpcgiveLayer::longTouchUpdate)))
			schedule(schedule_selector(NpcgiveLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		addRes();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void NpcgiveLayer::onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(NpcgiveLayer::longTouchUpdate)))
			schedule(schedule_selector(NpcgiveLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		subRes();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void NpcgiveLayer::updateCaryyCountLbl()
{
	std::string str = StringUtils::format("%d/%d", lastIndexCount, MyRes::getMyResCount(vec_rewards[lastSelectIndex]));
	if (vec_rewards[lastSelectIndex].compare("r006") == 0)
	{
		str = StringUtils::format("%d/%d", lastIndexCount, GlobalInstance::getInstance()->getMySoliverCount().getValue());
	}
	else if (vec_rewards[lastSelectIndex].compare("r012") == 0)
	{
		str = StringUtils::format("%d/%d", lastIndexCount, GlobalInstance::getInstance()->getMyCoinCount().getValue());
	}
	m_count->setString(str);

	int friendly = GlobalInstance::map_AllResources[vec_rewards[lastSelectIndex]].friendly * lastIndexCount;
	str = StringUtils::format("%d", friendly);
	m_friendly->setString(str);
}

void NpcgiveLayer::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		if (m_longTouchNode->getTag() == 10000)
			addRes();
		else if (m_longTouchNode->getTag() == 20000)
			subRes();
	}
}

void NpcgiveLayer::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(NpcgiveLayer::longTouchUpdate));
}

void NpcgiveLayer::addRes()
{
	bool isLack = false;
	if (vec_rewards[lastSelectIndex].compare("r006") == 0)
	{
		if (lastIndexCount >= GlobalInstance::getInstance()->getMySoliverCount().getValue())
		{
			isLack = true;
		}
	}
	else if (vec_rewards[lastSelectIndex].compare("r012") == 0)
	{
		if (lastIndexCount >= GlobalInstance::getInstance()->getMyCoinCount().getValue())
		{
			isLack = true;
		}
	}
	else
	{
		if (lastIndexCount >= MyRes::getMyResCount(vec_rewards[lastSelectIndex]))
		{
			isLack = true;
		}
	}
	if (isLack)
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["lacktext"].c_str(), GlobalInstance::map_AllResources[vec_rewards[lastSelectIndex]].name.c_str());
		MovingLabel::show(str);
		return;
	}
	lastIndexCount++;
	updateCaryyCountLbl();
}

void NpcgiveLayer::subRes()
{
	if (lastIndexCount <= 0)
		return;

	lastIndexCount--;
	updateCaryyCountLbl();
}