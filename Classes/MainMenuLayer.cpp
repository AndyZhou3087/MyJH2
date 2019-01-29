#include "MainMenuLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MaterialDescLayer.h"
#include "TaskTalkLayer.h"
#include "Quest.h"
#include "MessageLayer.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "SettingLayer.h"
#include "SoundManager.h"
#include "AchieveLayer.h"
#include "DataSave.h"
#include "HeadInfoLayer.h"
#include "NewPopLayer.h"
#include "ShopLayer.h"
#include "GiftContentLayer.h"
#include "NewGuideLayer.h"
#include "MarketLayer.h"
#include "ShopLayer.h"
#include "MovingLabel.h"
#include "TimeGiftLayer.h"
#include "HintBoxLayer.h"
#include "LoadingScene.h"

USING_NS_CC;

MainMenuLayer* g_MainMenuLayer = NULL;

MainMenuLayer::MainMenuLayer()
{
	isGetVipData = false;
}

MainMenuLayer::~MainMenuLayer()
{
	g_MainMenuLayer = NULL;
}


// on "init" you need to initialize your instance
bool MainMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	csbnode = CSLoader::createNode(ResourcePath::makePath("MainMenuLayer.csb"));
	this->addChild(csbnode);

	for (int i = 0; i < csbnode->getChildrenCount(); i++)
	{
		std::string name = csbnode->getChildren().at(i)->getName();
		cocos2d::ui::Widget* clickwidget = (cocos2d::ui::Widget*)csbnode->getChildByName(name);
		clickwidget->setTag(i);
		clickwidget->addTouchEventListener(CC_CALLBACK_2(MainMenuLayer::onClick, this));

		if (i >= R001BTN && i <= R005BTN)
		{
			reslbl[i - R001BTN] = (cocos2d::ui::Text*)clickwidget->getChildByName("countlbl");
			clickwidget->setVisible(false);
		}
		else if (i == ACHBTN)
		{
			achredpoint = (cocos2d::ui::Widget*)clickwidget->getChildByName("redpoint");
			achredpoint->setVisible(false);
		}
		else if (i == MAILBTN)
		{
			mailredpoint = (cocos2d::ui::Widget*)clickwidget->getChildByName("redpoint");
			mailredpoint->setVisible(false);
		}
		else if (i == SILVERBOX)
		{
			mysilverlbl = (cocos2d::ui::Text*)clickwidget->getChildByName("countlbl");
		}
		else if (i == COINBOX)
		{
			mycoinlbl = (cocos2d::ui::Text*)clickwidget->getChildByName("countlbl");
		}
		else if (i == TIMEGIFTBTN)
		{
			timegiftbtn = clickwidget;
			timegiftbtn->setVisible(false);
			timegiftlefttime = (cocos2d::ui::Text*)clickwidget->getChildByName("time");
		}
		else if (i >= TIMEGIFTBTN_0 && i <= TIMEGIFTBTN_2)
		{
			tgiftbtn[i - TIMEGIFTBTN_0] = clickwidget;
			tgiftbtn[i - TIMEGIFTBTN_0]->setVisible(false);
			tgiftname[i - TIMEGIFTBTN_0] = (cocos2d::ui::Text*)clickwidget->getChildByName("name");
		}

		if (i >= MOREBTN && i <= SHOPBTN)
		{
			cocos2d::ui::ImageView* textimg =  (cocos2d::ui::ImageView*)clickwidget->getChildByName("text");
			std::string textname = StringUtils::format("main_%s_text", name.c_str());
			textimg->loadTexture(ResourcePath::makeTextImgPath(textname, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}

		if (i >= VIP1BTN && i <= VIP2BTN)
		{
			cocos2d::ui::Text* text = (cocos2d::ui::Text*)clickwidget->getChildByName("text");
			int days = 0;
			if (i == VIP1BTN)
			{
				days = GlobalInstance::map_buyVipDays["vip1"];
				vipstrArr[0] = text;
			}
			else
			{
				days = GlobalInstance::map_buyVipDays["vip0"];
				vipstrArr[1] = text;
			}
			std::string textstr = StringUtils::format(ResourceLang::map_lang["daytext"].c_str(), days);
			text->setString(textstr);
			if (days == 0)
			{
				text->setVisible(false);
			}
		}
	}

	cocos2d::ui::Widget* headimgbox = (cocos2d::ui::Widget*)csbnode->getChildByName("headimgbox");
	head = (cocos2d::ui::ImageView*)headimgbox->getChildByName("headimg");
	std::string str = StringUtils::format("ui/h_%d_0.png", DataSave::getInstance()->getHeadId());
	head->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	head->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());

	morebtnnode = MoreBtnNode::create();
	morebtnnode->setPosition(Vec2(45, 90));
	this->addChild(morebtnnode);
	morebtnnode->setVisible(false);

	updateUI(0);
	this->schedule(schedule_selector(MainMenuLayer::updateUI), 1.0f);

	this->scheduleOnce(schedule_selector(MainMenuLayer::delayGetServerData), 1.2f);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		morebtnnode->runAction(Sequence::create(ScaleTo::create(0.2f, 0), Hide::create(), NULL));
		cocos2d::ui::Button* morebtn = (cocos2d::ui::Button*)csbnode->getChildByName("morebtn");
		if (morebtnnode->isVisible())
			morebtn->setBright(morebtnnode->isVisible());
		return true;
	};
	listener->setSwallowTouches(false);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void MainMenuLayer::delayGetServerData(float dt)
{
	isGetVipData = true;
	HttpDataSwap::init(this)->vipIsOn();
}

