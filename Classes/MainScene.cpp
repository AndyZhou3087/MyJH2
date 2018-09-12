#include "MainScene.h"
#include "Resource.h"
#include "MainMenuLayer.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "InnRoomLayer.h"
#include "HomeHillLayer.h"
#include "Const.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "OutTownLayer.h"
#include "StoreHouseLayer.h"
#include "SmithyLayer.h"
#include "MarketLayer.h"
#include "TaskLayer.h"
#include "HospitalLayer.h"
#include "TrainLayer.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "Quest.h"

USING_NS_CC;
MainScene* g_mainScene = NULL;
MainScene::MainScene()
{
	m_isDraging = false;
	costFoodsT = 0;
	GlobalInstance::myOutMapCarry = 0;
	isPlayNewHeroAnim = false;
}

MainScene::~MainScene()
{
	g_mainScene = NULL;
}

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	g_mainScene = MainScene::create();

    // add layer as a child to scene
	scene->addChild(g_mainScene);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("MainLayer.csb"));
	this->addChild(csbnode);

	Node* menunode = MainMenuLayer::create();
	this->addChild(menunode);

	scroll_3 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_3");
	scroll_3->setScrollBarEnabled(false);
	scroll_3->jumpToPercentHorizontal(32);
	scroll_3->addEventListener(CC_CALLBACK_2(MainScene::srollviewlistenEvent, this));
	scroll_3->setSwallowTouches(false);

	scroll_2 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_2");
	scroll_2->setScrollBarEnabled(false);
	scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
	scroll_2->setSwallowTouches(false);
	//scroll_2->addEventListener(CC_CALLBACK_2(MainScene::srollviewlistenEvent, this));

	scroll_1 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_1");
	scroll_1->setScrollBarEnabled(false);
	scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
	scroll_1->setSwallowTouches(false);
	//scroll_1->addEventListener(CC_CALLBACK_2(MainScene::srollviewlistenEvent, this));

	std::map<std::string, Building*>::iterator it;
	int i = 1;
	for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
	{
		std::string buidingNomalName;
		std::string buidingSelectName;
		std::string bulidingname;
		std::string bulidclipname;
		cocos2d::ui::ImageView* buildingNomal;
		cocos2d::ui::ImageView* buildingSelect;
		cocos2d::ui::ImageView* buildnametext;
		cocos2d::ui::Widget* bulidinclipnode;

		buidingNomalName = StringUtils::format("main_%02d_n", i);//可点击
		buidingSelectName = StringUtils::format("main_%02d_s", i);//选中

		bulidingname = StringUtils::format("main_%02d_t", i);//文字
		bulidclipname = StringUtils::format("main_%02d_c", i);//遮罩

		Node* buildParent;
		if (i <= 5)
		{
			buildParent = scroll_3;
		}
		else if (i <= 8)
		{
			buildParent = scroll_2;
		}
		else
		{
			buildParent = scroll_1;
		}
		buildingNomal = (cocos2d::ui::ImageView*)buildParent->getChildByName(buidingNomalName);

		buildingSelect = (cocos2d::ui::ImageView*)buildParent->getChildByName(buidingSelectName);

		buildnametext = (cocos2d::ui::ImageView*)buildParent->getChildByName(bulidingname);

		bulidinclipnode = (cocos2d::ui::Widget*)buildParent->getChildByName(bulidclipname);

		buildnametext->loadTexture(ResourcePath::makeTextImgPath(bulidingname, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		buildingNomal->setSwallowTouches(false);
		buildingNomal->setUserData((void*)buildingSelect);
		buildingNomal->addTouchEventListener(CC_CALLBACK_2(MainScene::onBuildingClick, this));
		if ((NewGuideLayer::checkifNewerGuide(63) && i == 2) || (i == 6 && NewGuideLayer::checkifNewerGuide(66)))//医馆不可点,市场不可点
		{
			buildingNomal->setTouchEnabled(false);
			bulidinclipnode->setVisible(true);
			buildnametext->setVisible(false);
		}

		if (i == 4)
		{
			std::string buidingNomalName_1 = StringUtils::format("main_%02d_n_1", i);
			cocos2d::ui::ImageView* buildingNomal_1 = (cocos2d::ui::ImageView*)buildParent->getChildByName(buidingNomalName_1);
			buildingNomal_1->setSwallowTouches(false);
			buildingNomal_1->setUserData((void*)buildingSelect);
			buildingNomal_1->addTouchEventListener(CC_CALLBACK_2(MainScene::onBuildingClick, this));
		}
		else if (i == 10)
		{
			updateTaskIcon();	
		}

		buildingSelect->setVisible(false);
		buildingSelect->setUserData((void*)it->first.c_str());
		i++;
	}

	MyRes::putMyPackagesToStorage();
	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_HOME);

	this->scheduleOnce(schedule_selector(MainScene::delayShowNewerGuide), 0.1f);

	maincityhintbox = scroll_2->getChildByName("maincityhintbox");
	maincityhintbox->setScale(0);
	maincityhintbox->setVisible(false);
	cocos2d::ui::Text* hinttext = (cocos2d::ui::Text*)maincityhintbox->getChildByName("text");
	hinttext->setString(ResourceLang::map_lang["newherohint"]);

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
			m_isDraging = true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MainScene::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(14))
	{
		if (NewGuideLayer::checkifNewerGuide(63) && GlobalInstance::getInstance()->getMyHerosDeadCount() > 0)
		{
			showNewerGuide(63);
		}
		else if (NewGuideLayer::checkifNewerGuide(66) && GlobalInstance::getInstance()->getHerosChangeLevelCount() > 0)
		{
			showNewerGuide(66);
		}
		/*else if (NewGuideLayer::checkifNewerGuide(SECONDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(15))
			{
				showNewerGuide(15);
			}
		}*/
		else if (NewGuideLayer::checkifNewerGuide(THRIDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(22))
			{
				showNewerGuide(22);
			}
		}
		else if (NewGuideLayer::checkifNewerGuide(MIDELEGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(40))
			{
				showNewerGuide(40);
			}
		}
		else if (NewGuideLayer::checkifNewerGuide(FOURTHGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(45))
			{
				showNewerGuide(45);
			}
		}
		else if (NewGuideLayer::checkifNewerGuide(FIFTHGUIDESTEP) && Quest::isMainQuestFinish(1))
		{
			if (NewGuideLayer::checkifNewerGuide(55))
			{
				showNewerGuide(55);
			}
		}
	}
}

