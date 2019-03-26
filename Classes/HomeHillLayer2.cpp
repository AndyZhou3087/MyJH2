#include "HomeHillLayer2.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "ResCreator.h"
#include "Const.h"
#include "ConsumeResActionLayer.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "MainScene.h"
#include "HillResNode2.h"
#include "HH_ResActionLayer.h"

USING_NS_CC;

Vec2 homeResNodePos[] = {Vec2(580, 555), Vec2(575, 940), Vec2(220, 1050), Vec2(250, 715), Vec2(230, 100) };

HomeHillLayer2::HomeHillLayer2()
{
	m_isDraging = false;
	m_guideResNode = NULL;
}

HomeHillLayer2::~HomeHillLayer2()
{
	
}


HomeHillLayer2* HomeHillLayer2::create(Building* buidingData)
{
	HomeHillLayer2 *pRet = new(std::nothrow)HomeHillLayer2();
	if (pRet && pRet->init(buidingData))
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
bool HomeHillLayer2::init(Building* buidingData)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_buidingData = buidingData;

	csbnode = CSLoader::createNode(ResourcePath::makePath("homehillLayer2.csb"));
	this->addChild(csbnode);

	//资源滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("ScrollView");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(false);
	m_contentscroll->setSwallowTouches(false);

	int langtype = GlobalInstance::getInstance()->getLang();

	for (int i = 1; i <= 5; i++)
	{
		std::string name = StringUtils::format("r%d", i);
		Node* rnode = m_contentscroll->getChildByName(name);

		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)rnode->getChildByName("textbg")->getChildByName("text");
		name = StringUtils::format("hh_r%d_text", i);
		text->loadTexture(ResourcePath::makeTextImgPath(name, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		cocos2d::ui::Widget* click = (cocos2d::ui::Widget*)rnode->getChildByName("c");
		Node* select = rnode->getChildByName("s");
		select->setVisible(false);
		click->setUserData((void*)select);
		click->setTag(i);
		click->setSwallowTouches(false);
		click->addTouchEventListener(CC_CALLBACK_2(HomeHillLayer2::onBuildClick, this));

		HillResNode2* hillresnode = HillResNode2::create(i - 1);
		hillresnode->setPosition(homeResNodePos[i - 1]);
		m_contentscroll->addChild(hillresnode);

		if (i == 1)
			m_guideResNode = hillresnode;

	}
	//关闭按钮
	cocos2d::ui::Widget* backclick = (cocos2d::ui::Widget*)csbnode->getChildByName("backclick");
	backclick->setTag(1002);
	backclick->addTouchEventListener(CC_CALLBACK_2(HomeHillLayer2::onBtnClick, this));

	Node* idlenode = m_contentscroll->getChildByName("hh_fmcount_box");
	m_idlefarmercount = (cocos2d::ui::Text*)idlenode->getChildByName("count");

	actionbtn = (cocos2d::ui::Widget*)idlenode->getChildByName("actionbtn");
	actionbtn->setTag(1001);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(HomeHillLayer2::onBtnClick, this));

	updateUI(0);
	this->schedule(schedule_selector(HomeHillLayer2::updateUI), 0.1f);

	this->scheduleOnce(schedule_selector(HomeHillLayer2::delayShowNewerGuide), newguidetime);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		m_isDraging = false;
		m_startClickX = touch->getLocation().x;
		m_startClickY = touch->getLocation().y;
		return true;
	};

	listener->onTouchMoved = [=](Touch *touch, Event *event)
	{
		if (fabsf(m_startClickX - touch->getLocation().x) > CLICKOFFSETP || fabsf(m_startClickY - touch->getLocation().y) > CLICKOFFSETP)
		{
			if (g_NewGuideLayer == NULL)
			{
				m_isDraging = true;
			}
		}
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void HomeHillLayer2::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(14))
	{
		/*if (GlobalInstance::getInstance()->getUnlockHomehillCondition() && !NewGuideLayer::checkifNewerGuide(15))
		{*/
		if (!NewGuideLayer::checkifNewerGuide(15))
		{
			/*if (NewGuideLayer::checkifNewerGuide(16))
			{
				showNewerGuide(16);
			}
 			else */if (NewGuideLayer::checkifNewerGuide(18))
			{
				showNewerGuide(18);
			}
			else if (NewGuideLayer::checkifNewerGuide(20))
			{
				showNewerGuide(20);
			}
			else if (NewGuideLayer::checkifNewerGuide(21))
			{
				showNewerGuide(21);
			}
			else if (NewGuideLayer::checkifNewerGuide(82))
			{
				showNewerGuide(82);
			}
			else if (NewGuideLayer::checkifNewerGuide(83))
			{
				showNewerGuide(83);
			}
			else if (NewGuideLayer::checkifNewerGuide(84))
			{
				showNewerGuide(84);
			}
			else if (NewGuideLayer::checkifNewerGuide(85))
			{
				showNewerGuide(85);
			}
		}
	}
}

void HomeHillLayer2::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	/*
	if (step == 16)
	{
		nodes.push_back(csbnode->getChildByName("lvupbtn"));
	}
	else */if (step == 18)
	{
		nodes.push_back(actionbtn);
	}
	else if (step == 20 || step == 21 || step == 82 || step == 83 || step == 84)
	{
		if (m_guideResNode != NULL)
		{
			nodes.push_back(m_guideResNode->getChildByName("csbnode")->getChildByName("addclick"));
		}
	}
	else if (step == 85)
	{
		nodes.push_back(csbnode->getChildByName("backclick"));
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void HomeHillLayer2::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		if (tag == 1002)
		{
			AnimationEffect::closeAniEffect(this);
		}
		else
		{
			HH_ResActionLayer* layer = HH_ResActionLayer::create(NULL, HH_EMPLOYFARMER);
			this->getParent()->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
	}
}

void HomeHillLayer2::onBuildClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	Node* snode = (Node*)clicknode->getUserData();
	int tag = clicknode->getTag();
	switch (type)
	{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
		{
			snode->setVisible(true);
		}
		break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
			break;
		case cocos2d::ui::Widget::TouchEventType::ENDED:
		{
			snode->setVisible(false);
			if (m_isDraging)
				return;
		}
		case cocos2d::ui::Widget::TouchEventType::CANCELED:
		{
			snode->setVisible(false);

			break;
		}
		default:
			break;
	}
}

void HomeHillLayer2::updateUI(float dt)
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getTotalFarmers() - GlobalInstance::getInstance()->getWorkingFarmerCount());
	m_idlefarmercount->setString(str);
}

void HomeHillLayer2::onExit()
{
	Layer::onExit();
}