void MainMenuLayer::onFinish(int code)
{
	if (code == SUCCESS)
	{
		if (NewGuideLayer::checkifNewerGuide(14) || NewGuideLayer::checkifNewerGuide(15) || NewGuideLayer::checkifNewerGuide(22) || NewGuideLayer::checkifNewerGuide(THRIDGUIDESTEP+1) || NewGuideLayer::checkifNewerGuide(45)
			|| (NewGuideLayer::checkifNewerGuide(FIFTHGUIDESTEP) && NewGuideLayer::checkifNewerGuide(55) && Quest::isMainQuestFinish(1)) || (NewGuideLayer::checkifNewerGuide(63) && GlobalInstance::getInstance()->getMyHerosDeadCount() > 0)
			|| ((NewGuideLayer::checkifNewerGuide(66) || (NewGuideLayer::checkifNewerGuide(69) && !NewGuideLayer::checkifNewerGuide(67))) && GlobalInstance::getInstance()->getHerosChangeLevelCount() > 0)
			|| (NewGuideLayer::checkifNewerGuide(73) && GlobalInstance::getInstance()->getHerosLevelCount(15) > 0) || (!NewGuideLayer::checkifNewerGuide(75) && NewGuideLayer::checkifNewerGuide(77))
			|| (GlobalInstance::getInstance()->getUnlockHomehillCondition() && NewGuideLayer::checkifNewerGuide(15)) || (GlobalInstance::getInstance()->getLittleHerosPower(10) && NewGuideLayer::checkifNewerGuide(88)))
		{
			return;
		}

		if (isGetVipData)
		{
			if (this->getChildByName("GiftContentLayer") == NULL)
			{
				if (GlobalInstance::vec_buyVipIds.size() > 0)
				{

					for (unsigned int m = 0; m < GlobalInstance::vec_buyVipIds.size(); m++)
					{
						for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
						{
							if (GlobalInstance::vec_shopdata[i].icon.compare(GlobalInstance::vec_buyVipIds[m]) == 0)
							{
								GiftContentLayer* layer = GiftContentLayer::create(&GlobalInstance::vec_shopdata[i], i, 2);
								this->addChild(layer, 0, "GiftContentLayer");
								AnimationEffect::openAniEffect((Layer*)layer);
								break;
							}
						}
					}
				}
			}

			if (GlobalInstance::serverTimeGiftData.isopen)
			{
				timegiftbtn->setVisible(true);
				std::string timestr = StringUtils::format("%02d:%02d:%02d", GlobalInstance::serverTimeGiftData.lefttime / 3600, GlobalInstance::serverTimeGiftData.lefttime % 3600 / 60, GlobalInstance::serverTimeGiftData.lefttime % 3600 % 60);
				timegiftlefttime->setString(timestr);
			}

			HttpDataSwap::init(this)->getMessageList(0);

			if (GlobalInstance::punishment != 0)
			{
				doPunishment();
			}
		}
		else
		{
			if (GlobalInstance::vec_notice.size() > 0 && GlobalInstance::vec_notice[0].status < 1 && GlobalInstance::vec_notice[0].subtype <= 2)
			{
				NewPopLayer* unlock = NewPopLayer::create();
				this->addChild(unlock);
				AnimationEffect::openAniEffect((Layer*)unlock);
			}
		}
	}

	if (!GlobalInstance::serverTimeGiftData.isopen)
	{
		for (int i = 0; i < 3; i++)
		{
			tgiftbtn[i]->setVisible(true);

			std::string timegiftname = StringUtils::format("timegift%d", i);
			for (unsigned int n = 0; n < GlobalInstance::vec_shopdata.size(); n++)
			{
				if (GlobalInstance::vec_shopdata[n].icon.compare(timegiftname) == 0)
				{
					tgiftname[i]->setString(GlobalInstance::vec_shopdata[n].name);
					break;
				}
			}
		}
	}

	isGetVipData = false;

}