void MainScene::showNewerGuide(int step)
{
	scroll_3->setEnabled(false);
	scroll_2->setEnabled(false);
	scroll_1->setEnabled(false);
	std::vector<Node*> nodes;
	if (step == 15)
	{
		scroll_3->jumpToPercentHorizontal(95);
		//scroll_2->jumpToPercentHorizontal(95);
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		//scroll_3->jumpToPercentHorizontal(95);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_08_n");
		nodes.push_back(node);
	}
	else if (step == 22)
	{

		//scroll_1->jumpToPercentHorizontal(50);
		//scroll_2->jumpToPercentHorizontal(50);
		scroll_3->jumpToPercentHorizontal(50);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_07_n");
		nodes.push_back(node);
	}
	else if (step == 40 || step == 55)
	{
		//scroll_1->jumpToPercentHorizontal(32);
		//scroll_2->jumpToPercentHorizontal(32);
		scroll_3->jumpToPercentHorizontal(32);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_1->getChildByName("main_10_n");
		nodes.push_back(node);
	}
	else if (step == 45)
	{
		scroll_3->setEnabled(true);
		scroll_2->setEnabled(true);
		scroll_1->setEnabled(true);
		//scroll_1->jumpToPercentHorizontal(80);
		//scroll_2->jumpToPercentHorizontal(80);
		scroll_3->jumpToPercentHorizontal(80);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_01_n");
		nodes.push_back(node);
	}
	else if (step == 63)
	{
		scroll_3->setEnabled(true);
		scroll_2->setEnabled(true);
		scroll_1->setEnabled(true);

		scroll_3->jumpToPercentHorizontal(100);

		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_02_n");
		node->setTouchEnabled(true);
		cocos2d::ui::Widget* cnode = (cocos2d::ui::Widget*)scroll_3->getChildByName("main_02_c");
		cnode->setVisible(false);
		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_02_t");
		text->setVisible(true);
		nodes.push_back(node);
	}
	else if (step == 66)
	{
		scroll_3->jumpToPercentHorizontal(100);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_06_n");
		node->setTouchEnabled(true);
		cocos2d::ui::Widget* cnode = (cocos2d::ui::Widget*)scroll_2->getChildByName("main_06_c");
		cnode->setVisible(false);
		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_06_t");
		text->setVisible(true);
		nodes.push_back(node);
	}
	showNewerGuideNode(step, nodes);
}

