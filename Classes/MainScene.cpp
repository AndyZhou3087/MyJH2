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
#include "DataSave.h"
#include "ErrorHintLayer.h"
#include "CommonFuncs.h"
#include "MatchMainLayer.h"
#include "LibraryLayer.h"
#include "FlowWorld.h"
#include "HomeHillLayer.h"
#include "GiftContentLayer.h"
#include "RandHeroLayer.h"
#include "MainHomeHintLayer.h"
#include "RepairBuildingLayer.h"
#include "SmallStallLayer.h"
#include "ShopLayer.h"
#include "TrainFinishLayer.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#endif

MainScene* g_mainScene = NULL;
MainScene::MainScene()
{
	m_isDraging = false;
	costFoodsT = 0;
	isPlayNewHeroAnim = false;
	tasktip = NULL;
	hostip = NULL;
	traintip = NULL;
	httpgettype = 0;
	ishasbeggar = false;
	ishintbeggar = false;
	isHasSmallStall = false;
	int z002count = MyRes::getMyResCount("z002", MYPACKAGE);
	if (z002count > 0)
		MyRes::Use("z002", z002count, MYPACKAGE);

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
    if (!Layer::init() )
    {
        return false;
    }

	int langtype = GlobalInstance::getInstance()->getLang();

	csbnode = CSLoader::createNode(ResourcePath::makePath("MainLayer.csb"));
	this->addChild(csbnode);

	g_MainMenuLayer = MainMenuLayer::create();
	this->addChild(g_MainMenuLayer);

	newsbg = (Sprite*)csbnode->getChildByName("newsbg");
	newsbg->setVisible(false);

	int hours = (GlobalInstance::getInstance()->getSysSecTime() + 8 * 60 * 60) % (TWENTYFOURHOURSTOSEC) / 3600;

	isnight = !(hours >= 6 && hours <= 21);

	lastisnight = !isnight;

	if (!isnight)
	{
		bool ishasbuildingbroken = false;
		if (!NewGuideLayer::checkifNewerGuide(72))
		{
			std::map<std::string, S_BUILDINREPAIR>::iterator bbit;

			for (bbit = GlobalInstance::map_buildingrepairdata.begin(); bbit != GlobalInstance::map_buildingrepairdata.end(); bbit++)
			{
				if (bbit->second.state > 0)
				{
					ishasbuildingbroken = true;
					break;
				}
			}
			if (!ishasbuildingbroken)
			{
				bool ishasnew = false;
				for (bbit = GlobalInstance::map_buildingrepairdata.begin(); bbit != GlobalInstance::map_buildingrepairdata.end(); bbit++)
				{
					int r = GlobalInstance::getInstance()->createRandomNum(1000);

					if (r < 50)
					{
						GlobalInstance::map_buildingrepairdata[bbit->first].state = 1;
						ishasnew = true;
					}
				}

				if (ishasnew)
					GlobalInstance::getInstance()->setBuildingBroken();
			}
		}
	}

	bool iscantrigger = true;
	std::map<std::string, S_BUILDINREPAIR>::iterator bbit;

	for (bbit = GlobalInstance::map_buildingrepairdata.begin(); bbit != GlobalInstance::map_buildingrepairdata.end(); bbit++)
	{
		if (bbit->second.state == 1)
		{
			iscantrigger = false;
			break;
		}
	}

	if (DataSave::getInstance()->getHasBeggar())
	{
		ishasbeggar = true;
		ishintbeggar = false;
	}
	else if (iscantrigger)
	{
		if (!NewGuideLayer::checkifNewerGuide(72))
		{
			int r = GlobalInstance::getInstance()->createRandomNum(1000);
			if (r < 50)
			{
				ishasbeggar = true;
				ishintbeggar = true;
				DataSave::getInstance()->setHasBeggar(true);
			}
		}
	}

	if (!isnight)
	{
		if (!isHasSmallStall)
		{
			int r = GlobalInstance::getInstance()->createRandomNum(1000);
			if (r < 50)
			{
				isHasSmallStall = true;
			}
		}
	}

	scroll_3 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_3");
	scroll_3->setScrollBarEnabled(false);
	scroll_3->jumpToPercentHorizontal(32);
	scroll_3->addEventListener(CC_CALLBACK_2(MainScene::srollviewlistenEvent, this));
	scroll_3->setSwallowTouches(false);

	scroll_2 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_2");
	scroll_2->setScrollBarEnabled(false);
	scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
	scroll_2->setSwallowTouches(false);

	scroll_1 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_1");
	scroll_1->setScrollBarEnabled(false);
	scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
	scroll_1->setSwallowTouches(false);

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
		buildingNomal->setTag(i);
		buildingNomal->addTouchEventListener(CC_CALLBACK_2(MainScene::onBuildingClick, this));

		if ((NewGuideLayer::checkifNewerGuide(63) && i == 2) || (i == 6 && GlobalInstance::getInstance()->getHerosLevelCount(10) <= 0)
			|| (GlobalInstance::getInstance()->getHerosLevelCount(20) <= 0 && i == 5) || (GlobalInstance::getInstance()->getHerosLevelCount(15) <= 0 && i == 3)
			|| (i == 8 && !GlobalInstance::getInstance()->getUnlockHomehillCondition() && NewGuideLayer::checkifNewerGuide(15))
			|| (GlobalInstance::getInstance()->getHerosLevelCount(20) <= 0 && i == 9))//医馆,市场,训练场，竞技场,后山,铁匠铺默认不开放
		{
			//buildingNomal->setTouchEnabled(false);
			buildnametext->setVisible(false);
			if (bulidinclipnode != NULL)
			{
				bulidinclipnode->setVisible(true);
			}
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
			tasktip = (cocos2d::ui::Widget*)buildnametext->getChildByName("main_10_p");
			tasktip->setVisible(false);
		}
		else if (i == 2)
		{
			hostip = (cocos2d::ui::Widget*)buildnametext->getChildByName("main_02_p");
		}
		else if (i == 5)
		{
			traintip = (cocos2d::ui::Widget*)buildnametext->getChildByName("main_05_p");
			traintip->setVisible(false);
		}
		else if (i == 1)
		{
			buildnametext->setVisible(false);
			buildingNomal->setEnabled(false);
		}
		buildingSelect->setVisible(false);
		buildingSelect->setUserData((void*)it->first.c_str());
		i++;
	}

	MyRes::putMyPackagesToStorage();
	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_HOME);

	this->scheduleOnce(schedule_selector(MainScene::delayShowNewerGuide), 1.1f);

	updateTaskLayerTip(0);
	this->schedule(schedule_selector(MainScene::updateTaskLayerTip), 1.0f);

	maincityhintbox = scroll_2->getChildByName("maincityhintbox");
	maincityhintbox->setScale(0);
	maincityhintbox->setVisible(false);
	cocos2d::ui::Text* hinttext = (cocos2d::ui::Text*)maincityhintbox->getChildByName("text");
	hinttext->setString(ResourceLang::map_lang["newherohint"]);

    //记录位置
    DataSave::getInstance()->setExitScene(0);

	//监测训练场开放
	if (GlobalInstance::getInstance()->getHerosLevelCount(20) <= 0)
	{
		this->schedule(schedule_selector(MainScene::checkBuildingOpen), 3.0f);
	}

	saveAllData();

	//for (unsigned i = 0; i < GlobalInstance::vec_TaskMain.size() - 2; i++)
	//{
	//	GlobalInstance::vec_TaskMain[i].isfinish = QUEST_FINISH;
	//	GlobalInstance::vec_TaskMain[i].finishtype = 0;
	//}
	//GlobalInstance::getInstance()->saveMyTaskMainData();

	//for (unsigned i = 0; i < GlobalInstance::vec_TaskBranch.size() -2; i++)
	//{
	//	GlobalInstance::vec_TaskBranch[i].isfinish = QUEST_FINISH;
	//	GlobalInstance::vec_TaskBranch[i].finishtype = 0;
	//}
	//GlobalInstance::getInstance()->saveMyTaskBranchData();

	Node* cloud = scroll_1->getChildByName("main_sky")->getChildByName("main_cloud");
	cloud->setVisible(!isnight);
	if (!isnight)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(870) + 1130;
		cloud->setPositionX(r);

		int t = (2800 - r) / 20;
		cloud->runAction(Sequence::create(MoveTo::create(t, Vec2(2800, cloud->getPositionY())), RemoveSelf::create(), NULL));

		r = GlobalInstance::getInstance()->createRandomNum(2);
		if (r <= 0)
		{
			std::string animstr = "mapbirds.csb";
			auto birdnode = CSLoader::createNode(animstr);

			r = GlobalInstance::getInstance()->createRandomNum(720) + 580;
			birdnode->setPosition(Vec2(r, 1100));
			scroll_1->addChild(birdnode);

			auto birdaction = CSLoader::createTimeline(animstr);
			birdnode->runAction(birdaction);
			birdaction->gotoFrameAndPlay(0, true);

			int tb = (3000 - r) / 40;

			birdnode->runAction(Sequence::create(MoveTo::create(tb, Vec2(3000, birdnode->getPositionY())), RemoveSelf::create(), NULL));
		}
	}

	changeDayOrNight();

	int maintaskcount = 0;
	for (unsigned i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
	{
		TaskData data = GlobalInstance::vec_TaskMain[i];
		if (data.isfinish >= QUEST_FINISH)
		{
			maintaskcount++;
		}
	}
	Quest::setAchieveTypeCount(FINISH_MAIN, maintaskcount);

	checkStarAchTask();

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
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MainScene::checkBuildingOpen(float dt)
{
	if (GlobalInstance::getInstance()->getHerosLevelCount(20) > 0)
	{
		this->unschedule(schedule_selector(MainScene::checkBuildingOpen));
		Node* textnode = scroll_3->getChildByName("main_05_t");
		if (textnode != NULL)
			textnode->setVisible(true);
		Node* cnode = scroll_3->getChildByName("main_05_c");
		if (cnode != NULL)
			cnode->setVisible(false);

		textnode = scroll_1->getChildByName("main_09_t");
		if (textnode != NULL)
			textnode->setVisible(true);

		cnode = scroll_1->getChildByName("main_09_c");
		if (cnode != NULL)
			cnode->setVisible(false);

	}
	if (GlobalInstance::getInstance()->getHerosLevelCount(10) > 0)
	{
		Node* textnode = scroll_2->getChildByName("main_06_t");
		if (textnode != NULL)
			textnode->setVisible(true);

		Node* cnode = scroll_2->getChildByName("main_06_c");
		if (cnode != NULL)
			cnode->setVisible(false);
	}
	if (GlobalInstance::getInstance()->getHerosLevelCount(15) > 0)
	{
		Node* textnode = scroll_3->getChildByName("main_03_t");
		if (textnode != NULL)
			textnode->setVisible(true);

		Node* cnode = scroll_3->getChildByName("main_03_c");
		if (cnode != NULL)
			cnode->setVisible(false);
	}
}

void MainScene::delayShowNewerGuide(float dt)
{
	if (NewGuideLayer::checkifNewerGuide(3))
	{
		showNewerGuide(3);
	}
	else if (NewGuideLayer::checkifNewerGuide(4))
	{
		showNewerGuide(4);
	}
	else if (NewGuideLayer::checkifNewerGuide(5))
	{
		showNewerGuide(5);
	}
	else if (NewGuideLayer::checkifNewerGuide(12) && !NewGuideLayer::checkifNewerGuide(5))
	{
		showNewerGuide(12);
	}
	else if (!NewGuideLayer::checkifNewerGuide(12))
	{
		if (NewGuideLayer::checkifNewerGuide(63) && GlobalInstance::getInstance()->getMyHerosDeadCount() > 0)
		{
			showNewerGuide(63);
		}
		else if (GlobalInstance::getInstance()->getHerosChangeLevelCount() > 0 && NewGuideLayer::checkifNewerGuide(69))
		{
			if (NewGuideLayer::checkifNewerGuide(66))
			{
				showNewerGuide(66);
			}
			else if (NewGuideLayer::checkifNewerGuide(69) && !NewGuideLayer::checkifNewerGuide(67))
			{
				showNewerGuide(69);
			}
		}
		else if (GlobalInstance::getInstance()->getHerosLevelCount(15) > 0 && NewGuideLayer::checkifNewerGuide(77))
		{
			if (NewGuideLayer::checkifNewerGuide(73))
			{
				showNewerGuide(73);
			}
			else if (NewGuideLayer::checkifNewerGuide(77))
			{
				showNewerGuide(77);
			}
		}
		/*else if (GlobalInstance::getInstance()->getUnlockHomehillCondition() && NewGuideLayer::checkifNewerGuide(15))
		{
			showNewerGuide(15);
		}*/

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

		else if (!NewGuideLayer::checkifNewerGuide(30) && NewGuideLayer::checkifNewerGuide(23))
		{
			showNewerGuide(23);
		}

		else if (NewGuideLayer::checkifNewerGuide(SECONDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(15))
			{
				showNewerGuide(15);
			}
		}
	}
}

void MainScene::showNewerGuide(int step)
{
	scroll_3->setEnabled(false);

	scroll_3->setDirection(cocos2d::ui::ScrollView::Direction::NONE);

	std::vector<Node*> nodes;


	if (step == 3 || step == 4 || step == 5)
	{

	}
	else if (step == 12)
	{
		scroll_3->jumpToPercentHorizontal(50);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
	}
	else if (step == 15)
	{
		scroll_3->jumpToPercentHorizontal(95);
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_08_n");//后山
		node->setTouchEnabled(true);
		node->setSwallowTouches(false);
		/*cocos2d::ui::Widget* cnode = (cocos2d::ui::Widget*)scroll_2->getChildByName("main_08_c");
		cnode->setVisible(false);*/
		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_08_t");
		text->setVisible(true);
		nodes.push_back(node);
	}
	else if (step == 22)
	{
		scroll_3->jumpToPercentHorizontal(50);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_07_n");//客栈
		nodes.push_back(node);
	}
	else if (step == 40 || step == 55)
	{
		scroll_3->jumpToPercentHorizontal(32);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_1->getChildByName("main_10_n");//议事厅
		nodes.push_back(node);
	}
	else if (step == 45)
	{
		//scroll_3->setEnabled(true);

		//scroll_3->jumpToPercentHorizontal(80);
		//scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		//scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		//cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_01_n");//出城
		//nodes.push_back(node);
		Node* node = g_MainMenuLayer->getChildByName("csbnode")->getChildByName("outbtn");
		nodes.push_back(node);
	}
	else if (step == 63)
	{
		DynamicValueInt dval = GlobalInstance::getInstance()->getMySoliverCount();
		if (dval.getValue() < 5000)
		{
			DynamicValueInt dva;
			dva.setValue(5000 - dval.getValue());
			GlobalInstance::getInstance()->addMySoliverCount(dva);
		}

		scroll_3->setEnabled(true);

		scroll_3->jumpToPercentHorizontal(100);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_02_n");//医馆
		node->setTouchEnabled(true);
		node->setSwallowTouches(false);
		cocos2d::ui::Widget* cnode = (cocos2d::ui::Widget*)scroll_3->getChildByName("main_02_c");
		cnode->setVisible(false);
		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_02_t");
		text->setVisible(true);
		nodes.push_back(node);
	}
	else if (step == 66)
	{
		scroll_3->jumpToPercentHorizontal(82);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_06_n");//市场
		node->setTouchEnabled(true);
		node->setSwallowTouches(false);
		cocos2d::ui::Widget* cnode = (cocos2d::ui::Widget*)scroll_2->getChildByName("main_06_c");
		cnode->setVisible(false);
		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_06_t");
		text->setVisible(true);
		nodes.push_back(node);
	}
	else if (step == 73)
	{
		scroll_3->setEnabled(true);

		scroll_3->jumpToPercentHorizontal(20);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_03_n");//铁匠铺
		node->setTouchEnabled(true);
		node->setSwallowTouches(false);
		cocos2d::ui::Widget* cnode = (cocos2d::ui::Widget*)scroll_3->getChildByName("main_03_c");
		cnode->setVisible(false);
		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_03_t");
		text->setVisible(true);
		nodes.push_back(node);
	}
	else if (step == 88)
	{
		scroll_3->setEnabled(true);

		scroll_3->jumpToPercentHorizontal(32);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_04_n");//仓库
		node->setTouchEnabled(true);
		node->setSwallowTouches(false);
		nodes.push_back(node);
	}
	else if (step == 23 || step == 69 || step == 77)
	{
		Node* node = g_MainMenuLayer->getChildByName("csbnode")->getChildByName("herobtn");
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

void MainScene::updateTaskLayerTip(float dt)
{
	//主线支线每日判断
	if (GlobalInstance::getInstance()->checkNewQuest())
	{
		tasktip->setVisible(true);
	}
	else
	{
		tasktip->setVisible(false);
	}

	//医馆提示
	int deadcount = 0;
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero* hero = GlobalInstance::vec_myHeros[i];
		if (hero->getState() == HS_DEAD)
		{
			deadcount++;
			break;
		}
	}
	if (deadcount > 0)
	{
		hostip->setVisible(true);
	}
	else
	{
		hostip->setVisible(false);
	}
}

void MainScene::setScrollGliding()
{
	scroll_3->setEnabled(true);
	scroll_3->setDirection(cocos2d::ui::ScrollView::Direction::HORIZONTAL);
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
	Vec2 pos = scroll_3->getInnerContainerPosition();
	//log("scoll posx:%f, posy:%f", pos.x, pos.y);
	////log("scoll eventType = %d", eventType);

	scroll_2->setInnerContainerPosition(pos);
	scroll_1->setInnerContainerPosition(pos);
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

bool MainScene::buildingIsClickOn(int tag)
{
	if (tag == 2)
	{
		if (NewGuideLayer::checkifNewerGuide(63))
		{
			return true;
		}
	}
	else if (tag == 3)
	{
		if (NewGuideLayer::checkifNewerGuide(73))
		{
			return true;
		}
	}
	else if (tag == 5)
	{
		if (GlobalInstance::getInstance()->getHerosLevelCount(20) <= 0)
		{
			return true;
		}
	}
	else if (tag == 6)
	{
		if (NewGuideLayer::checkifNewerGuide(66))
		{
			return true;
		}
	}
	else if (tag == 8)
	{
		if (!GlobalInstance::getInstance()->getUnlockHomehillCondition() && NewGuideLayer::checkifNewerGuide(15))
		{
			return true;
		}
	}
	else if (tag == 9)
	{
		if (GlobalInstance::getInstance()->getHerosLevelCount(20) <= 0)
		{
			return true;
		}
	}

	return false;
}

void MainScene::onEventBuildingClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::ImageView* clicknode = (cocos2d::ui::ImageView*)pSender;
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

		if (tag == 0)
		{
			SmallStallLayer* layer = SmallStallLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		else if (tag == 1)
		{
			RepairBuildingLayer* layer = RepairBuildingLayer::create("", 2);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
	{
		snode->setVisible(false);
	}
		break;
	default:
		break;
	}

}

void MainScene::onBuildingClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::ImageView* clicknode = (cocos2d::ui::ImageView*)pSender;
	Node* snode = (Node*)clicknode->getUserData();
	int tag = clicknode->getTag();
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

			if ((NewGuideLayer::checkifNewerGuide(63) && tag == 2) || (tag == 6 && GlobalInstance::getInstance()->getHerosLevelCount(10) <= 0)
				|| (GlobalInstance::getInstance()->getHerosLevelCount(20) <= 0 && tag == 5) || (GlobalInstance::getInstance()->getHerosLevelCount(15) <= 0 && tag == 3)
				|| (tag == 8 && !GlobalInstance::getInstance()->getUnlockHomehillCondition() && NewGuideLayer::checkifNewerGuide(15))
				|| (GlobalInstance::getInstance()->getHerosLevelCount(20) <= 0 && tag == 9))
			{
				Node* buildParent;
				if (tag <= 5)
				{
					buildParent = scroll_3;
				}
				else if (tag <= 8)
				{
					buildParent = scroll_2;
				}
				else
				{
					buildParent = scroll_1;
				}
				
				std::string strkey = StringUtils::format("unlockmain_text_%d_%d", tag, GlobalInstance::getInstance()->createRandomNum(2));
				std::string contentstr = ResourceLang::map_lang[strkey];
				contentstr.append("\n");
				strkey = StringUtils::format("unlockmain_%d", tag);
				contentstr.append(ResourceLang::map_lang[strkey]);
				MovingLabel::showbyNode(buildParent, contentstr, (Color4B)Color3B::WHITE, clicknode->getPosition());
			}
			else
			{
				Layer* layer = NULL;

				if (buildname.compare("0outtown") == 0)
				{
					layer = OutTownLayer::create();
				}
				else if (buildname.compare("1hospital") == 0)
				{
					layer = HospitalLayer::create();
				}
				else if (buildname.compare("2smithy") == 0)
				{
					layer = SmithyLayer::create(Building::map_buildingDatas[buildname]);
				}
				else if (buildname.compare("3storehouse") == 0)
				{
					layer = StoreHouseLayer::create();
				}
				else if (buildname.compare("4trainigroom") == 0)
				{
					layer = TrainLayer::create(Building::map_buildingDatas[buildname]);
				}
				else if (buildname.compare("5market") == 0)
				{
					layer = MarketLayer::create(Building::map_buildingDatas[buildname]);
				}
				else if (buildname.compare("6innroom") == 0)
				{
					buildname = "RandHeroLayer";
					//layer = InnRoomLayer::create(Building::map_buildingDatas[buildname]);
					layer = RandHeroLayer::create();
				}
				else if (buildname.compare("7homehill") == 0)
				{
					layer = HomeHillLayer::create(Building::map_buildingDatas[buildname]);
					//layer = HomeHillLayer2::create(Building::map_buildingDatas[buildname]);
				}
				else if (buildname.compare("8pkground") == 0)
				{
					layer = MatchMainLayer::create();
				}
				else if (buildname.compare("9assemblyhall") == 0)
				{
					layer = TaskLayer::create();
				}
				else if (buildname.compare("Alibrary") == 0)
				{
					layer = LibraryLayer::create();
				}
				if (layer != NULL)
				{
					this->addChild(layer, 0, buildname);
					AnimationEffect::openAniEffect(layer);
				}

				break;
			}
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
	if (httpgettype == 0)
	{
		ErrorHintLayer* networkerrLayer = (ErrorHintLayer*)this->getChildByName("networkerrorlayer");

		if (code == SUCCESS)
		{
			if (networkerrLayer != NULL)
				networkerrLayer->removeFromParentAndCleanup(true);

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
				GlobalInstance::getInstance()->setResetSilverRefHeroCountTime(zerotime - zerotime % TWENTYFOURHOURSTOSEC);
			}

			costFoodsT = 0;

			updateTime(0);
			this->schedule(schedule_selector(MainScene::updateTime), 1);

			if (GlobalInstance::servertime - GlobalInstance::getNewsTime >= 4 * 60)
			{
				GlobalInstance::getNewsTime = GlobalInstance::servertime;
				httpgettype = 1;
				HttpDataSwap::init(this)->getNews();
			}

			doBuildingEvent();
		}
		else
		{
			if (networkerrLayer == NULL)
			{
				ErrorHintLayer* layer = ErrorHintLayer::create(0);
				this->addChild(layer, 1000, "networkerrorlayer");
			}
			else
			{
				networkerrLayer->resetBtn();
			}
		}
	}
	else if (httpgettype == 1)
	{
		if (code == SUCCESS)
		{
			if (GlobalInstance::vec_news.size() > 0)
			{
				if (!newsbg->isVisible())
				{
					newsbg->setVisible(true);
					this->addChild(FlowWorld::getIntance()->createFW(newsbg));
				}
			}
		}
		httpgettype = 0;
	}
	else if (httpgettype == 2)
	{
		if (code == SUCCESS)
		{
			if (GlobalInstance::ispayconfirm)
			{
				ShopLayer::paySucc();
				GlobalInstance::ispayconfirm = false;
			}
		}
		httpgettype = 0;
	}
}

void MainScene::checkorder(std::string orderid, std::string goodsid, int price)
{
	httpgettype = 2;
	HttpDataSwap::init(this)->paySuccNotice(orderid, goodsid, price);
}

void MainScene::updateTime(float dt)
{
	GlobalInstance::servertime++;
	int respasttime = GlobalInstance::servertime - GlobalInstance::getInstance()->getRefreshResTime();
	if (respasttime >= RES_REFRESHTIME)
	{
		GlobalInstance::getInstance()->saveRefreshResTime(GlobalInstance::servertime - respasttime % RES_REFRESHTIME);
		for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
		{
			ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
			//if (rescreator->getFarmersCount().getValue() >= 0 && rescreator->getLv().getValue() >= 0)
			if (rescreator->getFarmersCount().getValue() >= 0)
			{
				int addcount = 0;//需要增加的数量
				if (rescreator->getName().compare("r001") == 0)
				{
					int foodout = GlobalInstance::getInstance()->calcFoodMakeOut();//计算其它采集消耗的食物
					addcount = foodout * respasttime / RES_REFRESHTIME;//可能增加食物食物数量为负数
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
				int maxcount = rescreator->getMaxCap(rescreator->getLv().getValue()).getValue();//最大容量

				int rcount = MyRes::getMyResCount(rescreator->getName());//现有资源

				if (addcount < 0)
				{
					if (addcount + rcount <= 0)
						addcount = -rcount;
				}
				else
				{
					if (rcount > maxcount)
					{
						//MyRes::Add(rescreator->getName(), maxcount - rcount);
						//rcount = maxcount;
						addcount = 0;
					}
					else if (addcount + rcount > maxcount)
						addcount = maxcount - rcount;
				}

				std::string formatstr;
				if (addcount < 0)
					formatstr = "%s%d";
				else
					formatstr = "%s+%d";

				if (addcount != 0)
				{
					MyRes::Add(rescreator->getName(), addcount);
					if (g_mainScene != NULL)
					{
						std::string showtext = StringUtils::format(formatstr.c_str(), GlobalInstance::map_AllResources[rescreator->getName()].name.c_str(), addcount);
						MovingLabel::show(showtext);
					}
				}
				else
				{
					if (MyRes::getMyResCount("r001") <= 0 && rescreator->getName().compare("r001") != 0 && rescreator->getFarmersCount().getValue() > 0)//食物不足时，提示不产出
					{
						std::string showtext = StringUtils::format(ResourceLang::map_lang["canntoutput"].c_str(), GlobalInstance::map_AllResources[rescreator->getName()].name.c_str());
						MovingLabel::show(showtext);
					}
				}

			}
		}

		if (ishasbeggar)
		{
			MyRes::Use("r001", 1);
			MovingLabel::show(ResourceLang::map_lang["beggarlossres"]);
		}
	}

	int refreshtime = GlobalInstance::getInstance()->getRefreshHeroTime();
	int pasttime = GlobalInstance::servertime - refreshtime;
	if (pasttime >= HERO_RESETTIME)
	{
		GlobalInstance::isNewHeroRefresh = true;
	}

	if (GlobalInstance::isNewHeroRefresh)
	{
		if (!isPlayNewHeroAnim)
		{
			isPlayNewHeroAnim = true;

			showInnRoomNewHeroAnim();
		}
	}
	else
	{
		isPlayNewHeroAnim = false;
		maincityhintbox->stopAllActions();
		maincityhintbox->setVisible(false);
	}

	bool istainfinish = false;
	for (unsigned int m = 0; m < GlobalInstance::vec_myHeros.size(); m++)
	{
		Hero* trainhero = GlobalInstance::vec_myHeros[m];
		if (trainhero->getState() == HS_TRAINING)
		{
			int refreshtime = trainhero->getTrainTime();
			int pasttime = GlobalInstance::servertime - refreshtime;
			if (pasttime >= trainhero->getTrainHour())
			{
				istainfinish = true;
				break;
				//int lv = Building::map_buildingDatas["4trainigroom"]->level.getValue();
				//int bexp = Building::map_buildingDatas["4trainigroom"]->vec_exdatatrain[lv];

				//int lastLevel = trainhero->getLevel();
				//trainhero->setExpLimit(trainhero->getTrainHour() / 3600 * bexp);
				//int curLevel = trainhero->getLevel();
				////if (lastLevel <= curLevel - 1)
				////{
				////	CommonFuncs::playCommonLvUpAnim(this, "texiao_sjcg");
				////}

				//trainhero->setTrainHour(0);
				//trainhero->setTrainTime(0);
				//trainhero->setState(HS_OWNED);
			}
		}
	}

	if (istainfinish && this->getChildByName("trainfinishlayer") == NULL)
	{
		TrainFinishLayer* layer = TrainFinishLayer::create();
		this->addChild(layer, 10000, "trainfinishlayer");
	}

	int zerotime = GlobalInstance::servertime + 8 * 60 * 60;
	//议事厅每日更新
	int t = zerotime / TWENTYFOURHOURSTOSEC;
	if (t > DataSave::getInstance()->getMyFreshDate())
	{
		Quest::resetDailyTask();
		GlobalInstance::timeMarketStr = "";
		GlobalInstance::map_timeMartData.clear();
		DataSave::getInstance()->deleteDataByKey("timemarket");
	}

	changeDayOrNight();

	std::map<std::string, S_BUILDINREPAIR>::iterator bbit;

	bool issavebroken = false;
	for (bbit = GlobalInstance::map_buildingrepairdata.begin(); bbit != GlobalInstance::map_buildingrepairdata.end(); bbit++)
	{
		if (bbit->second.state == 3)
		{
			int pasttime = GlobalInstance::servertime - GlobalInstance::map_buildingrepairdata[bbit->first].repairtime;

			if (pasttime >= REPAIRTIME)
			{
				GlobalInstance::map_buildingrepairdata[bbit->first].state = 0;
				GlobalInstance::map_buildingrepairdata[bbit->first].repairtime = 0;
				issavebroken = true;
				repairFinish(bbit->first);
				showRepairFinishAwd(bbit->first);
			}
			else
			{
				showRepairAnim(bbit->first);
			}
		}
	}

	if (issavebroken)
		GlobalInstance::getInstance()->setBuildingBroken();
}

void MainScene::changeDayOrNight()
{
	int hours = (GlobalInstance::getInstance()->getSysSecTime() + 8 * 60 * 60) % (TWENTYFOURHOURSTOSEC) / 3600;

	isnight = !(hours >= 6 && hours <= 21);

	if (lastisnight == isnight)
		return;

	lastisnight = isnight;

	std::string otherpicparentname[] = { "scroll_1", "scroll_2", "scroll_2", "scroll_2", "scroll_3" };
	std::string otherpicname[] = { "main_sky", "main_back_b", "main_bg_l", "main_bg_r", "main_back_f" };
	std::string otherpicext[] = { "jpg", "jpg", "png", "png", "jpg" };
	for (unsigned int i = 0; i < sizeof(otherpicname) / sizeof(otherpicname[0]); i++)
	{
		cocos2d::ui::ImageView* otherpic = (cocos2d::ui::ImageView*)csbnode->getChildByName(otherpicparentname[i])->getChildByName(otherpicname[i]);
		std::string otherpicnightname;
		
		if (isnight)
			otherpicnightname = StringUtils::format("mainimg/%s_n.%s", otherpicname[i].c_str(), otherpicext[i].c_str());
		else
			otherpicnightname = StringUtils::format("mainimg/%s.%s", otherpicname[i].c_str(), otherpicext[i].c_str());
		otherpic->loadTexture(ResourcePath::makePath(otherpicnightname), cocos2d::ui::Widget::TextureResType::LOCAL);
	}

	std::map<std::string, Building*>::iterator it;
	int i = 1;
	for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
	{
		Node* buildParent;
		if (i <= 5)
			buildParent = scroll_3;
		else if (i <= 8)
			buildParent = scroll_2;
		else
			buildParent = scroll_1;

		std::string buildpicname = StringUtils::format("main_%02d_pic", i);
		cocos2d::ui::ImageView* buildpic = (cocos2d::ui::ImageView*)buildParent->getChildByName(buildpicname);
		if (isnight)
		{
			std::string buildnightstr = StringUtils::format("mainimg/main_%02d_n_n.png", i);
			buildpic->loadTexture(ResourcePath::makePath(buildnightstr), cocos2d::ui::Widget::TextureResType::LOCAL);
		}
		else
		{
			bool isbroken = false;
			if (GlobalInstance::map_buildingrepairdata.find(it->first) != GlobalInstance::map_buildingrepairdata.end())
			{
				if (GlobalInstance::map_buildingrepairdata[it->first].state > 0)
					isbroken = true;
			}
			if (isbroken)
			{
				std::string buildnightstr = StringUtils::format("mainimg/main_%02d_n_b.png", i);
				buildpic->loadTexture(ResourcePath::makePath(buildnightstr), cocos2d::ui::Widget::TextureResType::LOCAL);
			}
			else
			{
				std::string buildnightstr = StringUtils::format("mainimg/main_%02d_n.png", i);
				buildpic->loadTexture(ResourcePath::makePath(buildnightstr), cocos2d::ui::Widget::TextureResType::LOCAL);
			}
		}
		i++;
	}

	cocos2d::ui::ImageView* smallstall = (cocos2d::ui::ImageView*)scroll_3->getChildByName("smallstall");
	cocos2d::ui::ImageView* smallstall_s = (cocos2d::ui::ImageView*)scroll_3->getChildByName("smallstall_s");
	smallstall_s->setVisible(false);
	smallstall->setSwallowTouches(false);
	smallstall->setTag(0);
	smallstall->setUserData((void*)smallstall_s);
	smallstall->addTouchEventListener(CC_CALLBACK_2(MainScene::onEventBuildingClick, this));

	smallstall->setVisible(isHasSmallStall && !isnight);


	beggar = (cocos2d::ui::ImageView*)scroll_3->getChildByName("beggar_n");
	cocos2d::ui::ImageView* beggar_s = (cocos2d::ui::ImageView*)scroll_3->getChildByName("beggar_s");
	beggar->setVisible(ishasbeggar);
	beggar_s->setVisible(false);
	beggar->setTag(1);
	beggar->setSwallowTouches(false);
	beggar->setUserData((void*)beggar_s);
	beggar->addTouchEventListener(CC_CALLBACK_2(MainScene::onEventBuildingClick, this));
	std::string beggarstr;
	if (isnight)
		beggarstr = "mainimg/beggar_n_n.png";
	else
		beggarstr = "mainimg/beggar_n.png";
	beggar->loadTexture(ResourcePath::makePath(beggarstr), cocos2d::ui::Widget::TextureResType::LOCAL);
}

void MainScene::repairFinish(std::string buildingname)
{
	std::map<std::string, Building*>::iterator it;
	int i = 1;
	int buildindex = 0;
	Node* buildParent = NULL;
	for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
	{
		if (it->first.compare(buildingname) == 0)
		{
			if (i <= 5)
				buildParent = scroll_3;
			else if (i <= 8)
				buildParent = scroll_2;
			else
				buildParent = scroll_1;
			buildindex = i;
			break;
		}
		i++;
	}
	if (!isnight)
	{
		if (GlobalInstance::map_buildingrepairdata[buildingname].state == 0)
		{
			std::string buildpicname = StringUtils::format("main_%02d_pic", buildindex);
			cocos2d::ui::ImageView* buildpic = (cocos2d::ui::ImageView*)buildParent->getChildByName(buildpicname);

			std::string buildnightstr = StringUtils::format("mainimg/main_%02d_n.png", buildindex);
			buildpic->loadTexture(ResourcePath::makePath(buildnightstr), cocos2d::ui::Widget::TextureResType::LOCAL);

			for (int n = 0; n < 2; n++)
			{
				std::string animname = StringUtils::format("repair_%02d_%d", buildindex, n+1);

				buildParent->removeChildByName(animname);
			}
		}
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

 	if (GlobalInstance::myCurMainData.isfinish == QUEST_TASK)
	{
		Sprite* taskicon = Sprite::createWithSpriteFrameName("ui/maintask_icon.png");
		taskicon->setPosition(Vec2(80, 90));
		buildnametext->addChild(taskicon);
		vec_taskicon.push_back(taskicon);
	}
	if (GlobalInstance::myCurBranchData.isfinish == QUEST_TASK)
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

void MainScene::saveAllData()
{
	//if (!GlobalInstance::isNotSameUUID)
	{
		GlobalInstance::getInstance()->saveMyHeros();
		MyRes::saveData();
		HttpDataSwap::init(NULL)->postAllData();
	}
}

void MainScene::cheatAction(int type)
{
	int para = 0;
	if (type == 1)
	{
		int cheatcount = DataSave::getInstance()->getCheatCount();
		int para = 1;
		if (cheatcount >= 2)
			para = 2;
	}
	else if (type == 3)
	{
		para = 3;
	}
	else if (type == 4)
	{
		para = 4;
	}
	
	if (!GlobalInstance::isCheat)
	{
		GlobalInstance::isCheat = true;
		ErrorHintLayer* layer = ErrorHintLayer::create(para);
		Director::getInstance()->getRunningScene()->addChild(layer, 10000);
		AnimationEffect::openAniEffect(layer);
	}
}

void MainScene::addNews(std::string content, int type)
{
	std::string utf8str = content;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	utf8str = gbkToUTF8(content.c_str());
#endif
	HttpDataSwap::init(NULL)->addNews(utf8str, type);
}

void MainScene::showVipReward(int payindex)
{
	this->setTag(payindex);
	this->scheduleOnce(schedule_selector(MainScene::delayShowVipReward), 0.5f);
}

void MainScene::delayShowVipReward(float dt)
{
	int payindex = this->getTag();
	GiftContentLayer* layer = GiftContentLayer::create(&GlobalInstance::vec_shopdata[payindex], payindex, 1);
	if (g_mainScene != NULL)
	{
		g_mainScene->addChild(layer, 10, "viprewardlayer");
	}
}

void MainScene::doBuildingEvent()
{
	bool ishas = false;
	std::map<std::string, S_BUILDINREPAIR>::iterator it;

	for (it = GlobalInstance::map_buildingrepairdata.begin(); it != GlobalInstance::map_buildingrepairdata.end(); it++)
	{
		if (it->second.state == 1)
		{
			ishas = true;
			break;
		}
	}

	if (ishas)
	{
		MainHomeHintLayer* layer = MainHomeHintLayer::create(0);
		addChild(layer);
	}
	else if (ishintbeggar)
	{
		ishintbeggar = false;
		MainHomeHintLayer* layer = MainHomeHintLayer::create(1);
		addChild(layer);
	}
}


void MainScene::showRepairFinishAwd(std::string buildingname)
{
	bool ishas = false;
	std::map<std::string, S_BUILDINREPAIR>::iterator it;

	int r = GlobalInstance::getInstance()->createRandomNum(100);
	if (r < 20)
	{
		RepairBuildingLayer* layer = RepairBuildingLayer::create(buildingname, 1);
		addChild(layer);
	}
}

void MainScene::showRepairAnim(std::string buildingname)
{
	Vec2 animpos1[] = { Vec2(2230, 490), Vec2(510,400), Vec2(85, 440), Vec2(1810, 510), Vec2(1090, 630)};
	Vec2 animpos2[] = { Vec2(1935, 245), Vec2(380,210), Vec2(100, 280), Vec2(1560, 345), Vec2(1250, 345) };
	std::map<std::string, Building*>::iterator it;
	int i = 1;
	int buildindex = 0;
	Node* buildParent = NULL;
	for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
	{
		if (it->first.compare(buildingname) == 0)
		{
			if (i <= 5)
				buildParent = scroll_3;
			else if (i <= 8)
				buildParent = scroll_2;
			else
				buildParent = scroll_1;
			buildindex = i;
			break;
		}
		i++;
	}

	int inbuildbrokenindex = -1;

	std::map<std::string, S_BUILDINREPAIR>::iterator bbit;

	for (bbit = GlobalInstance::map_buildingrepairdata.begin(); bbit != GlobalInstance::map_buildingrepairdata.end(); bbit++)
	{
		inbuildbrokenindex++;
		if (bbit->first.compare(buildingname) == 0)
		{
			break;
		}
	}

	if (!isnight)
	{
		bool ishasanim = false;
		std::string name1 = StringUtils::format("repair_%02d_1", buildindex);
		std::string name2 = StringUtils::format("repair_%02d_2", buildindex);
		if (buildParent->getChildByName(name1) != NULL || buildParent->getChildByName(name2) != NULL)
			ishasanim = true;

		if (!ishasanim)
		{
			int r = GlobalInstance::getInstance()->createRandomNum(3) + 1;
			if (r == 1 || r == 3)
			{

				Node* animnode = CSLoader::createNode(ResourcePath::makePath("repairanim1.csb"));
				animnode->setPosition(animpos1[inbuildbrokenindex]);
				buildParent->addChild(animnode, 0, name1);

				auto action = CSLoader::createTimeline("repairanim1.csb");
				animnode->runAction(action);
				action->gotoFrameAndPlay(0, true);
			}

			if (r == 2 || r == 3)
			{

				Node* animnode = CSLoader::createNode(ResourcePath::makePath("repairanim2.csb"));
				animnode->setPosition(animpos2[inbuildbrokenindex]);
				if (buildingname.compare("6innroom") == 0)
				{
					Sprite* repairfarmer = (Sprite*)animnode->getChildByName("repairanim");
					repairfarmer->setFlippedX(true);
					animnode->setScale(0.6f);
				}
				buildParent->addChild(animnode, 0, name2);
				auto action = CSLoader::createTimeline("repairanim2.csb");
				animnode->runAction(action);
				action->gotoFrameAndPlay(0, true);
			}
		}
	}
}

void MainScene::hideBeggar()
{
	ishasbeggar = false;
	ishintbeggar = false;
	beggar->setVisible(false);
}

void MainScene::checkStarAchTask()
{
	int totalstar = 0;
	tinyxml2::XMLDocument* pDoc = new tinyxml2::XMLDocument();

	std::string content = GlobalInstance::getInstance()->getUserDefaultXmlString(1);

	int err = pDoc->Parse(content.c_str());
	if (err != 0)
	{
		delete pDoc;
	}
	tinyxml2::XMLElement* rootEle = pDoc->RootElement();

	tinyxml2::XMLElement* element = rootEle->FirstChildElement();
	while (element != NULL)
	{
		std::string key = element->Name();

		if (key.find("jhstarm") != std::string::npos)
		{
			if (element->GetText() != NULL)
			{
				std::string textstr = DataSave::getInstance()->getFinishStar(key.substr(6));
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(textstr, vec_tmp, ",");
				totalstar += vec_tmp.size();
			}
		}
		element = element->NextSiblingElement();
	}

	Quest::setAchieveTypeCount(GET_STARS, totalstar);
}