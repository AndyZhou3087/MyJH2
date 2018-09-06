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

USING_NS_CC;
MainScene* g_mainScene = NULL;
MainScene::MainScene()
{
	m_isDraging = false;
	costFoodsT = 0;
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
	scroll_2->jumpToPercentHorizontal(32);

	scroll_1 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_1");
	scroll_1->setScrollBarEnabled(false);
	scroll_1->jumpToPercentHorizontal(32);
	std::map<std::string, Building*>::iterator it;
	int i = 1;
	for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
	{
		std::string buidingNomalName;
		std::string buidingSelectName;
		std::string bulidingname;
		cocos2d::ui::ImageView* buildingNomal;
		cocos2d::ui::ImageView* buildingSelect;
		cocos2d::ui::ImageView* buildnametext;

		buidingNomalName = StringUtils::format("main_%02d_n", i);//可点击
		buidingSelectName = StringUtils::format("main_%02d_s", i);//选中

		bulidingname = StringUtils::format("main_%02d_t", i);//文字

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

		buildnametext->loadTexture(ResourcePath::makeTextImgPath(bulidingname, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		buildingNomal->setSwallowTouches(false);
		buildingNomal->setUserData((void*)buildingSelect);
		buildingNomal->addTouchEventListener(CC_CALLBACK_2(MainScene::onBuildingClick, this));

		if (i == 4)
		{
			std::string buidingNomalName_1 = StringUtils::format("main_%02d_n_1", i);
			cocos2d::ui::ImageView* buildingNomal_1 = (cocos2d::ui::ImageView*)buildParent->getChildByName(buidingNomalName_1);
			buildingNomal_1->setSwallowTouches(false);
			buildingNomal_1->setUserData((void*)buildingSelect);
			buildingNomal_1->addTouchEventListener(CC_CALLBACK_2(MainScene::onBuildingClick, this));
		}
		buildingSelect->setVisible(false);
		buildingSelect->setUserData((void*)it->first.c_str());
		i++;
	}

	MyRes::putMyPackagesToStorage();
	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_HOME);

	this->scheduleOnce(schedule_selector(MainScene::delayShowNewerGuide), 0.1f);

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
		if (NewGuideLayer::checkifNewerGuide(SECONDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(15))
			{
				showNewerGuide(15);
			}
		}
		else if (NewGuideLayer::checkifNewerGuide(THRIDGUIDESTEP))
		{
			if (NewGuideLayer::checkifNewerGuide(22))
			{
				showNewerGuide(22);
			}
		}
	}
}

void MainScene::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 15)
	{
		scroll_1->jumpToPercentHorizontal(95);
		scroll_2->jumpToPercentHorizontal(95);
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_08_n");
		nodes.push_back(node);
	}
	else if (step == 22)
	{
		scroll_1->jumpToPercentHorizontal(32);
		scroll_2->jumpToPercentHorizontal(32);
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)scroll_2->getChildByName("main_07_n");
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

void MainScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	HttpDataSwap::init(this)->getServerTime();
}

void MainScene::srollviewlistenEvent(Ref* ref, ui::ScrollView::EventType eventType)
{
	Vec2 pos = scroll_3->getInnerContainerPosition();
	log("scoll posx:%f, posy:%f", pos.x, pos.y);
	switch (eventType) 
	{
		//最外层滑动时，带动后两层滑动，可修改时间调整效果
		case ui::ScrollView::EventType::CONTAINER_MOVED:
			//将引擎中的startAutoScrollToDestination 修改为pulic
			scroll_2->startAutoScrollToDestination(pos, 0.1f, true);
			scroll_1->startAutoScrollToDestination(pos, 0.2f, true);
			break;
		default:
			break;
	}
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
}

void MainScene::checkHint(float dt)
{

}