void MainMenuLayer::updateUI(float dt)
{
	std::string str;
	for (int i = 0; i < 5; i++)
	{
		std::string rid = StringUtils::format("r%03d", i + 1);

		str = StringUtils::format("%d", MyRes::getMyResCount(rid));
		reslbl[i]->setString(str);

	}
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	mycoinlbl->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	mysilverlbl->setString(str);

	bool showmessageredpoint = false;


	for (unsigned int i = 0; i < GlobalInstance::vec_notice.size(); i++)
	{
		int type = GlobalInstance::vec_notice[i].type;
		int s = GlobalInstance::vec_notice[i].status;
		if (type == 0 && s == 0)
		{
			showmessageredpoint = true;
			break;
		}
	}

	for (unsigned int i = 0; i < GlobalInstance::vec_messsages.size(); i++)
	{
		int type = GlobalInstance::vec_messsages[i].type;
		int s = GlobalInstance::vec_messsages[i].status;
		if (type != 0 && s < 2)
		{
			showmessageredpoint = true;
			break;
		}
	}

	mailredpoint->setVisible(showmessageredpoint);

	//月卡更新
	if (GlobalInstance::map_buyVipDays["vip1"] > 0)
	{
		vipstrArr[0]->setVisible(true);
	}
	std::string textstr = StringUtils::format(ResourceLang::map_lang["daytext"].c_str(), GlobalInstance::map_buyVipDays["vip1"]);
	vipstrArr[0]->setString(textstr);
	
	if (GlobalInstance::map_buyVipDays["vip0"] > 0)
	{
		vipstrArr[1]->setVisible(true);
	}
	textstr = StringUtils::format(ResourceLang::map_lang["daytext"].c_str(), GlobalInstance::map_buyVipDays["vip0"]);
	vipstrArr[1]->setString(textstr);
	
	//更新限时礼包时间
	if (GlobalInstance::serverTimeGiftData.isopen)
	{
		GlobalInstance::serverTimeGiftData.lefttime -= 1;
		std::string timestr = StringUtils::format("%02d:%02d:%02d", GlobalInstance::serverTimeGiftData.lefttime / 3600, GlobalInstance::serverTimeGiftData.lefttime % 3600 / 60, GlobalInstance::serverTimeGiftData.lefttime % 3600 % 60);
		timegiftlefttime->setString(timestr);
	}
	//成就更新
	int achcount = 0;
	for (unsigned int i = 0; i < GlobalInstance::vec_achievedata.size(); i++)
	{
		AchieveData data = GlobalInstance::vec_achievedata[i];
		if (data.state == DAILY_FINISHED)
		{
			achcount++;
			achredpoint->setVisible(true);
			break;
		}
	}
	if (achcount <= 0)
	{
		achredpoint->setVisible(false);
	}
}

