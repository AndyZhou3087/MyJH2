#include "RandHeroLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "Const.h"
#include "MovingLabel.h"
#include "Quest.h"
#include "HintBoxLayer.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "NewGuideLayer.h"
#include "MainScene.h"
#include "MarketLayer.h"
#include "ShopLayer.h"
#include "DataSave.h"
#include "MyRes.h"
#include "HeroAttrLayer.h"
#include "InnRoomLayer.h"
#include "BuyCoinLayer.h"

USING_NS_CC;

RandHeroLayer::RandHeroLayer()
{
	GlobalInstance::isNewHeroRefresh = false;
	refreshHeroCoin.setValue(COINREFRESH_HERO_NUM);
	refreshHeroSilver.setValue(SILVERREFRESH_HERO_NUM);

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
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	csbnode = CSLoader::createNode(ResourcePath::makePath("randHeroLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("innroomtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	cocos2d::ui::Text* lvUIText = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", Building::map_buildingDatas["6innroom"]->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIText->setString(str);


	std::string btnname[] = { "srefreshbtn", "crefreshbtn", "usecardbtn", "silverbox", "addbtn1", "coinbox", "addbtn2","closebtn"};//与BTNTYPE对应
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
		else if (tag == BTN_USECARD)
		{
			//使用卷轴按钮文字
			cocos2d::ui::ImageView* usecardbtntxt = (cocos2d::ui::ImageView*)btn->getChildByName("text");
			usecardbtntxt->loadTexture(ResourcePath::makeTextImgPath("usecard_Text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
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
	std::string lblstr = StringUtils::format("%d", refreshHeroSilver.getValue() + GlobalInstance::getInstance()->getSilverRefHeroCount() * 100);
	refreshsilverlbl->setString(lblstr);
	refreshcoinlbl = (cocos2d::ui::Text*)csbnode->getChildByName("cnumbl");
	lblstr = StringUtils::format("%d", COINREFRESH_HERO_NUM);
	refreshcoinlbl->setString(lblstr);

	herocardcountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("cardnumbl");
	lblstr = StringUtils::format("%d", MyRes::getMyResCount("u001"));
	herocardcountlbl->setString(lblstr);

	m_timebar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("timebar");
	m_timebar->setPercent(100);

	m_timelbl = (cocos2d::ui::Text*)csbnode->getChildByName("timelbl");

	if (GlobalInstance::vec_rand3Heros.size() <= 0)
		create3RandHero();

	for (int i = 0; i < 3; i++)
	{
		heronode[i] = RandHeroNode::create();
		heronode[i]->setPosition(140+i*220, 730);
		heronode[i]->setData(GlobalInstance::vec_rand3Heros[i], false);
		this->addChild(heronode[i], 0, i);
	}

	updateUI(0);
	this->schedule(schedule_selector(RandHeroLayer::updateUI), 1.0f);

	this->scheduleOnce(schedule_selector(RandHeroLayer::delayShowNewerGuide), newguidetime);

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

void RandHeroLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(23))
	{
		if (NewGuideLayer::checkifNewerGuide(THRIDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(14))
			{
				cocos2d::ui::Button* colsebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
				colsebtn->setEnabled(false);
				showNewerGuide(14);
			}
			else if (NewGuideLayer::checkifNewerGuide(24))
			{
				cocos2d::ui::Button* colsebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
				colsebtn->setEnabled(true);
				showNewerGuide(24);
			}
			else if (NewGuideLayer::checkifNewerGuide(26))
			{
				showNewerGuide(26);
			}
			else if (NewGuideLayer::checkifNewerGuide(28))
			{
				showNewerGuide(28);
			}
			else if (NewGuideLayer::checkifNewerGuide(30))
			{
				showNewerGuide(30);
			}
		}
	}
}

void RandHeroLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 14)
	{
		nodes.push_back(csbnode->getChildByName("srefreshbtn"));
	}
	if (step == 24)
	{
		nodes.push_back(heronode[0]->getChildByName("csbnode")->getChildByName("randheadbox"));
	}
	else if (step == 26)
	{
		nodes.push_back(heronode[1]->getChildByName("csbnode")->getChildByName("randheadbox"));
	}
	else if (step == 28)
	{
		nodes.push_back(heronode[2]->getChildByName("csbnode")->getChildByName("randheadbox"));
	}
	else if (step == 30)
	{
		nodes.push_back(csbnode->getChildByName("closebtn"));
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void RandHeroLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	if (tag != BTN_ADD_COINBOX && tag != BTN_ADD_SILVERBOX)
		CommonFuncs::BtnAction(pSender, type);
	else if (type == ui::Widget::TouchEventType::ENDED)
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case BTN_S_REFRESH://银子刷新
			if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= refreshHeroSilver.getValue() + GlobalInstance::getInstance()->getSilverRefHeroCount() * 100)
			{
				if (checkIsTopPotentail(1) < 0)
					refresh3Hero(1);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomoresilver"]);
			}
			break;
		case BTN_C_REFRESH://元宝刷新
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= refreshHeroCoin.getValue())
			{
				if (checkIsTopPotentail(2) < 0)
					refresh3Hero(2);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
				Layer* layer = BuyCoinLayer::create(refreshHeroCoin.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
			}
			break;
		case BTN_USECARD:
		{
			if (MyRes::getMyResCount("u001") > 0)
			{
				if (checkIsTopPotentail(3) < 0)
					refresh3Hero(3);
				//Hero* myhero = new Hero();
				//myhero->generate();
				//myhero->setPotential(3);
				//myhero->setState(HS_OWNED);

				//GlobalInstance::vec_myHeros.push_back(myhero);
				//GlobalInstance::getInstance()->saveMyHeros();
				//MyRes::Use("u001");
				//Layer* layer = HeroAttrLayer::create(myhero, 0, 2);
				//layer->setName("heroattrlayer");
				//g_mainScene->addChild(layer, 0, GlobalInstance::vec_myHeros.size() - 1);
				//AnimationEffect::openAniEffect((Layer*)layer);

				//std::string str = StringUtils::format("%d", MyRes::getMyResCount("u001"));
				//herocardcountlbl->setString(str);

				//InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
				//innroomLayer->refreshMyHerosUi();
			}
			else
			{
				std::string str = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), GlobalInstance::map_AllResources["u001"].name.c_str());
				str.append(ResourceLang::map_lang["herocardhint"]);
				MovingLabel::show(str);
			}
		}
			break;
		case BTN_ADD_SILVERBOX://增加银子
		case BTN_ADD_SILVER://增加银子
		{
			if (g_mainScene != NULL && g_mainScene->buildingIsClickOn(6))
			{
				MovingLabel::show(ResourceLang::map_lang["unlockmain_6"]);
			}
			else
			{
				MarketLayer* layer = MarketLayer::create(Building::map_buildingDatas["5market"]);
				g_mainScene->addChild(layer, 0, "5market");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
		}
			break;
		case BTN_ADD_COINBOX://增加元宝
		case BTN_ADD_COIN://增加元宝
		{
			ShopLayer* layer = ShopLayer::create();
			g_mainScene->addChild(layer, 0, "ShopLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case BTN_CLOSE://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}
	}
}

int RandHeroLayer::checkIsTopPotentail(int buytype)
{
	int topPotential = -1;
	for (unsigned int i = 0; i < GlobalInstance::vec_rand3Heros.size(); i++)
	{
		if (GlobalInstance::vec_rand3Heros[i]->getState() != HS_OWNED)
		{
			int p = GlobalInstance::vec_rand3Heros[i]->getPotential();
			if (p >= 2)
				topPotential = p;
		}
	}
	if (topPotential >= 2)
	{
		std::string potentialstr = StringUtils::format("potential_%d", topPotential);
		std::string hintstr = StringUtils::format(ResourceLang::map_lang["hintrefresh"].c_str(), ResourceLang::map_lang[potentialstr].c_str());
		HintBoxLayer* hint = HintBoxLayer::create(hintstr, 1);
		this->addChild(hint, 0, buytype);
	}
	return topPotential;
}

void RandHeroLayer::refresh3Hero(int i)
{
	if (i == 1)
	{
		DynamicValueInt dval;
		dval.setValue(refreshHeroSilver.getValue() + GlobalInstance::getInstance()->getSilverRefHeroCount() * 100);
		GlobalInstance::getInstance()->costMySoliverCount(dval);

		GlobalInstance::getInstance()->setSilverRefHeroCount(GlobalInstance::getInstance()->getSilverRefHeroCount() + 1);

		std::string str = StringUtils::format("%d", refreshHeroSilver.getValue() + GlobalInstance::getInstance()->getSilverRefHeroCount() * 100);
		refreshsilverlbl->setString(str);
	}
	else if (i == 2)
	{
		DynamicValueInt dval;
		dval.setValue(refreshHeroCoin.getValue());
		GlobalInstance::getInstance()->costMyCoinCount(dval);
	}
	else if (i == 3)
	{
		MyRes::Use("u001");
		std::string str = StringUtils::format("%d", MyRes::getMyResCount("u001"));
		herocardcountlbl->setString(str);
	}

	bool israndanim = false;
	if (i > 0)
		israndanim = true;
	create3RandHero(i);
	for (int m = 0; m < 3; m++)
	{
		heronode[m]->setData(GlobalInstance::vec_rand3Heros[m], israndanim);
	}
	//记录刷新次数
	Quest::setDailyTask(FRESH_PUBENLIST, 1);
	Quest::setAchieveTypeCount(FRESH_PUBENLIST, 1);
}

void RandHeroLayer::updateUI(float dt)
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	mysilverlbl->setString(str);

	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	mycoinlbl->setString(str);

	str = StringUtils::format("%d", refreshHeroSilver.getValue() + GlobalInstance::getInstance()->getSilverRefHeroCount() * 100);
	refreshsilverlbl->setString(str);

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
			heronode[i]->setData(GlobalInstance::vec_rand3Heros[i], false);
		}
	}
	else
	{
		lefttime = HERO_RESETTIME - pasttime;
	}
	std::string timestr = StringUtils::format("%02d:%02d:%02d", lefttime / 3600, lefttime % 3600 / 60, lefttime % 3600 % 60);
	m_timelbl->setString(timestr);
	m_timebar->setPercent(lefttime*100/ HERO_RESETTIME);

	int zerotime = GlobalInstance::servertime;
	if (zerotime - GlobalInstance::getInstance()->getResetSilverRefHeroCountTime() >= TWENTYFOURHOURSTOSEC)
	{
		GlobalInstance::getInstance()->setResetSilverRefHeroCountTime(zerotime - zerotime % TWENTYFOURHOURSTOSEC);
		GlobalInstance::getInstance()->setSilverRefHeroCount(0);
		GlobalInstance::isNewHeroRefresh = true;
	}

}

void RandHeroLayer::create3RandHero(int tool)
{
	bool isfirsttime = GlobalInstance::vec_rand3Heros.size() == 0 ? true : false;
	delete3RandHero();
	for (int i = 0; i < 3; i++)
	{
		Hero* randhero = new Hero();
		randhero->generate();
		//元宝刷新
		if (tool == 2)
		{
			randhero->setPotential(GlobalInstance::getInstance()->generateHeroPotentialByCoin());
		}
		else if (tool == 1)//第一次新手引导刷新
		{
			if (!NewGuideLayer::checkifNewerGuide(14) && NewGuideLayer::checkifNewerGuide(24) && i == 0)
			{
				randhero->setVocation(0);
				randhero->setPotential(3);
			}
			else if (isfirsttime)
			{
				int p = i == 2 ? 1 : i;
				randhero->setPotential(p);
			}
		}
		GlobalInstance::vec_rand3Heros.push_back(randhero);
	}
	
	if (tool == 3)//角色券刷新
	{
		int r = GlobalInstance::getInstance()->createRandomNum(3);
		GlobalInstance::vec_rand3Heros[r]->setPotential(3);
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


