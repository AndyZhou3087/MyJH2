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
#include "LibraryLayer2.h"
#include "HomeHillLayer2.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#endif

USING_NS_CC;
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
    if ( !Layer::init() )
    {
        return false;
    }

	int langtype = GlobalInstance::getInstance()->getLang();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("MainLayer.csb"));
	this->addChild(csbnode);

	g_MainMenuLayer = MainMenuLayer::create();
	this->addChild(g_MainMenuLayer);

	newsbg = (Sprite*)csbnode->getChildByName("newsbg");
	newsbg->setVisible(false);

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

	DataSave::getInstance()->setFirstEnter(false);
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
	if (NewGuideLayer::checkifNewerGuide(FIRSTGUIDESTEP))
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
	}
	else if (NewGuideLayer::checkifNewerGuide(12) && NewGuideLayer::checkifNewerGuide(14))
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
		else if (NewGuideLayer::checkifNewerGuide(SECONDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(15))
			{
				showNewerGuide(15);
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
	else if (step == 22 || step == 69 || step == 77)
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
		scroll_3->setEnabled(true);

		scroll_3->jumpToPercentHorizontal(80);
		scroll_1->setInnerContainerPosition(scroll_3->getInnerContainerPosition());
		scroll_2->setInnerContainerPosition(scroll_3->getInnerContainerPosition());

		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_3->getChildByName("main_01_n");//出城
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
					layer = InnRoomLayer::create(Building::map_buildingDatas[buildname]);
				}
				else if (buildname.compare("7homehill") == 0)
				{
					//layer = HomeHillLayer::create(Building::map_buildingDatas[buildname]);
					layer = HomeHillLayer2::create(Building::map_buildingDatas[buildname]);
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
					layer = LibraryLayer2::create();
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

			if (GlobalInstance::servertime - GlobalInstance::getNewsTime >= 5 * 60)
			{
				GlobalInstance::getNewsTime = GlobalInstance::servertime;
				httpgettype = 1;
				HttpDataSwap::init(this)->getNews();
			}
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
	else
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

	for (unsigned int m = 0; m < GlobalInstance::vec_myHeros.size(); m++)
	{
		Hero* trainhero = GlobalInstance::vec_myHeros[m];
		if (trainhero->getState() == HS_TRAINING)
		{
			int refreshtime = trainhero->getTrainTime();
			int pasttime = GlobalInstance::servertime - refreshtime;
			if (pasttime >= trainhero->getTrainHour())
			{
				int lv = Building::map_buildingDatas["4trainigroom"]->level.getValue();
				int bexp = Building::map_buildingDatas["4trainigroom"]->vec_exdatatrain[lv];

				int lastLevel = trainhero->getLevel();
				trainhero->setExpLimit(trainhero->getTrainHour() / 3600 * bexp);
				int curLevel = trainhero->getLevel();
				//if (lastLevel <= curLevel - 1)
				//{
				//	CommonFuncs::playCommonLvUpAnim(this, "texiao_sjcg");
				//}

				trainhero->setTrainHour(0);
				trainhero->setTrainTime(0);
				trainhero->setState(HS_OWNED);

				traintip->setVisible(true);
				std::string showstr = StringUtils::format(ResourceLang::map_lang["trainfinishtext"].c_str(), trainhero->getName().c_str());
				MovingLabel::show(showstr);
			}
		}
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
	if (!GlobalInstance::isNotSameUUID)
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
	HttpDataSwap::init(NULL)->addNews(utf8str);
}
