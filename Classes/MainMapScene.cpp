#include "MainMapScene.h"
#include "Resource.h"
#include "SelectSubMapLayer.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MainScene.h"
#include "SoundManager.h"
#include "NewGuideLayer.h"

MainMapScene::MainMapScene()
{
	m_isDraging = false;
}


MainMapScene::~MainMapScene()
{

}

Scene* MainMapScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto mainLayer = MainMapScene::create();

	// add layer as a child to scene
	scene->addChild(mainLayer);

	// return the scene
	return scene;
}

bool MainMapScene::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mainMapLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ScrollView* mapscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("mapscroll");
	mapscroll->setScrollBarEnabled(false);
	mapscroll->setSwallowTouches(false);
	mapscroll->jumpToPercentHorizontal(100);

	cocos2d::ui::Widget* mapbg = (cocos2d::ui::Widget*)mapscroll->getChildByName("mapbg");
	mapbg->setScale(0.8f);
	int mapnamecount = mapbg->getChildrenCount();

	std::map<std::string, std::vector<Node*>> map_taskicon;

	for (int i = 0; i < mapnamecount; i++)
	{
		cocos2d::ui::Widget* mapname = (cocos2d::ui::Widget*)mapbg->getChildren().at(i);
		cocos2d::ui::Widget* normal = (cocos2d::ui::Widget*)mapname->getChildByName("normal");
		cocos2d::ui::Widget* select = (cocos2d::ui::Widget*)mapname->getChildByName("select");
		select->setVisible(false);
		normal->setUserData((void*)select);
		normal->addTouchEventListener(CC_CALLBACK_2(MainMapScene::onclick, this));
		normal->setSwallowTouches(false);
		std::string mname = mapname->getName();
		int c = atoi(mname.substr(1, mname.find_first_of("-") - 1).c_str());

		std::string mapnameid = GlobalInstance::myCurMainData.place.substr(0, 4);
		if ((mapnameid.compare(mname) == 0 && GlobalInstance::myCurMainData.isfinish == QUEST_ACC) || (GlobalInstance::myCurMainData.isfinish != QUEST_ACC && mname.compare("m0-1") == 0))
		{
			Sprite* taskicon = Sprite::createWithSpriteFrameName("ui/maintask_icon.png");
			taskicon->setPosition(Vec2(55, 15));
			taskicon->setScale(0.8f);
			mapname->addChild(taskicon);
			map_taskicon[mname].push_back(taskicon);

			if (!NewGuideLayer::checkifNewerGuide(FOURTHGUIDESTEP))
			{
				if (NewGuideLayer::checkifNewerGuide(53))
				{
					showNewerGuide(53, normal);
				}
			}
		}
		mapnameid = GlobalInstance::myCurBranchData.place.substr(0, 4);
		if ((mapnameid.compare(mname) == 0 && GlobalInstance::myCurBranchData.isfinish == QUEST_ACC) || (GlobalInstance::myCurBranchData.isfinish != QUEST_ACC && mname.compare("m0-1") == 0))
		{
			Sprite* taskicon = Sprite::createWithSpriteFrameName("ui/branchtask_icon.png");
			taskicon->setPosition(Vec2(55, 15));
			taskicon->setScale(0.8f);
			mapname->addChild(taskicon);
			map_taskicon[mname].push_back(taskicon);
		}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		mapname->setVisible(true);
#else
		if (c <= 5 && c <= GlobalInstance::getInstance()->getUnlockChapter())
		{
			mapname->setVisible(true);
		}
		else
		{
			mapname->setVisible(false);
		}
#endif
	}
	std::map<std::string, std::vector<Node*>>::iterator it;
	for (it = map_taskicon.begin(); it != map_taskicon.end(); it++)
	{
		int iconsize = map_taskicon[it->first].size();
		if (iconsize == 2)
		{
			for (int i = 0; i < iconsize; i++)
			{
				map_taskicon[it->first][i]->setPositionY(38 - 45 * i);
			}
		}
	}

	for (it = map_taskicon.begin(); it != map_taskicon.end(); it++)
	{
		int iconsize = map_taskicon[it->first].size();

		for (int i = 0; i < iconsize; i++)
		{
			map_taskicon[it->first][i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.8f, Vec2(-5, 0)), MoveBy::create(0.8f, Vec2(5, 0)), NULL)));
		}
	}

	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_MAINMAP);

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

	HttpDataSwap::init(this)->getServerTime();

	return true;
}

void MainMapScene::showNewerGuide(int step, Node* node)
{
	std::vector<Node*> nodes;
	if (step == 53)
	{
		nodes.push_back(node);
	}
	showNewerGuideNode(step, nodes);
}

void MainMapScene::showNewerGuideNode(int step, std::vector<Node*> nodes)
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

void MainMapScene::onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	Node* selnode = (Node*)clicknode->getUserData();

	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		selnode->setVisible(true);
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		selnode->setVisible(false);
		std::string mapname = clicknode->getParent()->getName();
		if (m_isDraging)
			return;

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		if (mapname.compare("m0-1") == 0)
		{
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
		}
		else
		{
			Layer* layer = SelectSubMapLayer::create(mapname);
			addChild(layer);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		selnode->setVisible(false);
		break;
	default:
		break;
	}
}

void MainMapScene::updateTime(float dt)
{
	GlobalInstance::servertime++;
}

void MainMapScene::onFinish(int code)
{
	if (code == SUCCESS)
	{
		updateTime(0);
		this->schedule(schedule_selector(MainMapScene::updateTime), 1);

		for (int i = 0; i < 6; i++)
		{
			Hero* myhero = GlobalInstance::myCardHeros[i];
			if (myhero != NULL && myhero->getPower().getValue() < 100)
			{
				int pasttime = GlobalInstance::servertime - myhero->getPowerTime();

				DynamicValueInt dv;
				int count = pasttime / HEROPOWER_RESETTIME;
				if (count > 0)
				{
					myhero->setPowerTime(GlobalInstance::servertime);
					if (count + myhero->getPower().getValue() > 100)
						count = 100;
					else
						count += myhero->getPower().getValue();
					dv.setValue(count);

					myhero->setPower(dv);
					GlobalInstance::getInstance()->saveHero(myhero);
				}
			}
		}

	}
}