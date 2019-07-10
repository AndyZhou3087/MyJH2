#include "MainMapScene.h"
#include "Resource.h"
#include "SelectSubMapLayer.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MainScene.h"
#include "SoundManager.h"
#include "NewGuideLayer.h"
#include "DataSave.h"
#include "MyTransitionScene.h"
#include "M1_5_BoxLayer.h"
#include "MyRes.h"
#include "CommonFuncs.h"
#include "AnimationEffect.h"
#include "ErrorHintLayer.h"

MainMapScene* g_MainMapScene = NULL;
MainMapScene::MainMapScene()
{
	m_isDraging = false;

	int z002count = MyRes::getMyResCount("z002", MYPACKAGE);
	if (z002count > 0)
		MyRes::Use("z002", z002count, MYPACKAGE);
}


MainMapScene::~MainMapScene()
{
	g_MainMapScene = NULL;
}

Scene* MainMapScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	g_MainMapScene = MainMapScene::create();

	// add layer as a child to scene
	scene->addChild(g_MainMapScene);

	// return the scene
	return scene;
}

void MainMapScene::onExit()
{
	Layer::onExit();
}

bool MainMapScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mainMapLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ScrollView* mapscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("mapscroll");
	mapscroll->setScrollBarEnabled(false);
	mapscroll->setSwallowTouches(false);
	mapscroll->jumpToPercentHorizontal(100);

	cocos2d::ui::Widget* mapbg = (cocos2d::ui::Widget*)mapscroll->getChildByName("mapbg");
	mapbg->setScale(0.8f);
	int mapnamecount = mapbg->getChildrenCount();

	cocos2d::ui::Widget* m1_5_box = (cocos2d::ui::Widget*)mapbg->getChildByName("m1-5")->getChildByName("box");
	m1_5_box->addTouchEventListener(CC_CALLBACK_2(MainMapScene::onBoxclick, this));
	m1_5_box->setSwallowTouches(false);

	int curc = atoi(GlobalInstance::myCurMainData.place.substr(1, GlobalInstance::myCurMainData.place.find_first_of("-") - 1).c_str());

	if (curc > GlobalInstance::getInstance()->getUnlockChapter())
		GlobalInstance::getInstance()->saveUnlockChapter(curc);

	std::map<std::string, std::vector<Node*>> map_taskicon;

	for (int i = 0; i < mapnamecount; i++)
	{
		cocos2d::ui::Widget* mapname = (cocos2d::ui::Widget*)mapbg->getChildren().at(i);
		cocos2d::ui::Widget* normal = (cocos2d::ui::Widget*)mapname->getChildByName("normal");
		cocos2d::ui::Widget* select = (cocos2d::ui::Widget*)mapname->getChildByName("select");
		cocos2d::ui::Widget* click = (cocos2d::ui::Widget*)mapname->getChildByName("click");
		select->setVisible(false);

		normal->setEnabled(false);
		//normal->setSwallowTouches(false);
		//normal->setUserData((void*)select);
		//normal->addTouchEventListener(CC_CALLBACK_2(MainMapScene::onclick, this));

		click->setUserData((void*)select);
		click->setSwallowTouches(false);
		click->addTouchEventListener(CC_CALLBACK_2(MainMapScene::onclick, this));

		std::string mname = mapname->getName();
		int c = atoi(mname.substr(1, mname.find_first_of("-") - 1).c_str());

		std::string mapnameid = GlobalInstance::myCurMainData.place.substr(0, GlobalInstance::myCurMainData.place.find_last_of("-"));
		if ((mapnameid.compare(mname) == 0 && GlobalInstance::myCurMainData.isfinish == QUEST_ACC) || (GlobalInstance::myCurMainData.isfinish == QUEST_TASK && mname.compare("m0-1") == 0))
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
		checkShowFlag(mapname, mname);
//#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//		mapname->setVisible(true);
#else
		if (c <= 6 && c <= GlobalInstance::getInstance()->getUnlockChapter())
		{
			mapname->setVisible(true);
			checkShowFlag(mapname, mname);
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

    //记录位置
    DataSave::getInstance()->setExitScene(1);

	DataSave::getInstance()->setHeroMapCarryCount(GlobalInstance::myOutMapCarry);
    
	return true;
}

void MainMapScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
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
	cocos2d::ui::Widget* clicknode = (cocos2d::ui::Widget*)pSender;
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
			clicknode->setEnabled(false);
#if USE_TRANSCENE
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif
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

void MainMapScene::onBoxclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Widget* clicknode = (cocos2d::ui::Widget*)pSender;
		Node* selnode = (Node*)clicknode->getUserData();

		std::string mapname = clicknode->getParent()->getName();
		if (m_isDraging)
			return;

		M1_5_BoxLayer* layer = M1_5_BoxLayer::create();
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}

void MainMapScene::updateTime(float dt)
{
	GlobalInstance::servertime++;

	int zerotime = GlobalInstance::servertime + 8 * 60 * 60;
	//议事厅每日更新
	int t = zerotime / TWENTYFOURHOURSTOSEC;
	if (t > DataSave::getInstance()->getMyFreshDate())
	{
		std::map<std::string, S_MOPUPRWDDATA>::iterator mopupit;

		for (mopupit = GlobalInstance::map_mopuprwds.begin(); mopupit != GlobalInstance::map_mopuprwds.end(); mopupit++)
		{
			std::string strkey = StringUtils::format("mopup%s", mopupit->first.c_str());
			DataSave::getInstance()->deleteDataByKey(strkey);
		}
	}
}

void MainMapScene::onFinish(int code)
{
	ErrorHintLayer* networkerrLayer = (ErrorHintLayer*)this->getChildByName("networkerrorlayer");

	if (code == SUCCESS)
	{
		if (networkerrLayer != NULL)
			networkerrLayer->removeFromParentAndCleanup(true);

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

void MainMapScene::checkShowFlag(Node* mapnode, std::string mainmapid)
{
	if (mainmapid.compare("m1-5") != 0)
	{
		int submapcount = 0;
		int getstarcount = 0;
		std::map<std::string, S_SubMap>::iterator it;
		for (it = GlobalInstance::map_mapsdata[mainmapid].map_sublist.begin(); it != GlobalInstance::map_mapsdata[mainmapid].map_sublist.end(); it++)
		{
			submapcount++;
			std::vector<std::string> vec_finishstar;

			CommonFuncs::split(DataSave::getInstance()->getFinishStar(it->first), vec_finishstar, ",");
			getstarcount += vec_finishstar.size();
		}

		if (getstarcount > 0 && getstarcount >= submapcount * 3)
		{
			Sprite* flag = Sprite::createWithSpriteFrameName("ui/finishstarflag1.png");
			flag->setAnchorPoint(Vec2(0, 0.5f));
			
			flag->setPosition(Vec2(mapnode->getChildByName("normal")->getPositionX() - mapnode->getChildByName("normal")->getContentSize().width/2, mapnode->getChildByName("normal")->getPositionY() + mapnode->getChildByName("normal")->getContentSize().height/2));
			mapnode->addChild(flag);

			auto animation = Animation::create();
			for (int i = 1; i <= 3; i++)
			{
				std::string framename = StringUtils::format("ui/finishstarflag%d.png", i);
				animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(framename));
			}
			//设置帧动画属性
			animation->setDelayPerUnit(0.2f);//每一帧停留的时间
			animation->setRestoreOriginalFrame(true);//播放完后回到第一帧
			auto animate = Animate::create(animation);
			flag->runAction(RepeatForever::create(animate));
		}
	}
}