void MainScene::showNewerGuideNode(int step, std::vector<Node*> nodes)
{
	if (NewGuideLayer::checkifNewerGuide(step))
	{
		if (g_NewGuideLayer == NULL)
		{
			g_NewGuideLayer = NewGuideLayer::create(step, nodes);
			this->addChild(g_NewGuideLayer, 10);
		}
	}
}

void MainScene::setScrollGliding()
{
	scroll_3->setEnabled(true);
	scroll_2->setEnabled(true);
	scroll_1->setEnabled(true);
}

void MainScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	this->scheduleOnce(schedule_selector(MainScene::delayGetServerTime), 0.1f);
}

void MainScene::srollviewlistenEvent(Ref* ref, ui::ScrollView::EventType eventType)
{
	//if (g_NewGuideLayer != NULL)
	//{
	//	return;
	//}
	//Vec2 pos = scroll_3->getInnerContainerPosition();
	////log("scoll posx:%f, posy:%f", pos.x, pos.y);
	////log("scoll eventType = %d", eventType);

	//switch (eventType) 
	//{
	//	//最外层滑动时，带动后两层滑动，可修改时间调整效果
	//	case ui::ScrollView::EventType::CONTAINER_MOVED:
	//	{
	//		//将引擎中的startAutoScrollToDestination 修改为pulic
	//		scroll_2->startAutoScrollToDestination(pos, 0.016f, true);
	//		scroll_1->startAutoScrollToDestination(pos, 0.03f, true);
	//		break;
	//	}
	//	default:
	//		break;
	//}
}

void MainScene::onBuildingClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::ImageView* clicknode = (cocos2d::ui::ImageView*)pSender;
	Node* snode = (Node*)clicknode->getUserData();
	std::string buildname = (char*)snode->getUserData();
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

			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

			Layer* layer = NULL;

			if (buildname.compare("6innroom") == 0)
			{
				layer = InnRoomLayer::create(Building::map_buildingDatas[buildname]);
			}
			else if (buildname.compare("7homehill") == 0)
			{
				layer = HomeHillLayer::create(Building::map_buildingDatas[buildname]);
			}
			else if (buildname.compare("0outtown") == 0)
			{
				layer = OutTownLayer::create();
			}
			else if (buildname.compare("3storehouse") == 0)
			{
				layer = StoreHouseLayer::create();
			}
			else if (buildname.compare("2smithy") == 0)
			{
				layer = SmithyLayer::create(Building::map_buildingDatas[buildname]);
			}
			else if (buildname.compare("5market") == 0)
			{
				layer = MarketLayer::create(Building::map_buildingDatas[buildname]);
			}
			else if (buildname.compare("9assemblyhall") == 0)
			{
				layer = TaskLayer::create();
			}
			else if (buildname.compare("1hospital") == 0)
			{
				layer = HospitalLayer::create();
			}
			else if (buildname.compare("4trainigroom") == 0)
			{
				layer = TrainLayer::create(Building::map_buildingDatas[buildname]);
			}

			if (layer != NULL)
			{
				this->addChild(layer, 0, buildname);
				AnimationEffect::openAniEffect(layer);
			}

			break;
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

void MainScene::onExit()
{
	Layer::onExit();
}

void MainScene::onFinish(int code)
{
	if (code == SUCCESS)
	{
		if (GlobalInstance::getInstance()->getRefreshHeroTime() == 0)
		{
			GlobalInstance::getInstance()->saveRefreshHeroTime(GlobalInstance::servertime);
		}
		if (GlobalInstance::getInstance()->getRefreshResTime() == 0)
		{
			GlobalInstance::getInstance()->saveRefreshResTime(GlobalInstance::servertime);
		}
		if (GlobalInstance::getInstance()->getRefreshMarketTime() == 0)
		{
			GlobalInstance::getInstance()->saveRefreshMarketTime(GlobalInstance::servertime);
		}

		if (GlobalInstance::getInstance()->getResetSilverRefHeroCountTime() == 0)
		{
			int zerotime = GlobalInstance::servertime + 8 * 60 * 60;
			GlobalInstance::getInstance()->setResetSilverRefHeroCountTime(zerotime - zerotime % (24 * 60 * 60));
		}

		costFoodsT = 0;

		updateTime(0);
		this->schedule(schedule_selector(MainScene::updateTime), 1);
	}
}

