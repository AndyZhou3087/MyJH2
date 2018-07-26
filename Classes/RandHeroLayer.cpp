#include "RandHeroLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "Const.h"
#include "MovingLabel.h"
#include "Quest.h"
#include "HintBoxLayer.h"

USING_NS_CC;

#define SILVERREFRESH_NUM 100
#define COINREFRESH_NUM 10
RandHeroLayer::RandHeroLayer()
{
	isTopPotential = false;
}

RandHeroLayer::~RandHeroLayer()
{
	
}


RandHeroLayer* RandHeroLayer::create()
{
	RandHeroLayer *pRet = new(std::nothrow)RandHeroLayer();
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
bool RandHeroLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("randHeroLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("innroomtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	cocos2d::ui::Text* lvUIText = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", Building::map_buildingDatas["6innroom"]->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIText->setString(str);


	std::string btnname[] = { "srefreshbtn", "crefreshbtn", "silverbox", "addbtn1", "coinbox", "addbtn2","closebtn"};//与BTNTYPE对应
	for (int i = 0; i < sizeof(btnname) / sizeof(btnname[0]); i++)
	{
		int tag = i + BTN_S_REFRESH;
		cocos2d::ui::Widget* btn = (cocos2d::ui::Widget*)csbnode->getChildByName(btnname[i]);
		btn->setTag(tag);
		btn->addTouchEventListener(CC_CALLBACK_2(RandHeroLayer::onBtnClick, this));

		if (tag == BTN_S_REFRESH)
		{
			//银子刷新按钮文字
			cocos2d::ui::ImageView* srefreshbtntxt = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			srefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("srefresh_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else if (tag == BTN_C_REFRESH)
		{
			//元宝刷新按钮文字
			cocos2d::ui::ImageView* crefreshbtntxt = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			crefreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("crefresh_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else if (tag == BTN_ADD_SILVERBOX)
		{
			mysilverlbl = (cocos2d::ui::Text*)btn->getChildByName("countlbl");
		}
		else if (tag == BTN_ADD_COINBOX)
		{
			mycoinlbl = (cocos2d::ui::Text*)btn->getChildByName("countlbl");
		}
	}


	refreshsilverlbl = (cocos2d::ui::Text*)csbnode->getChildByName("snumbl");
	std::string lblstr = StringUtils::format("%d", SILVERREFRESH_NUM);
	refreshsilverlbl->setString(lblstr);
	refreshcoinlbl = (cocos2d::ui::Text*)csbnode->getChildByName("cnumbl");
	lblstr = StringUtils::format("%d", COINREFRESH_NUM);
	refreshcoinlbl->setString(lblstr);

	m_timebar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("timebar");
	m_timebar->setPercent(100);

	m_timelbl = (cocos2d::ui::Text*)csbnode->getChildByName("timelbl");

	if (GlobalInstance::vec_rand3Heros.size() <= 0)
		create3RandHero();

	for (int i = 0; i < 3; i++)
	{
		heronode[i] = RandHeroNode::create();
		heronode[i]->setPosition(140+i*220, 730);
		heronode[i]->setData(GlobalInstance::vec_rand3Heros[i]);
		this->addChild(heronode[i], 0, i);
	}

	updateUI(0);
	this->schedule(schedule_selector(RandHeroLayer::updateUI), 1.0f);

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

void RandHeroLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	if (tag != BTN_ADD_COINBOX && tag != BTN_ADD_SILVERBOX)
		CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case BTN_S_REFRESH://银子刷新
			if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= SILVERREFRESH_NUM)
			{
				if (isTopPotential)
				{
					HintBoxLayer* hint = HintBoxLayer::create(ResourceLang::map_lang["hintrefresh"],1);
					this->addChild(hint);
					return;
				}
				
				refresh3Hero(1);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomoresilver"]);
			}
			break;
		case BTN_C_REFRESH://元宝刷新
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= COINREFRESH_NUM)
			{
				if (isTopPotential)
				{
					HintBoxLayer* hint = HintBoxLayer::create(ResourceLang::map_lang["hintrefresh"],2);
					this->addChild(hint);
					return;
				}

				refresh3Hero(2);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
			}
			break;
		case BTN_ADD_SILVERBOX://增加银子
		case BTN_ADD_SILVER://增加银子
			break;
		case BTN_ADD_COINBOX://增加元宝
		case BTN_ADD_COIN://增加元宝
			break;
		case BTN_CLOSE://关闭
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void RandHeroLayer::refresh3Hero(int i)
{
	if (i == 1)
	{
		DynamicValueInt dval;
		dval.setValue(SILVERREFRESH_NUM);
		GlobalInstance::getInstance()->costMySoliverCount(dval);
		create3RandHero();
		for (int i = 0; i < 3; i++)
		{
			heronode[i]->setData(GlobalInstance::vec_rand3Heros[i]);
		}
		//记录刷新次数
		Quest::setDailyTask(FRESH_PUBENLIST, 1);
	}
	else
	{
		DynamicValueInt dval;
		dval.setValue(COINREFRESH_NUM);
		GlobalInstance::getInstance()->costMyCoinCount(dval);
		create3RandHero();
		for (int i = 0; i < 3; i++)
		{
			heronode[i]->setData(GlobalInstance::vec_rand3Heros[i]);
		}
		//记录刷新次数
		Quest::setDailyTask(FRESH_PUBENLIST, 1);
	}
}

void RandHeroLayer::updateUI(float dt)
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	mysilverlbl->setString(str);

	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	mycoinlbl->setString(str);

	int lefttime = 0;
	int refreshtime = GlobalInstance::getInstance()->getRefreshHeroTime();
	int pasttime = GlobalInstance::servertime - refreshtime;
	if (pasttime >= HERO_RESETTIME)
	{
		int t = GlobalInstance::servertime % HERO_RESETTIME;

		refreshtime = GlobalInstance::servertime - t;
		GlobalInstance::getInstance()->saveRefreshHeroTime(refreshtime);

		lefttime = HERO_RESETTIME - t;
		create3RandHero();
		for (int i = 0; i < 3; i++)
		{
			heronode[i]->setData(GlobalInstance::vec_rand3Heros[i]);
		}
	}
	else
	{
		lefttime = HERO_RESETTIME - pasttime;
	}
	std::string timestr = StringUtils::format("%02d:%02d:%02d", lefttime / 3600, lefttime % 3600 / 60, lefttime % 3600 % 60);
	m_timelbl->setString(timestr);
	m_timebar->setPercent(lefttime*100/ HERO_RESETTIME);
}

void RandHeroLayer::create3RandHero()
{
	isTopPotential = false;
	delete3RandHero();
	for (int i = 0; i < 3; i++)
	{
		Hero* randhero = new Hero();
		randhero->generate();
		GlobalInstance::vec_rand3Heros.push_back(randhero);
		if (randhero->getPotential() > 2)
		{
			isTopPotential = true;
		}
	}
	GlobalInstance::getInstance()->saveRand3Heros();
}

void RandHeroLayer::delete3RandHero()
{
	int randsize = GlobalInstance::vec_rand3Heros.size();
	for (int i = 0; i < randsize; i++)
	{
		delete GlobalInstance::vec_rand3Heros[i];
		GlobalInstance::vec_rand3Heros[i] = NULL;
	}
	GlobalInstance::vec_rand3Heros.clear();
}

void RandHeroLayer::onExit()
{
	Layer::onExit();
}