void MainMenuLayer::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int menuType = clicknode->getTag();
	if ((menuType >= MOREBTN && menuType <= SHOPBTN) || menuType == TIMEGIFTBTN || (menuType >= TIMEGIFTBTN_0 && menuType <= TIMEGIFTBTN_2))
		CommonFuncs::BtnAction(pSender, type);
	else if (type == ui::Widget::TouchEventType::ENDED)
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (menuType)
		{
		case HEADBOX:
		{
			HeadInfoLayer* layer = HeadInfoLayer::create();
			this->addChild(layer, 0, "HeadInfoLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case R001BTN:
		case R002BTN:
		case R003BTN:
		case R004BTN:
		case R005BTN:
		{
			Layer * malayer = MaterialDescLayer::create(clicknode->getName());
			this->addChild(malayer, 0, "MaterialDescLayer");
		}
			break;
		case MOREBTN:
		{
			cocos2d::ui::Button* morebtn = (cocos2d::ui::Button*)pSender;

			if (morebtnnode->isVisible())
				morebtnnode->runAction(Sequence::create(ScaleTo::create(0.2f, 0), Hide::create(), NULL));
			else
				morebtnnode->runAction(Sequence::create(Show::create(), ScaleTo::create(0.2f, 1), NULL));

			morebtn->setBright(morebtnnode->isVisible());
		}
			break;
		case ACHBTN:
		{ 
			AchieveLayer* layer = AchieveLayer::create();
			g_mainScene->addChild(layer, 0, "achieveLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case RANKBTN:
			break;
		case MAILBTN:
		{
			MessageLayer* layer = MessageLayer::create();
			g_mainScene->addChild(layer, 0, "messagelayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case SHOPBTN:
		{
			ShopLayer* layer = ShopLayer::create();
			g_mainScene->addChild(layer, 0, "ShopLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case SILVERBOX:
		case ADDSILVERBTN:
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
		case COINBOX:
		case ADDCOINBTN:
		{
			ShopLayer* layer = ShopLayer::create();
			g_mainScene->addChild(layer, 0, "ShopLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case VIP1BTN:
			for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
			{
				if (GlobalInstance::vec_shopdata[i].icon.compare("vip1") == 0)
				{
					GiftContentLayer* layer = GiftContentLayer::create(&GlobalInstance::vec_shopdata[i], i);
					this->addChild(layer);
					AnimationEffect::openAniEffect((Layer*)layer);
					break;
				}
			}
			break;
		case VIP2BTN:
			for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
			{
				if (GlobalInstance::vec_shopdata[i].icon.compare("vip0") == 0)
				{
					GiftContentLayer* layer = GiftContentLayer::create(&GlobalInstance::vec_shopdata[i], i);
					this->addChild(layer);
					AnimationEffect::openAniEffect((Layer*)layer);
					break;
				}
			}
			break;
		case TIMEGIFTBTN:
		{
			std::string timegiftname = StringUtils::format("timegift%d", GlobalInstance::serverTimeGiftData.turn);
			for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
			{
				if (GlobalInstance::vec_shopdata[i].icon.compare(timegiftname) == 0)
				{
					TimeGiftLayer* layer = TimeGiftLayer::create(&GlobalInstance::vec_shopdata[i]);
					this->addChild(layer, 0, i);
					AnimationEffect::openAniEffect((Layer*)layer);
					break;
				}
			}
		}
		break;
		case TIMEGIFTBTN_0:
		case TIMEGIFTBTN_1:
		case TIMEGIFTBTN_2:
		{
			int turn = menuType - TIMEGIFTBTN_0;
			std::string timegiftname = StringUtils::format("timegift%d", turn);
			for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
			{
				if (GlobalInstance::vec_shopdata[i].icon.compare(timegiftname) == 0)
				{
					TimeGiftLayer* layer = TimeGiftLayer::create(&GlobalInstance::vec_shopdata[i]);
					this->addChild(layer, 0, i);
					AnimationEffect::openAniEffect((Layer*)layer);
					break;
				}
			}
		}
			break;
		default:
			break;
		}
	}
}

void MainMenuLayer::changeHead()
{
	std::string str = StringUtils::format("ui/h_%d_0.png", DataSave::getInstance()->getHeadId());
	head->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	head->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());
}

void MainMenuLayer::doPunishment()
{
	if (GlobalInstance::punishment > 0)
	{
		DynamicValueInt dint;
		dint.setValue(GlobalInstance::punishment - abs(GlobalInstance::getInstance()->getMyCoinCount().getValue()));
		GlobalInstance::getInstance()->addMyCoinCount(dint);

		HintBoxLayer* layer = HintBoxLayer::create(ResourceLang::map_lang["punishment1"], 10);
		Director::getInstance()->getRunningScene()->addChild(layer, 100000);
	}
	else if (GlobalInstance::punishment < 0)
	{
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LoadingScene::createScene()));
	}
}

void MainMenuLayer::onExit()
{
	Layer::onExit();
}