void MainScene::updateTime(float dt)
{
	GlobalInstance::servertime++;
	int respasttime = GlobalInstance::servertime - GlobalInstance::getInstance()->getRefreshResTime();
	if (respasttime >= RES_REFRESHTIME)
	{
		GlobalInstance::getInstance()->saveRefreshResTime(GlobalInstance::servertime - respasttime%RES_REFRESHTIME);
		for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
		{
			ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
			if (rescreator->getFarmersCount().getValue() >= 0 && rescreator->getLv().getValue() >= 0)
			{
				int addcount = 0;
				if (rescreator->getName().compare("r001") == 0)
				{
					int foodout = GlobalInstance::getInstance()->calcFoodMakeOut();
					addcount = foodout * respasttime / RES_REFRESHTIME;
					if (addcount + MyRes::getMyResCount("r001") < 0)
					{
						costFoodsT = (abs(addcount) - MyRes::getMyResCount("r001")) / abs(foodout);
					}
				}
				else
				{
					if (MyRes::getMyResCount("r001") <= 0)
						addcount = 0;
					else
						addcount = (respasttime / RES_REFRESHTIME - costFoodsT) * rescreator->getFarmersCount().getValue();
				}
				int maxcount = rescreator->getMaxCap(rescreator->getLv().getValue()).getValue();

				int rcount = MyRes::getMyResCount(rescreator->getName());

				if (addcount + rcount >= maxcount)
					addcount = maxcount - rcount;

				if (addcount < 0 && rcount < abs(addcount))
				{
					addcount = -rcount;
				}

				std::string formatstr;
				if (addcount < 0)
					formatstr = "%s%d";
				else
					formatstr = "%s+%d";

				if (addcount != 0)
				{
					MyRes::Add(rescreator->getName(), addcount);
					std::string showtext = StringUtils::format(formatstr.c_str(), GlobalInstance::map_AllResources[rescreator->getName()].name.c_str(), addcount);
					MovingLabel::show(showtext);
				}

			}
		}
	}


	bool isHasNewHero = false;
	for (unsigned int i = 0; i < GlobalInstance::vec_rand3Heros.size(); i++)
	{
		if (GlobalInstance::vec_rand3Heros[i]->getState() == HS_READY)
		{
			isHasNewHero = true;
			if (!isPlayNewHeroAnim)
			{
				isPlayNewHeroAnim = true;
				showInnRoomNewHeroAnim();
			}
			break;
		}
	}
	if (isPlayNewHeroAnim && !isHasNewHero)
	{
		isPlayNewHeroAnim = false;
		maincityhintbox->stopAllActions();
		maincityhintbox->setVisible(false);
	}
}

void MainScene::showInnRoomNewHeroAnim()
{
	maincityhintbox->runAction(RepeatForever::create(Sequence::create(Show::create(), ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), ScaleTo::create(0.1f, 1.2f), ScaleTo::create(0.1f, 1.0f), DelayTime::create(1.0f), Hide::create(), ScaleTo::create(0.0f, 0.0f), DelayTime::create(0.2f), NULL)));
}

void MainScene::delayGetServerTime(float dt)
{
	HttpDataSwap::init(this)->getServerTime();
}

void MainScene::updateTaskIcon()
{
	cocos2d::ui::ImageView* buildnametext = (cocos2d::ui::ImageView*)scroll_1->getChildByName("main_10_t");

	for (unsigned int i = 0; i < vec_taskicon.size(); i++)
	{
		vec_taskicon[i]->removeFromParentAndCleanup(true);
	}
	vec_taskicon.clear();

	if (GlobalInstance::myCurMainData.isfinish != QUEST_ACC)
	{
		Sprite* taskicon = Sprite::createWithSpriteFrameName("ui/maintask_icon.png");
		taskicon->setPosition(Vec2(80, 90));
		buildnametext->addChild(taskicon);
		vec_taskicon.push_back(taskicon);
	}
	if (GlobalInstance::myCurBranchData.isfinish != QUEST_ACC)
	{
		Sprite* taskicon = Sprite::createWithSpriteFrameName("ui/branchtask_icon.png");
		taskicon->setPosition(Vec2(80, 90));
		buildnametext->addChild(taskicon);
		vec_taskicon.push_back(taskicon);
	}

	int iconsize = vec_taskicon.size();
	if (iconsize == 2)
	{
		for (int i = 0; i < iconsize; i++)
		{
			vec_taskicon[i]->setPositionY(115 - 55 * i);
		}
	}
	for (int i = 0; i < iconsize; i++)
	{
		vec_taskicon[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(1.0f, Vec2(-10, 0)), MoveBy::create(1.0f, Vec2(10, 0)), NULL)));
	}
}
