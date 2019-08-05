#include "MapBlockScene.h"
#include "Resource.h"
#include "MyRes.h"
#include "CommonFuncs.h"
#include "tinyxml2/tinyxml2.h"
#include "MainScene.h"
#include "Const.h"
#include "MainMapScene.h"
#include "MovingLabel.h"
#include "MapEventLayer.h"
#include "DataSave.h"
#include "FightingLayer.h"
#include "FightingResultLayer.h"
#include "Quest.h"
#include "TaskTalkLayer.h"
#include "MyPackageLayer.h"
#include "MainScene.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "HintBoxLayer.h"
#include "NewPopLayer.h"
#include "CutScenesLayer.h"
#include "GoBackLayer.h"
#include "NewGuideLayer.h"
#include "RewardLayer.h"
#include "Utility.h"
#include "NpcLayer.h"
#include "MyTransitionScene.h"
#include "BuyResLayer.h"
#include "UsePropLayer.h"
#include "MazeTransitionScene.h"
#include "MazeDescLayer.h"
#include "BuySelectLayer.h"
#include "StarResultLayer.h"
#include "CannotTouchLayer.h"
#include "MapZoomGuideLayer.h"
#include "SupperBossRankLayer.h"
#include "ErrorHintLayer.h"
#include "WaitingProgress.h"

MapBlockScene* g_MapBlockScene = NULL;

#define DEFAULTRND 10
#define HEROOFFSET_Y 20
#define TORCHSCALE 5
std::string walkname[] = {"walk_up", "walk_d", "walk_l", "walk_l"};
std::string standname[] = { "stand_up", "stand_d", "stand_l", "stand_l" };
MapBlockScene::MapBlockScene()
{
	myposHero = NULL;
	isMoving = false;

	walkcount = 0;
	monsterComeRnd = DEFAULTRND;
	fogscale = 5;
	m_fightbgtype = 0;
	m_walkDirection = 0;
	m_lastWalkDirection = 0;
	_fogrender = NULL;
	mapIsAllOpen = false;
	randStartPos = -1;
	firstpostype = -1;
	isNewerGuideMap = false;
	usefood = 1;
	usingprop = -1;
	isDraging = false;
	isMaze = false;
	buildfocus = NULL;
	mapAllOpenFog = NULL;

	totalBoxcount = 0;

	totalbosscount = 0;

	astarrouting = NULL;

	iscfgmazeentry = false;

	isMovingRouting = false;

	isRoutingBreakOff = false;

	destblockindex = -1;

	isoverscreen = false;
    
    ishalfscreen = false;

	guideBossPos = -1;
}


MapBlockScene::~MapBlockScene()
{
	GlobalInstance::getInstance()->recoveCardHeroMaxHp();
	g_MapBlockScene = NULL;

	if (astarrouting != NULL)
	{
		delete astarrouting;
		astarrouting = NULL;
	}

}

Scene* MapBlockScene::createScene(std::string mapname, int bgtype)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	g_MapBlockScene = MapBlockScene::create(mapname, bgtype);

	// add layer as a child to scene
	scene->addChild(g_MapBlockScene);

	// return the scene
	return scene;
}

MapBlockScene* MapBlockScene::create(std::string mapname, int bgtype)
{
	MapBlockScene *pRet = new(std::nothrow)MapBlockScene();
	if (pRet && pRet->init(mapname, bgtype))
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

void MapBlockScene::onExit()
{
	if (!mapIsAllOpen)
	{
		std::string str;
		std::map<int, MapBlock*>::iterator it;
		for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
		{
			std::string onestr = StringUtils::format("%d,", map_mapBlocks[it->first]->getIsCanSee() ? 1 : 0);
			str.append(onestr);
		}
		str = str.substr(0, str.length() - 1);
		std::string startstr = StringUtils::format(";%d", randStartPos);
		str.append(startstr);
		DataSave::getInstance()->setMapVisibleArea(m_mapid, str);
	}

	Layer::onExit();
}

bool MapBlockScene::init(std::string mapname, int bgtype)
{
	m_mapid = mapname;

	if (m_mapid.compare(0, 2, "mg") == 0)
		isMaze = true;
	
	m_fightbgtype = bgtype;

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("mapBlockLayer.csb"));
	this->addChild(m_csbnode);

	std::string mapdisplayname = GlobalInstance::map_AllResources[mapname].name;
	if (isMaze)
	{
		mapdisplayname = ResourceLang::map_lang["mazename"];
		//mapdisplayname.append(mapname);
	}
	std::u32string utf32String;
	StringUtils::UTF8ToUTF32(mapdisplayname, utf32String);

	int u32strlen = utf32String.length();
	cocos2d::ui::ImageView* mapnamebox = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("mapnamebox");
	mapnamelbl = (cocos2d::ui::Text*)m_csbnode->getChildByName("mapname");
	mapnamebox->setContentSize(Size(70 + (mapnamelbl->getFontSize() + 1) * u32strlen, mapnamebox->getContentSize().height));
	mapnamelbl->setString(mapdisplayname);

	std::string mainid = mapname.substr(0, mapname.find_last_of("-"));
	if (GlobalInstance::map_mapsdata.find(mainid) != GlobalInstance::map_mapsdata.end())
	{
		S_SubMap submap = GlobalInstance::map_mapsdata[mainid].map_sublist[mapname];
		usefood = submap.pf;
	}

	Node* topnode = m_csbnode->getChildByName("mapblocktop");

	carrycountlbl = (cocos2d::ui::Text*)topnode->getChildByName("carrycount");
	foodcountlbl = (cocos2d::ui::Text*)topnode->getChildByName("r001count");
	solivercountlbl = (cocos2d::ui::Text*)topnode->getChildByName("solivercountlbl");

	sitelbl = (cocos2d::ui::Text*)topnode->getChildByName("site");

	sitetext = (cocos2d::ui::Text*)topnode->getChildByName("sitetext");

	int vsionLv = MyRes::getMyResCount("v001", MYSTORAGE);

	//fogscale += vsionLv;

	resetBlockData();

	parseMapXml(mapname);

	createFog();

	setBlockRange();

	if (isMaze)
	{
		MAZE_POS mdata;
		max_mazepos = 1;
		for (unsigned int i = 0; i < vec_mazetranspoints.size(); i++)
		{
			int m = vec_mazetranspoints[i].maid;
			if (m > max_mazepos)
				max_mazepos = m;
		}
		for (unsigned int i = 0; i < vec_mazetranspoints.size(); i++)
		{
			int m = vec_mazetranspoints[i].maid;
			int b = vec_mazetranspoints[i].blockindex;
			if (m >= max_mazepos)
			{
				mdata.maid = m;
				mdata.blockindex = b;
				vec_mazetranspoints.erase(vec_mazetranspoints.begin() + i);
				break;
			}
		}
		std::random_shuffle(vec_mazetranspoints.begin(), vec_mazetranspoints.end());

		int randcount = 0;
		if (vec_mazetranspoints.size() >= 4)
			randcount = 4;
		else
			randcount = vec_mazetranspoints.size();

		int r1 = GlobalInstance::getInstance()->createRandomNum(randcount) + 1;

		for (unsigned int i = 0; i < vec_mazetranspoints.size(); i++)
		{
			if (vec_mazetranspoints[i].maid == r1)
			{
				int swapmaid = vec_mazetranspoints[0].maid;
				int swapb = vec_mazetranspoints[0].blockindex;
				vec_mazetranspoints[0].maid = vec_mazetranspoints[i].maid;
				vec_mazetranspoints[0].blockindex = vec_mazetranspoints[i].blockindex;
				vec_mazetranspoints[i].maid = swapmaid;
				vec_mazetranspoints[i].blockindex = swapb;
				break;
			}
		}

		vec_mazetranspoints.push_back(mdata);

		randStartPos = vec_mazetranspoints[0].blockindex;

		scrollView->setMinScale(1);
		scrollView->setMaxScale(1);
		//scrollView->setTouchEnabled(false);

		buildfocus = cocos2d::ui::ImageView::create("mapui/focusbuildblock_8.png", cocos2d::ui::Widget::TextureResType::PLIST);
		buildfocus->ignoreContentAdaptWithSize(true);
		buildfocus->setPosition(Vec2(0, 0));
		buildfocus->setTouchEnabled(true);
		buildfocus->setVisible(false);
		buildfocus->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onUsePropClick, this));
		scrollView->addChild(buildfocus, 19998);

		//cocos2d::ui::ImageView* z002box = cocos2d::ui::ImageView::create("mapui/mapzbox.png", cocos2d::ui::Widget::TextureResType::PLIST);
		//z002box->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onUsePropClick, this));
		//z002box->setPosition(Vec2(buildfocus->getContentSize().width, buildfocus->getContentSize().height));
		//buildfocus->addChild(z002box);

		cocos2d::ui::ImageView* z002 = cocos2d::ui::ImageView::create("ui/z002.png", cocos2d::ui::Widget::TextureResType::PLIST);
		z002->setTouchEnabled(true);
		//z002->setPosition(Vec2(z002box->getContentSize().width / 2, z002box->getContentSize().height / 2 + 8));
		z002->setScale(0.6f);
		z002->setAnchorPoint(Vec2(0.7f, 0.25f));
		//z002box->addChild(z002);
		z002->setPosition(Vec2(buildfocus->getContentSize().width, buildfocus->getContentSize().height - 20));
		z002->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onUsePropClick, this));
		buildfocus->addChild(z002, 0, "z002");

		z002countlbl = Label::createWithTTF("", FONT_NAME, 23);
		z002countlbl->setColor(Color3B(34, 74, 79));
		z002countlbl->setPosition(Vec2(buildfocus->getContentSize().width + 5, buildfocus->getContentSize().height));
		buildfocus->addChild(z002countlbl);
	}
	else
	{
		int count = vec_startpos.size();

		if (randStartPos < 0)
		{
			if (GlobalInstance::map_randstartpos >= 0)
			{
				randStartPos = GlobalInstance::map_randstartpos;
				GlobalInstance::map_randstartpos = -1;
			}
			else
			{
				int startposindex = GlobalInstance::getInstance()->createRandomNum(count);
				randStartPos = vec_startpos[startposindex];
			}
		}

		map_mapBlocks[randStartPos]->setPosIcon();

		for (int i = 0; i < count; i++)
		{
			if (randStartPos != vec_startpos[i])
			{
				map_mapBlocks[vec_startpos[i]]->setPosType(-1);
			}
		}
	}



	mycurCol = randStartPos % blockColCount;
	mycurRow = randStartPos / blockColCount;

	if (!isMaze)
	{
		if (MapBlock::vec_randMonsters.size() > 0)
		{
			std::map<int, MapBlock*>::iterator it;

			for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
			{
				MapBlock* block = map_mapBlocks[it->first];
				if (block->getPosType() == POS_NOTHING && block->getWalkable() && block->map_eventrnd.size() <= 0)
					vec_normalBlocks.push_back(block);
			}

			std::random_shuffle(vec_normalBlocks.begin(), vec_normalBlocks.end());

			int tcount = vec_normalBlocks.size() / 4;

			int mcount = 0;
			std::vector<MapBlock*>::iterator mit;

			for (mit = vec_normalBlocks.begin(); mit != vec_normalBlocks.end();)
			{

				if (mcount >= tcount)
					break;

				bool ret = true;

				for (unsigned int i = 0; i < vec_monsterBlocks.size(); i++)
				{
					int off = abs((*mit)->Row - vec_monsterBlocks[i]->Row) + abs((*mit)->Col - vec_monsterBlocks[i]->Col);
					if (off <= 4)
					{
						ret = false;
						break;
					}
				}
				if (ret)
				{
					vec_monsterBlocks.push_back(*mit);
					mcount++;
				}
				mit = vec_normalBlocks.erase(mit);

			}

			createRndMonsters();

			for (unsigned int i = 0; i < vec_monsterBlocks.size(); i++)
			{
				vec_monsterBlocks[i]->setPosType(POS_MONSTER);

				int mbindex = vec_monsterBlocks[i]->getTag();
				if (map_allRndMonsters[mbindex].size() > 0)
				{
					std::string firstmid = map_allRndMonsters[mbindex].at(0)->getId();
					vec_monsterBlocks[i]->setPosNpcID(GlobalInstance::map_Npcs[firstmid].icon);
					vec_monsterBlocks[i]->setPosIcon();
				}
			}
		}

		//迷宫地图记下的位置
		if (GlobalInstance::eventstartmappos >= 0)
		{
			MapBlock* startymapblock = map_mapBlocks[GlobalInstance::eventstartmappos];
			std::map<int, int>::iterator rnd_it;
			for (rnd_it = startymapblock->map_eventrnd.begin(); rnd_it != startymapblock->map_eventrnd.end(); rnd_it++)
			{
				startymapblock->map_eventrnd[rnd_it->first] = 0;
			}

			mycurCol = GlobalInstance::eventstartmappos % blockColCount;
			mycurRow = GlobalInstance::eventstartmappos / blockColCount;
			GlobalInstance::eventstartmappos = -1;
			GlobalInstance::ishasmazeentry = false;
			GlobalInstance::showz002hinttextcount = 0;
		}
	}

	setMyPos();

	Node* bottomnode = m_csbnode->getChildByName("mapblockbottom");

	gocitybtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("gocitybtn");
	gocitybtn->setTag(BTN_GOCITY);
	gocitybtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));
	gocitybtn->setVisible(!isMaze);

	cocos2d::ui::Widget* exitmazebtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("exitmazebtn");
	exitmazebtn->setTag(BTN_EXITMAZE);
	exitmazebtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));
	exitmazebtn->setVisible(isMaze);

	mypackagebtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("packagebtn");
	mypackagebtn->setTag(BTN_PACKAGE);
	mypackagebtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));

	torchbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("torchbtn");
	torchbtn->setTag(BTN_TORCH);
	torchbtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));
	torchbtn->setVisible(!isMaze);

	visionbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("visionbtn");
	visionbtn->setTag(BTN_VISION);
	visionbtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));
	visionbtn->setVisible(!isMaze);

	visioncountlbl = (cocos2d::ui::TextBMFont*)visionbtn->getChildByName("countlbl");
	torchcountlbl = (cocos2d::ui::TextBMFont*)torchbtn->getChildByName("countlbl");

	cocos2d::ui::Widget* allopenbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("allopenbtn");
	allopenbtn->setTag(BTN_ALLOPEN);
	allopenbtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));
	allopenbtn->setVisible(!isMaze);

	transbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("transerbtn");
	transbtn->setTag(BTN_TRANS);
	transbtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));
	transbtn->setVisible(!isMaze);

	if (GlobalInstance::challangeType == CH_SUPERBOSS)
	{
		gocitybtn->setVisible(true);
		exitmazebtn->setVisible(false);
		mypackagebtn->setVisible(false);
		torchbtn->setVisible(false);
		visionbtn->setVisible(false);
		allopenbtn->setVisible(false);
		transbtn->setVisible(false);
	}

	updateLabel(0);
	this->schedule(schedule_selector(MapBlockScene::updateLabel), 0.5f);

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(60 + 120 * i, 1196);
		fightHeroNode->setScale(0.84f);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i], F_HERO, FS_READY);
		addChild(fightHeroNode, 0, i);
	}

	//scheduleUpdate();
	if (NewGuideLayer::checkifNewerGuide(1))
	{
		isNewerGuideMap = true;
	}

	if (!isMaze)
	{
		if (!(GlobalInstance::challangeType == CH_SUPERBOSS))
		{
			loadTaskUI();
			if (isNewerGuideMap)
			{
				m_tasknode->stopAllActions();
				m_tasknode->setPosition(Vec2(985, m_tasknode->getPositionY()));
				taskclick->setVisible(true);
			}
			else
			{
				this->scheduleOnce(schedule_selector(MapBlockScene::closeTaskTipNode), 7.0f);
			}
		}
	}
	else
	{
		Node* z002node = m_csbnode->getChildByName("z002node");
		z002node->setVisible(true);
		z002countlbl_1 = (cocos2d::ui::Text*)m_csbnode->getChildByName("z002node")->getChildByName("coutlbl");
		this->schedule(schedule_selector(MapBlockScene::updateZ002Count), 1.0f);
	}

	int r = GlobalInstance::getInstance()->createRandomNum(5);
	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_SUBMAP_0 + r);

	DataSave::getInstance()->setHeroMapCarryCount(GlobalInstance::myOutMapCarry);



	astarrouting = new AstarRouting(blockColCount, blockRowCount);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		isDraging = false;
		m_startClickX = touch->getLocation().x;
		m_startClickY = touch->getLocation().y;
		return true;
	};

	listener->onTouchMoved = [=](Touch *touch, Event *event)
	{
		if (fabsf(m_startClickX - touch->getLocation().x) > CLICKOFFSETP || fabsf(m_startClickY - touch->getLocation().y) > CLICKOFFSETP)
		{
			isDraging = true;
		}
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void MapBlockScene::updateZ002Count(float dt)
{
	std::string zcountstr = StringUtils::format("%d", MyRes::getMyResCount("z002", MYPACKAGE) + MyRes::getMyResCount("z002", MYSTORAGE));
	z002countlbl_1->setString(zcountstr);

	if (buildfocus->isVisible())
	{
		z002countlbl->setString(zcountstr);
	}
}

void MapBlockScene::setBtnEnable(bool isval)
{
	gocitybtn->setEnabled(isval);
	torchbtn->setEnabled(isval);
	mypackagebtn->setEnabled(isval);
	visionbtn->setEnabled(isval);
}

void MapBlockScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	if (isMaze && GlobalInstance::mazerouteindex == 0)
	{
		MazeDescLayer* layer = MazeDescLayer::create();
		this->addChild(layer, 10000);
	}

	if (GlobalInstance::ishasmazeentry && iscfgmazeentry)
	{
		this->scheduleOnce(schedule_selector(MapBlockScene::delayShowMazeHint), 1.0f);
	}

	if (NewGuideLayer::checkifNewerGuide(0))
	{
		MapZoomGuideLayer* layer = MapZoomGuideLayer::create();
		this->addChild(layer);
	}
}

void MapBlockScene::delayShowMazeHint(float dt)
{
	MovingLabel::show(ResourceLang::map_lang["mazeisopen"]);
}

void MapBlockScene::showNewerGuideFight()
{
	showFightingLayer(vec_enemys);
}

void MapBlockScene::delayShowNewerGuide(float dt)
{
	if (NewGuideLayer::checkifNewerGuide(1))
	{
		showNewerGuide(1);
	}

	else if (getFirstFightBoss() && NewGuideLayer::checkifNewerGuide(86))
	{
		showNewerGuide(86);
	}
	setBtnEnable(true);
	this->removeChildByName("cannottouchlayer");
}

void MapBlockScene::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 1)
	{
		//find boss
		std::map<int, MapBlock*>::iterator it;

		for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
		{
			MapBlock* block = map_mapBlocks[it->first];
			if (block->getPosType() == POS_BOSS)
			{
				showBossGuideAnim(block->getPosition());
				nodes.push_back(block);
				guideBossPos = it->first;
				break;
			}
		}
	}
	if (step == 86)
	{
		nodes.push_back(gocitybtn);
	}
	showNewerGuideNode(step, nodes);
}

void MapBlockScene::showNewerGuideNode(int step, std::vector<Node*> nodes)
{
	if (NewGuideLayer::checkifNewerGuide(step))
	{
		if (g_NewGuideLayer != NULL)
		{
			g_NewGuideLayer->removeFromParentAndCleanup(true);
		}
		if (g_NewGuideLayer == NULL)
		{
			g_NewGuideLayer = NewGuideLayer::create(step, nodes);
			this->addChild(g_NewGuideLayer, 10);
		}
	}
}

bool MapBlockScene::getIsMoving()
{
	return isMoving;
}

void MapBlockScene::showNewerGuideGoBack()
{
	setBtnEnable(false);

	if (NewGuideLayer::checkifNewerGuide(2))
	{
#if USE_TRANSCENE
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif
	}
}

void MapBlockScene::showFightingLayer(std::vector<Npc*> enemys)
{
	CutScenesLayer* cutlayer = CutScenesLayer::create(enemys, m_fightbgtype);
	this->addChild(cutlayer, 0, "CutScenesLayer");
}

void MapBlockScene::loadTaskUI()
{
	int langtype = GlobalInstance::getInstance()->getLang();
	//添加任务提示框
	m_tasknode = CSLoader::createNode(ResourcePath::makePath("taskTipsNode.csb"));
	m_csbnode->addChild(m_tasknode, -1);
	m_tasknode->setPosition(Vec2(458, 992));

	cocos2d::ui::ImageView* tasktitle = (cocos2d::ui::ImageView*)m_tasknode->getChildByName("title");
	tasktitle->loadTexture(ResourcePath::makeTextImgPath("maptask_title", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* bgclick = (cocos2d::ui::ImageView*)m_tasknode->getChildByName("bg");
	bgclick->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onTaskAction, this));
	bgclick->setTag(1);
	taskclick = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("taskclick");
	taskclick->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onTaskAction, this));
	taskclick->setTag(0);
	taskclick->setVisible(false);

	textmain = (cocos2d::ui::Text*)m_tasknode->getChildByName("textmain");
	textbranch = (cocos2d::ui::Text*)m_tasknode->getChildByName("textbranch");
	updateTaskInfo(0);
	this->schedule(schedule_selector(MapBlockScene::updateTaskInfo), 1.0f);
}

void MapBlockScene::updateTaskInfo(float dt)
{
	//主线
	if (Quest::getMainQuest())
	{
		if (textmain->getTag() != 1)
		{
			textmain->setTag(1);
			std::string mainid = GlobalInstance::myCurMainData.place.substr(0, GlobalInstance::myCurMainData.place.find_last_of("-"));
			S_SubMap submap = GlobalInstance::map_mapsdata[mainid].map_sublist[GlobalInstance::myCurMainData.place];
			std::string str = StringUtils::format(ResourceLang::map_lang["tasktips"].c_str(), GlobalInstance::myCurMainData.name.c_str(), GlobalInstance::map_AllResources[submap.id].name.c_str(), GlobalInstance::map_AllResources[GlobalInstance::myCurMainData.npcid].name.c_str());
			textmain->setString(str);
			changeTaskTipTextColor(0);
		}
	}
	else
	{
		if (GlobalInstance::myCurMainData.id == 1)
		{
			textmain->setString(ResourceLang::map_lang["nottasktext"]);
		}
		else
		{
			textmain->setString(ResourceLang::map_lang["finishtasktext"]);
			textmain->setTextColor(Color4B(255, 0, 0, 255));

			std::u32string utf32lblString;
			StringUtils::UTF8ToUTF32(textmain->getString(), utf32lblString);

			for (unsigned int m = 0; m < utf32lblString.length(); m++)
			{
				Sprite* letter = textmain->getLetter(m);
				if (letter != NULL)
				{
					letter->setColor(Color3B(255, 0, 0));
				}
			}
		}
	}

	//支线
	if (Quest::getBranchQuest())
	{
		if (textbranch->getTag() != 1)
		{
			textbranch->setTag(1);
			std::string mainid = GlobalInstance::myCurBranchData.place.substr(0, GlobalInstance::myCurBranchData.place.find_last_of("-"));
			S_SubMap submap = GlobalInstance::map_mapsdata[mainid].map_sublist[GlobalInstance::myCurBranchData.place];
			std::string str = StringUtils::format(ResourceLang::map_lang["tasktips"].c_str(), GlobalInstance::myCurBranchData.name.c_str(), GlobalInstance::map_AllResources[submap.id].name.c_str(), GlobalInstance::map_AllResources[GlobalInstance::myCurBranchData.npcid].name.c_str());
			textbranch->setString(str);
			changeTaskTipTextColor(1);
		}
	}
	else
	{
		if (GlobalInstance::myCurBranchData.id == 1)
		{
			textbranch->setString(ResourceLang::map_lang["nottasktext"]);
			textbranch->setVisible(false);
			cocos2d::ui::Widget* maptask_branch = (cocos2d::ui::Widget*)m_tasknode->getChildByName("maptask_branch_4");
			maptask_branch->setVisible(false);
		}
		else
		{

			textbranch->setString(ResourceLang::map_lang["finishtasktext"]);
			textbranch->setTextColor(Color4B(255, 0, 0, 255));

			std::u32string utf32lblString;
			StringUtils::UTF8ToUTF32(textbranch->getString(), utf32lblString);

			for (unsigned int m = 0; m < utf32lblString.length(); m++)
			{
				Sprite* letter = textbranch->getLetter(m);
				if (letter != NULL)
				{
					letter->setColor(Color3B(255, 0, 0));
				}
			}

		}
	}
}

void MapBlockScene::changeTaskTipTextColor(int type)
{
	Label* lbl;
	std::string lbluft8str;
	Color3B changecolor;

	if (type == 0)
	{
		lbluft8str = textmain->getString();
		lbl = (Label*)textmain->getVirtualRenderer();
		changecolor = Color3B(255, 165, 0);
	}
	else
	{
		lbluft8str = textbranch->getString();
		lbl = (Label*)textbranch->getVirtualRenderer();
		changecolor = Color3B(16, 252, 16);
	}

	std::u32string utf32enMaohaoString;
	StringUtils::UTF8ToUTF32(":", utf32enMaohaoString);

	std::string prefkey[] = { "m", "n" };
	std::map<std::string, AllResources>::iterator it;
	for (it = GlobalInstance::map_AllResources.begin(); it != GlobalInstance::map_AllResources.end(); it++)
	{
		std::string strkey = it->first;

		for (int i = 0; i < 2; i++)
		{
			if (strkey.compare(0, 1, prefkey[i]) == 0)
			{
				std::string namestr = GlobalInstance::map_AllResources[strkey].name;
				
				std::u32string utf32nameString;
				StringUtils::UTF8ToUTF32(namestr, utf32nameString);

				std::u32string utf32lblString;
				StringUtils::UTF8ToUTF32(lbluft8str, utf32lblString);

				size_t f1 = utf32lblString.find(utf32enMaohaoString);
				if (f1 == std::string::npos)
					f1 = 0;

				size_t findpos = utf32lblString.find(utf32nameString, f1);
				if (findpos != std::string::npos)
				{
					for (unsigned int m = findpos; m < (findpos + utf32nameString.length()); m++)
					{
						lbl->getLetter(m)->setColor(changecolor);
					}
					break;
				}
			}
		}
	}
}

void MapBlockScene::updateLabel(float dt)
{
	int foodcount = MyRes::getMyResCount("r001", MYPACKAGE);
	std::string str = StringUtils::format("%d/%d", MyRes::getMyPackageCount(), GlobalInstance::myOutMapCarry/*GlobalInstance::getInstance()->getTotalCarry()*/);
	carrycountlbl->setString(str);
	str = StringUtils::format("%d", foodcount);
	foodcountlbl->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	solivercountlbl->setString(str);

	if (MyRes::getMyPackageCount() >= GlobalInstance::myOutMapCarry)
	{
		carrycountlbl->setTextColor(Color4B(255, 61, 61, 255));
	}
	else
	{
		carrycountlbl->setTextColor(Color4B(255, 255, 255, 255));
	}
	std::string countstr = StringUtils::format("%d", MyRes::getMyResCount("v001"));
	visioncountlbl->setString(countstr);

	countstr = StringUtils::format("%d", MyRes::getMyResCount("z001"));
	torchcountlbl->setString(countstr);
}

void MapBlockScene::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case BTN_GOCITY:
		{
			GoBackLayer* layer = GoBackLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case BTN_TORCH:
		{
			if (usingprop >= MAP_USEINGTORCH)
			{
				MovingLabel::show(ResourceLang::map_lang["mapusingprop"]);
				return;
			}

			if (MyRes::getMyResCount("z001") <= 0)
			{
				std::vector< MSGAWDSDATA> vec_res;
				MSGAWDSDATA rdata;
				rdata.rid = "z001";
				rdata.count = 5;
				rdata.qu = 0;
				vec_res.push_back(rdata);
				BuyResLayer* layer = BuyResLayer::create(vec_res);
				this->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["usetorchdesc"]);
				usingprop = MAP_USEINGTORCH;

				Node* focus = torchbtn->getChildByName("focus");
				focus->setVisible(true);
				focus->runAction(RepeatForever::create(Sequence::create(FadeOut::create(0.8f), FadeIn::create(0.8f), NULL)));
			}
		}
			break;
		case BTN_PACKAGE:
		{
			MyPackageLayer* layer = MyPackageLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		break;
		case BTN_VISION:
		{
			if (MyRes::getMyResCount("v001") <= 0)
			{
				std::vector< MSGAWDSDATA> vec_res;
				MSGAWDSDATA rdata;
				rdata.rid = "v001";
				rdata.count = 5;
				rdata.qu = 0;
				vec_res.push_back(rdata);
				BuyResLayer* layer = BuyResLayer::create(vec_res);
				this->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			else if (fogscale == 10)
			{
				MovingLabel::show(ResourceLang::map_lang["vsionmax"]);
			}
			else
			{
				MyRes::Use("v001");
				fogscale += 1;
				_mylight->setScale(fogscale);
				if (_fogrender != NULL)
				{
					std::string fogname = StringUtils::format("fog%d", mycurRow*blockColCount + mycurCol);
					_fogrender->removeChildByName(fogname);
					_fogrender->begin();
					addFogBlock(mycurRow, mycurCol, fogscale);
					_fogrender->end();
					Director::getInstance()->getRenderer()->render();
				}
				GlobalInstance::getInstance()->usePropsCount(COSTPROP_VISION, 1);
			}
		}
		break;
		case BTN_EXITMAZE:
		{
			std::string str = StringUtils::format(ResourceLang::map_lang["exitmaze"].c_str(), 20);
			HintBoxLayer* hlayer = HintBoxLayer::create(str, 13);
			this->addChild(hlayer);
			AnimationEffect::openAniEffect(hlayer);
		}
			break;
		case BTN_ALLOPEN:
		{
			if (mapIsAllOpen)
			{
				std::string str = StringUtils::format(ResourceLang::map_lang["isopenall"].c_str(), GlobalInstance::map_AllResources[m_mapid].name.c_str());
				MovingLabel::show(str);
				return;
			}

			UsePropLayer* layer = UsePropLayer::create("z003", 1);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		break;
		case BTN_TRANS:
		{
			if (usingprop >= MAP_USEINGTORCH)
			{
				MovingLabel::show(ResourceLang::map_lang["mapusingprop"]);
				return;
			}
			UsePropLayer* layer = UsePropLayer::create("z004", 1);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
			break;
		default:
			break;
		}
	}
}

void MapBlockScene::useAllOpen()
{
	openAllMap();
	GlobalInstance::getInstance()->usePropsCount(COSTPROP_MAPALLOPEN, 1);
}

void MapBlockScene::useTranser()
{
	Node* focus = transbtn->getChildByName("focus");
	focus->setVisible(true);
	focus->runAction(RepeatForever::create(Sequence::create(FadeOut::create(0.8f), FadeIn::create(0.8f), NULL)));

	MovingLabel::show(ResourceLang::map_lang["usetranserdesc"]);
	usingprop = MAP_USEINGTRANSER;
}

void MapBlockScene::onTaskAction(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		m_tasknode->stopAllActions();
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 0:
		{
			//taskclick->setVisible(false);
			taskclick->runAction(Sequence::create(MoveTo::create(0.05f, Vec2(730, taskclick->getPositionY())), Hide::create(), NULL));
			MoveTo* moveto = MoveTo::create(0.15f, Vec2(458, m_tasknode->getPositionY()));
			m_tasknode->runAction(moveto);
		}
			break;
		case 1:
		{
			//taskclick->setVisible(true);
			taskclick->runAction(Sequence::create(DelayTime::create(0.2f), Show::create(), MoveTo::create(0.05f, Vec2(670, taskclick->getPositionY())), NULL));
			MoveTo* moveto = MoveTo::create(0.15f, Vec2(985, m_tasknode->getPositionY()));
			m_tasknode->runAction(moveto);
		}
			break;
		default:
			break;
		}
	}
}

void MapBlockScene::closeTaskTipNode(float dt)
{
	m_tasknode->stopAllActions();
	taskclick->setVisible(true);
	MoveTo* moveto = MoveTo::create(0.15f, Vec2(985, m_tasknode->getPositionY()));
	m_tasknode->runAction(moveto);
}

void MapBlockScene::go(MAP_KEYTYPE keyArrow)
{
	if (isMoving)
		return;

	m_walkDirection = keyArrow - KEY_UP;

	if (buildfocus != NULL)
		buildfocus->setVisible(false);

	int checkret = checkRoad(keyArrow);
	if (checkret <= 0)
	{
		myposHero->setAnimation(0, standname[m_walkDirection], false);
		if (checkret == -1)
		{
			MovingLabel::show(ResourceLang::map_lang["noroad"]);
		}
		else if (checkret <= -10000)
		{
			if (GlobalInstance::showz002hinttextcount < 3)
			{
				MovingLabel::show(ResourceLang::map_lang["mazestone"]);
				GlobalInstance::showz002hinttextcount++;
			}
			
			//UsePropLayer* layer = UsePropLayer::create("z002", 1);
			//layer->setTag(-checkret - 10000);
			//this->addChild(layer, 10);
		}
		return;
	}

	switch (keyArrow)
	{
	case KEY_UP:
	{
		mycurRow += 1;
		break;
	}
	case KEY_DOWN:
	{
		mycurRow -= 1;
		break;
	}
	case KEY_LEFT:
	{
		mycurCol -= 1;
		break;
	}
	case KEY_RIGHT:
	{
		mycurCol += 1;
		break;
	}

	default:
		break;
	}

	bool isfollowme = true;
	if (!isoverscreen && !isMaze)
	{
		int mycurindex = mycurRow * blockColCount + mycurCol;
        int destrow = destblockindex/blockColCount;
        int destcol = destblockindex % blockColCount;
        int drow = abs(mycurRow - destrow);
        int dcol = abs(mycurCol - destcol);
		if (ishalfscreen && (drow == 1 ||dcol == 1))
			isfollowme = true;
		else
			isfollowme = false;
	}

	setMyPos(isfollowme);

	int usefoodcount = usefood;
	int mypackagefood = MyRes::getMyResCount("r001", MYPACKAGE);
	if (usefood >= mypackagefood)
		usefoodcount = mypackagefood;
	MyRes::Use("r001", usefoodcount, MYPACKAGE);

	if (isMaze)
	{
		int index = 0;
		int bindex = mycurRow*blockColCount + mycurCol;
		if (map_mapBlocks[bindex]->getPosType() == POS_MAZETRANS)
		{
			setBtnEnable(false);

			for (unsigned int i = 0; i < vec_mazetranspoints.size(); i++)
			{
				int b = vec_mazetranspoints[i].blockindex;
				if (b == bindex)
				{
					index = i;
					break;
				}
			}

			if (index == vec_mazetranspoints.size() - 1)
			{
				GlobalInstance::mazerouteindex++;
				if (GlobalInstance::mazerouteindex == GlobalInstance::vec_mazeroute.size())//回到当前地图
				{
					Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MazeTransitionScene::createScene(0, TO_OUT)));
					MyRes::Use("z002", MyRes::getMyResCount("z002", MYPACKAGE), MYPACKAGE);
				}
				else//继续下一个
				{
					int r = atoi(m_mapid.substr(2, m_mapid.find_first_of("-") - 2).c_str());
					Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MazeTransitionScene::createScene(r, TO_ENTER)));
				}
			}
			else
			{
				if (index == vec_mazetranspoints.size() - 2)//不是进入下一个
				{
					index = 0;
				}
				else
				{
					index++;
				}
				mycurRow = vec_mazetranspoints[index].blockindex / blockColCount;
				mycurCol = vec_mazetranspoints[index].blockindex%blockColCount;

				this->scheduleOnce(schedule_selector(MapBlockScene::heroDelayTranse), 0.5f);
			}
		}
		else
		{
			checkFood();
		}
	}
	else
	{
		walkcount++;
		monsterComeRnd += (5 + walkcount);

		if (mycurCol == randStartPos % blockColCount && mycurRow == randStartPos / blockColCount)
		{
			isRoutingBreakOff = true;
			this->scheduleOnce(schedule_selector(MapBlockScene::delayShowExit), 0.45f);
		}
		else
		{
			checkFood();
		}

		calcStar(SA_GOSTEP);
	}
}

void MapBlockScene::removeMazeStone(int blockindex)
{
	buildfocus->setEnabled(false);
	//map_mapBlocks[blockindex]->removeBuild();
	//map_mapBlocks[blockindex]->setWalkable(true);
	//map_mapBlocks[blockindex]->setPosIconVisable(true);
	Node* z002 = buildfocus->getChildByName("z002");
	z002->runAction(Sequence::create(RotateTo::create(0.1f, 80), RotateTo::create(0.1f, -20), DelayTime::create(0.2f), CallFunc::create(CC_CALLBACK_0(MapBlockScene::removeMazeStoneAfterAnim, this, blockindex)), NULL));
	//buildfocus->setVisible(false);
}

void MapBlockScene::removeMazeStoneAfterAnim(int blockindex)
{
	buildfocus->setVisible(false);
	buildfocus->setEnabled(true);
	map_mapBlocks[blockindex]->removeBuild();
	map_mapBlocks[blockindex]->setWalkable(true);
	if (map_mapBlocks[blockindex]->getPosType() > POS_START)
		map_mapBlocks[blockindex]->setPosIconVisable(true);
	Node* z002 = buildfocus->getChildByName("z002");
	z002->setRotation(0);
}

void MapBlockScene::delayShowExit(float dt)
{
	std::string hintstr = StringUtils::format(ResourceLang::map_lang["leavemaptext"].c_str(), GlobalInstance::map_AllResources[m_mapid].name.c_str());
	HintBoxLayer* hintLayer = HintBoxLayer::create(hintstr, 4);
	this->addChild(hintLayer);
	AnimationEffect::openAniEffect((Layer*)hintLayer);
}

void MapBlockScene::stopMoving()
{
	myposHero->setVisible(true);
	_mylight->setVisible(true);

	removeCurRouting(mycurRow, mycurCol);

	if (!isNewerGuideMap)
		doMyStatus();
	isMoving = false;

	myposHero->setAnimation(0, standname[m_walkDirection], false);
}

void MapBlockScene::checkFood()
{
	int foodcount = MyRes::getMyResCount("r001", MYPACKAGE);

	if (foodcount <= 10 && foodcount > 0)
	{
		if (foodcount % 5 == 0)
			MovingLabel::show(ResourceLang::map_lang["lackfoodhint"]);

		if (foodcount == 10 || foodcount == 1)
		{
			showBuySelectFood();

			if (isMovingRouting)
			{
				astarrouting->clearPathList();
				removeAllRoutingBlock();
			}
		}
	}
	else if (foodcount <= 0)
	{
		MovingLabel::show(ResourceLang::map_lang["nofoodhint"], Color4B(Color3B(255,0,0)));

		for (int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL)
			{
				float hp = GlobalInstance::myCardHeros[i]->getHp();
				hp -= GlobalInstance::myCardHeros[i]->getMaxHp()*0.2;
				if (hp <= 0)
				{
					hp = 0;
					GlobalInstance::myCardHeros[i]->setState(HS_DEAD);
					GlobalInstance::myCardHeros[i]->setPos(0);
				}
				GlobalInstance::myCardHeros[i]->setHp(hp);
				updateHeroUI(i);
				if (GlobalInstance::myCardHeros[i]->getState() == HS_DEAD)
					GlobalInstance::myCardHeros[i] = NULL;
			}
		}

		if (isMovingRouting)
		{
			astarrouting->clearPathList();
			removeAllRoutingBlock();
		}

		if (!checklive())
		{
			HintBoxLayer* layer = HintBoxLayer::create(ResourceLang::map_lang["nofooddeathhint"], 11);
			this->addChild(layer);
		}
	}
}

bool MapBlockScene::checklive()
{
	int index = 0;
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] == NULL || GlobalInstance::myCardHeros[i]->getState() == HS_DEAD)
		{
			index++;
		}
	}
	if (index == 6)
		return false;
	return true;
}

int MapBlockScene::checkRoad(MAP_KEYTYPE keyArrow)
{
	int bindex = 0;
	switch (keyArrow)
	{
		case KEY_UP:
		{
			bindex = (mycurRow + 1)*blockColCount + mycurCol;
			if (mycurRow >= blockRowCount - 1)
				return 0;
		}
		break;
		case KEY_DOWN:
		{
			bindex = (mycurRow - 1)*blockColCount + mycurCol;
			if (mycurRow <= 0)
				return 0;
		}
		break;
		case KEY_LEFT:
		{
			bindex = (mycurRow)*blockColCount + mycurCol - 1;
			if (mycurCol <= 0)
				return 0;
			myposHero->getSkeleton()->flipX = 0;
		}
		break;
		case KEY_RIGHT:
		{
			bindex = (mycurRow)*blockColCount + mycurCol + 1;
			if (mycurCol >= blockColCount - 1)
				return 0;
			myposHero->getSkeleton()->flipX = 1;
		}
		break;
		default:
			break;
	}
	if (!map_mapBlocks[bindex]->getWalkable())
	{
		std::string bname = map_mapBlocks[bindex]->getBuildName();
		if (isMaze && (bname.compare("8.png") == 0 || bname.compare("51.png") == 0))
		{
			std::string buildname = StringUtils::format("mapui/focusbuildblock_%s", bname.c_str());
			buildfocus->loadTexture(buildname, cocos2d::ui::Widget::TextureResType::PLIST);

			buildfocus->setPosition(Vec2(bindex % blockColCount * MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, MAPBLOCKHEIGHT / 2 + bindex / blockColCount * MAPBLOCKHEIGHT));
			buildfocus->setVisible(true);

			buildfocus->setTag(bindex);
			buildfocus->getChildByName("z002")->setTag(bindex);
			std::string zcountstr = StringUtils::format("%d", MyRes::getMyResCount("z002", MYPACKAGE) + MyRes::getMyResCount("z002", MYSTORAGE));
			z002countlbl->setString(zcountstr);
			//map_mapBlocks[bindex]->focusBuild();
			return -(10000 + bindex);
		}
		return -1;
	}
	return 1;
}

void MapBlockScene::createMap()
{
	scrollView = ScrollView::create();
	scrollView->setViewSize(Size(720, 1067));
	scrollView->setPosition(0, 0);
	m_csbnode->addChild(scrollView, -1);

	int mapW = blockColCount * MAPBLOCKWIDTH;
	int mapH = (blockRowCount + 1.5)* MAPBLOCKHEIGHT;
	m_mapscrollcontainer = LayerColor::create(Color4B(0, 0, 0, 255));
	m_mapscrollcontainer->setContentSize(Size(blockColCount * MAPBLOCKWIDTH, (blockRowCount + 1.5f)* MAPBLOCKHEIGHT));

	float scalex = scrollView->getViewSize().width / mapW;
	float scaley = scrollView->getViewSize().height / mapH;
	scrollView->setContainer(m_mapscrollcontainer);

	scrollView->setTouchEnabled(true);
	scrollView->setDirection(ScrollView::Direction::BOTH);
	scrollView->setMinScale(MIN(scalex, scaley));
	scrollView->setMaxScale(2);
	scrollView->setBounceable(true);
	scrollView->setDelegate(this);
	scrollView->setSwallowTouches(false);

	float offsetx = m_mapscrollcontainer->getContentSize().width / 2 - scrollView->getViewSize().width / 2;
	float offsety = m_mapscrollcontainer->getContentSize().height / 2 - scrollView->getViewSize().height / 2;

	scrollView->setContentOffset(Vec2(-offsetx, -offsety));
}

void MapBlockScene::scrollViewDidScroll(ScrollView* view)
{
	//isDraging = true;

	log("zhou scrollViewDidScroll");
	return;
}

void MapBlockScene::scrollViewDidZoom(ScrollView* view)
{
	isDraging = true;

	log("zhou scrollViewDidZoom = %.2f", scrollView->getZoomScale());
	ajustMyPos();
	return;
}

void MapBlockScene::heroDelayTranse(float dt)
{
	float delay = 0.0f;
	if (isMaze)
	{
		delay = 0.6f;
		myposHero->setVisible(false);
		_mylight->setVisible(false);
		setMyPos();
	}
	else
	{
		delay = 1.1f;
		ajustMyPos(true);
		this->scheduleOnce(schedule_selector(MapBlockScene::delaySetMyPos), 0.5f);
	}

	mapnamelbl->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(CC_CALLBACK_0(MapBlockScene::ajustStatus, this)), NULL));
}

void MapBlockScene::delaySetMyPos(float dt)
{
	myposHero->setVisible(false);
	_mylight->setVisible(false);
	setMyPos();
}

void MapBlockScene::ajustStatus()
{
	ajustMyPos(!isMaze);
	setBtnEnable(true);
	if (isMaze)
		isRoutingBreakOff = false;
}

void MapBlockScene::setMyPos(bool isfollowme)
{
	int px = mycurCol * MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2;
	int py = mycurRow * MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2;
	if (myposHero == NULL)
	{
		myposHero = spine::SkeletonAnimation::createWithJsonFile("herowalk/herowalk.json", "herowalk/herowalk.atlas", 0.5f);//0.5是设置图片的缩放比例

		myposHero->setPosition(Vec2(px, py + HEROOFFSET_Y));
		myposHero->setTimeScale(2);
		m_mapscrollcontainer->addChild(myposHero, 39999);
		myposHero->setAnimation(0, standname[m_walkDirection], false);

		createMyRender();
		this->schedule(schedule_selector(MapBlockScene::updateMyRender), 0.05f);
	}
	else
	{
		isMoving = true;
		myposHero->setToSetupPose();
		spTrackEntry* spentry = myposHero->setAnimation(0, walkname[m_walkDirection], true);//true是指循环播放walk动作
		if (spentry) {//如果指定的name 找不到，setAnimation失败，就会导致 spAnimationState_apply 崩溃，所以加个判断
			spAnimationState_apply(myposHero->getState(), myposHero->getSkeleton());
		}
		myposHero->runAction(Sequence::create(MoveTo::create(0.5f, Vec2(px, py + HEROOFFSET_Y)), CallFunc::create(CC_CALLBACK_0(MapBlockScene::stopMoving, this)), NULL));
		m_lastWalkDirection = m_walkDirection;
	}

	if (isfollowme)
		ajustMyPos();

	this->scheduleOnce(schedule_selector(MapBlockScene::updateFog), 0.25f);

	std::string str = StringUtils::format("%d,%d", blockRowCount - mycurRow, blockColCount - mycurCol);
	sitelbl->setString(str);
}

void MapBlockScene::updateMyRender(float dt)
{
	_myrender->beginWithClear(0, 0, 0, 0.3f, 0, 0);
	_mylight->setPosition(myposHero->getPosition());
	_mylight->visit();
	_myrender->end();
	Director::getInstance()->getRenderer()->render();
}

void MapBlockScene::ajustMyPos(bool isanim)
{
	float offsetx = myposHero->getPosition().x * scrollView->getZoomScale() - scrollView->getViewSize().width / 2;
	float offsety = myposHero->getPosition().y * scrollView->getZoomScale() - scrollView->getViewSize().height / 2;

	if (offsetx < 0)
		offsetx = 0;
	if (offsety < 0)
		offsety = 0;

	if (offsetx > m_mapscrollcontainer->getContentSize().width * scrollView->getZoomScale() - scrollView->getViewSize().width)
		offsetx = m_mapscrollcontainer->getContentSize().width * scrollView->getZoomScale() - scrollView->getViewSize().width;
	if (offsety > m_mapscrollcontainer->getContentSize().height * scrollView->getZoomScale() - scrollView->getViewSize().height)
		offsety = m_mapscrollcontainer->getContentSize().height * scrollView->getZoomScale() - scrollView->getViewSize().height;

	if (isanim)
		scrollView->setContentOffsetInDuration(Vec2(-offsetx, -offsety), 0.5f);
	else
		scrollView->setContentOffset(Vec2(-offsetx, -offsety));
}

FightHeroNode* MapBlockScene::getFightHeroNode(int index)
{
	return (FightHeroNode*)this->getChildByTag(index);
}

void MapBlockScene::createMyRender()
{
	_myrender = RenderTexture::create(m_mapscrollcontainer->getContentSize().width, m_mapscrollcontainer->getContentSize().height);
	_myrender->setPosition(m_mapscrollcontainer->getContentSize().width / 2, m_mapscrollcontainer->getContentSize().height / 2);
	m_mapscrollcontainer->addChild(_myrender, 30000);

	_myrender->beginWithClear(0, 0, 0, 0.3f, 0, 0);

	_mylight = Sprite::createWithSpriteFrameName("mapui/fog.png");
	_mylight->setBlendFunc({ GL_ZERO, GL_ONE_MINUS_SRC_ALPHA });
	_mylight->setAnchorPoint(Vec2(0.5, 0.5));
	_mylight->setPosition(myposHero->getPosition());
	_mylight->setScale(fogscale);
	_myrender->addChild(_mylight);
	_mylight->visit();
	_myrender->end();
	Director::getInstance()->getRenderer()->render();
}

void MapBlockScene::createFog()
{
	std::map<int, MapBlock*>::iterator it;

	std::string str = DataSave::getInstance()->getMapVisibleArea(m_mapid);

	std::vector<std::string> vec_cfg;
	CommonFuncs::split(str, vec_cfg, ";");


	if (GlobalInstance::challangeType == CH_SUPERBOSS)
	{
		mapIsAllOpen = true;
		return;
	}
	if (vec_cfg.size() >= 2)
	{
		//randStartPos = atoi(vec_cfg[1].c_str());

		if (vec_cfg[0].compare("-1") == 0)
		{
			mapIsAllOpen = true;
			return;
		}

		if (vec_cfg[0].length() > 0)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(vec_cfg[0], vec_tmp, ",");
			unsigned int i = 0;
			for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
			{
				if (i < vec_tmp.size())
				{
					int val = atoi(vec_tmp[i].c_str());
					map_mapBlocks[it->first]->setIsCanSee(val == 1 ? true : false);
					i++;
				}
			}
		}
	}

	_fogrender = RenderTexture::create(m_mapscrollcontainer->getContentSize().width, m_mapscrollcontainer->getContentSize().height);
	_fogrender->setPosition(m_mapscrollcontainer->getContentSize().width / 2, m_mapscrollcontainer->getContentSize().height / 2);
	m_mapscrollcontainer->addChild(_fogrender, 20000);

	_fogrender->beginWithClear(0, 0, 0, 1.0f, 0, 0);
	for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
	{
		if (map_mapBlocks[it->first]->getIsCanSee())
		{
			addFogBlock(it->first / blockColCount, it->first%blockColCount, fogscale);
		}
	}
	_fogrender->end();
	Director::getInstance()->getRenderer()->render();
}

void MapBlockScene::updateFog(float dt)
{
	if (!map_mapBlocks[mycurRow*blockColCount + mycurCol]->getIsCanSee() && _fogrender != NULL)
	{
		_fogrender->begin();
		addFogBlock(mycurRow, mycurCol, fogscale);
		_fogrender->end();
		Director::getInstance()->getRenderer()->render();
		map_mapBlocks[mycurRow*blockColCount + mycurCol]->setIsCanSee(true);
	}
}

void MapBlockScene::addFogBlock(int row, int col, int scale)
{
	Sprite * sp = Sprite::createWithSpriteFrameName("mapui/fog.png");
	sp->setBlendFunc({ GL_ZERO, GL_ONE_MINUS_SRC_ALPHA });
	sp->setAnchorPoint(Vec2(0.5, 0.5));
	sp->setPosition(Vec2(col*MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, row*MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2));
	std::string spname = StringUtils::format("fog%d", row*blockColCount + col);
	sp->setScale(scale);
	_fogrender->addChild(sp, 0, spname);
	sp->visit();
}

void MapBlockScene::showThrowTorchParticle(int row, int col)
{
	ParticleSystemQuad* particleSystem = ParticleSystemQuad::create("hr.plist");
	particleSystem->setPosition(myposHero->getPosition());
	m_mapscrollcontainer->addChild(particleSystem, 1000000);
	Vec2 destPos = Vec2(col*MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, row*MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2);
	Spawn* movebig = Spawn::create(MoveBy::create(0.75f, Vec2((destPos.x - myposHero->getPositionX())/2, (destPos.y - myposHero->getPositionY())/2)), ScaleTo::create(0.75f, 2.0f), NULL);
	Spawn* movenormal = Spawn::create(MoveTo::create(0.75f, Vec2(destPos.x, destPos.y)), ScaleTo::create(0.75f, 1.0f), NULL);
	particleSystem->runAction(Sequence::create(movebig, movenormal, CallFunc::create(CC_CALLBACK_0(MapBlockScene::torchLight, this, row, col)), FadeOut::create(2.0f), RemoveSelf::create(), NULL));
}

void MapBlockScene::torchLight(int row, int col)
{
	if (_fogrender != NULL)
	{
		_fogrender->begin();
		addFogBlock(row, col, TORCHSCALE);
		_fogrender->end();
		Director::getInstance()->getRenderer()->render();
	}
}


void MapBlockScene::resetBlockData()
{
	MapBlock::vec_randMonsters.clear();
	MapBlock::vec_randMonstersRes.clear();
	MapBlock::randMonstersMinCount = 0;
	MapBlock::randMonstersMaxCount = 0;
}

bool MapBlockScene::getFirstFightBoss()
{
	if (firstpostype == POS_BOSS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MapBlockScene::doMyStatus()
{
	int ret = -1;
	int status = MAP_S_NOTING;
	int mycr = mycurRow*blockColCount + mycurCol;
	MapBlock* mapblock = map_mapBlocks[mycr];
	std::map<int, int>::iterator rnd_it;
	int r = GlobalInstance::getInstance()->createRandomNum(100) + 1;
	
	for (rnd_it = mapblock->map_eventrnd.begin(); rnd_it != mapblock->map_eventrnd.end(); rnd_it++)
	{
		if (r <= mapblock->map_eventrnd[rnd_it->first])
		{
			ret = rnd_it->first;
			break;
		}
	}
	//ret = 5;
	if (ret >= 0)
	{
		status = MAP_S_EVENT;
		//触发过一次不再触发 
		for (rnd_it = mapblock->map_eventrnd.begin(); rnd_it != mapblock->map_eventrnd.end(); rnd_it++)
		{
			mapblock->map_eventrnd[rnd_it->first] = 0;
			mapblock->removeEventIcon();
		}
#if MAP_BET
		if (ret <= POS_BUSINESS)//其它事件美术没有准备好，会崩溃，有图后打开这里
#else
		if (ret <= POS_BUSINESS && ret != POS_BET)
#endif
		{
			isRoutingBreakOff = true;
			MapEventLayer* mlayer = MapEventLayer::create(ret);
			this->addChild(mlayer);
			AnimationEffect::openAniEffect((Layer*)mlayer);
			calcStar(SA_EVENT);
			sitetext->stopAllActions();
			removeAllRoutingBlock();
		}
		else
		{
			status = MAP_S_NOTING;
		}
	}
	else
	{
		vec_enemys.clear();
		vec_winrewards.clear();

		if (mapblock->getPosType() != POS_NOTHING)
		{
			isRoutingBreakOff = true;
			sitetext->stopAllActions();
			removeAllRoutingBlock();
		}
		if (mapblock->getPosType() == POS_NPC || mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS)
		{
			if (mapblock->getPosType() == POS_BOSS)
			{
				firstpostype = mapblock->getPosType();
			}
			creatNpcOrBoss(mapblock);
		}
		else if (mapblock->getPosType() == POS_MONSTER)
		{
			configRndMonstersAndRewards(mapblock->getTag());
			mapblock->setPosType(POS_NOTHING);
			mapblock->removePosIcon();
		}
		else if (mapblock->getPosType() == POS_BOX)
		{
			int mid = atoi(mapblock->getPosNpcID().c_str());
			int count = DataSave::getInstance()->getMapBoxRewards(m_mapid, mid);

			status = MAP_S_EVENT;
			createBoxRewards(mapblock);

			mapblock->setPosType(POS_NOTHING);
			mapblock->removePosIcon();

			DataSave::getInstance()->setMapBoxRewards(m_mapid, mid, count + 1);
		}

		else if (mapblock->getPosType() == POS_MAZEENTRY)//进入迷宫
		{
			GlobalInstance::vec_mazeroute.clear();
			GlobalInstance::mazerouteindex = 0;
			GlobalInstance::ishasmazeentry = false;

			GlobalInstance::eventstartmappos = mycr;

			GlobalInstance::map_randstartpos = randStartPos;

			GlobalInstance::eventfrommapid = m_mapid;

			int c = atoi(GlobalInstance::eventfrommapid.substr(1, 1).c_str());
			if (c > 5)
			{
				c = 5;
			}
			int rz = c - 1;
			for (int i = 1; i < mazes[rz]; i++)
			{
				GlobalInstance::vec_mazeroute.push_back(i);
			}
			std::random_shuffle(GlobalInstance::vec_mazeroute.begin(), GlobalInstance::vec_mazeroute.end());
			GlobalInstance::vec_mazeroute.push_back(mazes[rz]);
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MazeTransitionScene::createScene(c, TO_ENTER)));

			GlobalInstance::getInstance()->setMazeEventData(1, 0);

			MyRes::Add("z002", 10, MYPACKAGE);
			std::string contentstr = StringUtils::format(ResourceLang::map_lang["newtemplet7"].c_str(), GlobalInstance::getInstance()->getMyNickName().c_str());
			MainScene::addNews(contentstr, 2);
			calcStar(SA_ENTERMAZE);
			return;
		}

		if (vec_enemys.size() > 0)
		{
			//先判断任务
			bool isTask = false;
			int mycr = mycurRow*blockColCount + mycurCol;
			MapBlock* mapblock = map_mapBlocks[mycr];
			if (Quest::getMainQuestMap(m_mapid) && Quest::getMainQuestNpc(mapblock->getPosNpcID()) && (mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS || mapblock->getPosType() == POS_NPC))
			{
				isTask = true;
				this->addChild(TaskTalkLayer::create(mapblock->getPosNpcID(), vec_enemys));
			}
			else if (Quest::getBranchQuestMap(m_mapid) && Quest::getBranchQuestNpc(mapblock->getPosNpcID()) && (mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS || mapblock->getPosType() == POS_NPC))
			{
				isTask = true;
				this->addChild(TaskTalkLayer::create(mapblock->getPosNpcID(), vec_enemys, 1));
			}
			else if (m_mapid.compare("m0-0-0") != 0 && m_mapid.compare("m1-6-1") != 0 && (mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS || mapblock->getPosType() == POS_NPC))
			{
				isTask = true;
				NpcLayer* layer = NpcLayer::create(mapblock->getPosNpcID(), vec_enemys);
				this->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			if (!isTask)
			{
				if (GlobalInstance::challangeType == CH_SUPERBOSS)
				{

					if (GlobalInstance::supperbossinfo.bosslefthp <= 0)
					{
						isRoutingBreakOff = false;
						MovingLabel::show(ResourceLang::map_lang["supperbossnohp"]);
						return;
					}
					if (GlobalInstance::supperbossinfo.leftfreecount <= 0)
					{
						if (GlobalInstance::supperbossinfo.leftcoincount > 0)
						{
							HintBoxLayer* layer = HintBoxLayer::create(ResourceLang::map_lang["supperbossbuycount"], 15);
							this->addChild(layer);
							AnimationEffect::openAniEffect(layer);
							isRoutingBreakOff = false;
						}
						else
						{
							MovingLabel::show(ResourceLang::map_lang["supperbosscountlimit"]);
							isRoutingBreakOff = false;
						}
					}
					else
					{
						supperBossFight();
					}
				}
				else
				{
					showFightingLayer(vec_enemys);
				}
			}
		}
	}
}

void MapBlockScene::supperBossFight()
{
	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	Director::getInstance()->getRunningScene()->addChild(wp, 0, "waitingprogress");
	HttpDataSwap::init(this)->getServerTime();
}

void MapBlockScene::createBoxRewards(MapBlock* mbolck)
{
	std::vector<MSGAWDSDATA> vec_rewards;
	if (mbolck->vec_RewardsRes.size() > 0)
	{
		for (unsigned int i = 0; i < mbolck->vec_RewardsRes.size(); i++)
		{
			FOURProperty mdata = mbolck->vec_RewardsRes[i];

			MSGAWDSDATA wdata;
			wdata.rid = mdata.sid;
			wdata.count = mdata.intPara1;
			wdata.qu = mdata.intPara2;

			int rnd = mdata.floatPara3 * 100;
			int r2 = GlobalInstance::getInstance()->createRandomNum(10000);
			if (r2 < rnd)
				vec_rewards.push_back(wdata);
		}
	}
	else
	{
		MapEventLayer::loadEventData("boxevent");
		MapEventLayer::loadPrData();
		int r = GlobalInstance::getInstance()->createRandomNum(4) + 0;
		for (int i = 0; i < r; i++)
		{
			std::string resid = MapEventLayer::getDataIdByPr();
			int m_count = MapEventLayer::getResCountRand(resid);
			int qu = 0;
			int t = 0;
			for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
			{
				if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
					break;
			}
			if (t >= T_ARMOR && t <= T_FASHION)
			{
				qu = MapEventLayer::getEquipQuRand(resid);
			}
			else if (t >= T_WG && t <= T_NG)
			{
				qu = GlobalInstance::map_GF[resid].qu;
			}

			MSGAWDSDATA wdata;
			wdata.rid = resid;
			wdata.count = m_count;
			wdata.qu = qu;
			vec_rewards.push_back(wdata);
		}
	}
	
	if (vec_rewards.size() > 0)
	{
		RewardLayer* layer = RewardLayer::create(vec_rewards, MYSTORAGE, 2);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);

		std::string namestr;
		for (unsigned int i = 0; i < vec_rewards.size(); i++)
		{
			if (namestr.length() > 0)
			{
				namestr.append(ResourceLang::map_lang["dunhao"]);
			}
			int qu = vec_rewards[i].qu;
			if (qu >= 4)
			{
				std::string resid = vec_rewards[i].rid;
				std::string st = StringUtils::format("potential_%d", qu);
				namestr = StringUtils::format(ResourceLang::map_lang["libraryinfoequipname"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str(), ResourceLang::map_lang[st].c_str());
			}
		}
		if (namestr.length() > 0)
		{
			std::string contentstr = StringUtils::format(ResourceLang::map_lang["newtemplet1"].c_str(), GlobalInstance::getInstance()->getMyNickName().c_str(), namestr.c_str());
			MainScene::addNews(contentstr, 2);
		}
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["nothingbox"]);
		isRoutingBreakOff = false;
	}
}

void MapBlockScene::createRndMonsters()
{
	vector<int> monstersrnd;

	int rrnd = 0;
	for (unsigned int m = 0; m < MapBlock::vec_randMonsters.size(); m++)
	{
		FOURProperty propty = MapBlock::vec_randMonsters[m];
		rrnd += propty.floatPara3;
		monstersrnd.push_back(rrnd);
	}

	for (unsigned int n = 0; n < vec_monsterBlocks.size(); n++)
	{
		int rndcount = MapBlock::randMonstersMinCount + GlobalInstance::getInstance()->createRandomNum(MapBlock::randMonstersMaxCount - MapBlock::randMonstersMinCount + 1);
		for (int i = 0; i < rndcount; i++)
		{
			int r1 = GlobalInstance::getInstance()->createRandomNum(100);
			for (unsigned int m = 0; m < MapBlock::vec_randMonsters.size(); m++)
			{
				FOURProperty propty = MapBlock::vec_randMonsters[m];
				if (r1 < monstersrnd[m])
				{
					int minlv = propty.intPara1 / 1000;
					int maxlv = propty.intPara1 % 1000;
					int minqu = propty.intPara2 / 1000;
					int maxqu = propty.intPara2 % 1000;
					int rlv = minlv + GlobalInstance::getInstance()->createRandomNum(maxlv - minlv + 1);
					int rqu = minqu + GlobalInstance::getInstance()->createRandomNum(maxqu - minqu + 1);

					Npc* enemyhero = new Npc();
					std::string sid = propty.sid;
					enemyhero->setId(sid);
					enemyhero->setName(GlobalInstance::map_AllResources[sid].name);
					enemyhero->setVocation(GlobalInstance::map_Npcs[sid].vocation);
					enemyhero->setPotential(rqu);
					enemyhero->setLevel(rlv);
					enemyhero->setHp(enemyhero->getMaxHp());
					map_allRndMonsters[vec_monsterBlocks[n]->getTag()].push_back(enemyhero);
					break;
				}
			}
		}
	}
	
}

void MapBlockScene::configRndMonstersAndRewards(int mbindex)
{
	//int r = 100;
	//
	//if (walkcount > 1)
	//	r = GlobalInstance::getInstance()->createRandomNum(100);
	//if (r < monsterComeRnd/3)
	//{
	//	monsterComeRnd = DEFAULTRND;
	//	walkcount = 0;
	//	int rndcount = MapBlock::randMonstersMinCount + GlobalInstance::getInstance()->createRandomNum(MapBlock::randMonstersMaxCount - MapBlock::randMonstersMinCount + 1);
	//	for (int i = 0; i < rndcount; i++)
	//	{
	//		int r1 = GlobalInstance::getInstance()->createRandomNum(100);
	//		int rnd = 0;
	//		for (unsigned int m = 0; m < MapBlock::vec_randMonsters.size(); m++)
	//		{
	//			FOURProperty propty = MapBlock::vec_randMonsters[m];
	//			rnd += propty.floatPara3;
	//			if (r1 < rnd)
	//			{
	//				int minlv = propty.intPara1 / 1000;
	//				int maxlv = propty.intPara1 % 1000;
	//				int minqu = propty.intPara2 / 1000;
	//				int maxqu = propty.intPara2 % 1000;
	//				int rlv = minlv + GlobalInstance::getInstance()->createRandomNum(maxlv - minlv + 1);
	//				int rqu = minqu + GlobalInstance::getInstance()->createRandomNum(maxqu - minqu + 1);

	//				Npc* enemyhero = new Npc();
	//				std::string sid = MapBlock::vec_randMonsters[m].sid;
	//				enemyhero->setId(sid);
	//				enemyhero->setName(GlobalInstance::map_AllResources[sid].name);
	//				enemyhero->setVocation(GlobalInstance::map_Npcs[sid].vocation);
	//				enemyhero->setPotential(rqu);
	//				enemyhero->setLevel(rlv);
	//				enemyhero->setHp(enemyhero->getMaxHp());
	//				vec_enemys.push_back(enemyhero);
	//				break;
	//			}
	//		}

	//	}

	//	for (unsigned int i = 0; i < MapBlock::vec_randMonstersRes.size(); i++)
	//	{
	//		FOURProperty propty = MapBlock::vec_randMonstersRes[i];
	//		int rnd = propty.floatPara3 * 100;
	//		int r2 = GlobalInstance::getInstance()->createRandomNum(10000);
	//		if (r2 < rnd)
	//			vec_winrewards.push_back(propty);
	//	}
	//}


	vec_enemys = map_allRndMonsters[mbindex];

	for (unsigned int i = 0; i < MapBlock::vec_randMonstersRes.size(); i++)
	{
		FOURProperty propty = MapBlock::vec_randMonstersRes[i];
		int rnd = propty.floatPara3 * 100;
		int r2 = GlobalInstance::getInstance()->createRandomNum(10000);
		if (r2 < rnd)
			vec_winrewards.push_back(propty);
	}
}

void MapBlockScene::eventFight()
{
	createEventMonsters();

	showFightingLayer(vec_enemys);
}

void MapBlockScene::createEventMonsters()
{
	vector<int> monstersrnd;

	int rrnd = 0;
	for (unsigned int m = 0; m < MapBlock::vec_randMonsters.size(); m++)
	{
		FOURProperty propty = MapBlock::vec_randMonsters[m];
		rrnd += propty.floatPara3;
		monstersrnd.push_back(rrnd);
	}

	int rndcount = MapBlock::randMonstersMinCount + GlobalInstance::getInstance()->createRandomNum(MapBlock::randMonstersMaxCount - MapBlock::randMonstersMinCount + 1);
	for (int i = 0; i < rndcount; i++)
	{
		int r1 = GlobalInstance::getInstance()->createRandomNum(100);
		for (unsigned int m = 0; m < MapBlock::vec_randMonsters.size(); m++)
		{
			FOURProperty propty = MapBlock::vec_randMonsters[m];
			if (r1 < monstersrnd[m])
			{
				int minlv = propty.intPara1 / 1000;
				int maxlv = propty.intPara1 % 1000;
				int minqu = propty.intPara2 / 1000;
				int maxqu = propty.intPara2 % 1000;
				int rlv = minlv + GlobalInstance::getInstance()->createRandomNum(maxlv - minlv + 1);
				int rqu = minqu + GlobalInstance::getInstance()->createRandomNum(maxqu - minqu + 1);

				Npc* enemyhero = new Npc();
				std::string sid = "n001";
				enemyhero->setId(sid);
				enemyhero->setName(GlobalInstance::map_AllResources[sid].name);
				enemyhero->setVocation(GlobalInstance::map_Npcs[sid].vocation);
				enemyhero->setPotential(rqu);
				enemyhero->setLevel(rlv);
				enemyhero->setHp(enemyhero->getMaxHp());
				vec_enemys.push_back(enemyhero);
				break;
			}
		}

	}
}

void MapBlockScene::creatNpcOrBoss(MapBlock* mbolck)
{
	//int r1 = GlobalInstance::getInstance()->createRandomNum(100);
	//if (r1 < mbolck->getPosNpcRnd())
	//{
	//	for (unsigned int m = 0; m < 6; m++)
	//	{
	//		FOURProperty propty = mbolck->npcs[m];
	//		if (propty.sid.length() <= 0)
	//		{
	//			vec_enemys.push_back(NULL);
	//		}
	//		else
	//		{
	//			Npc* enemyhero = new Npc();
	//			std::string sid = propty.sid;
	//			enemyhero->setId(sid);
	//			enemyhero->setName(GlobalInstance::map_AllResources[sid].name);
	//			enemyhero->setVocation(GlobalInstance::map_Npcs[sid].vocation);
	//			enemyhero->setPotential(propty.intPara1);
	//			enemyhero->setLevel(propty.intPara2 - 1);
	//			enemyhero->setHp(enemyhero->getMaxHp());
	//			vec_enemys.push_back(enemyhero);
	//		}
	//	}
	//}

	for (unsigned int m = 0; m < 6; m++)
	{
		FOURProperty propty = mbolck->npcs[m];
		if (propty.sid.length() <= 0)
		{
			vec_enemys.push_back(NULL);
		}
		else
		{
			Npc* enemyhero = new Npc();
			std::string sid = propty.sid;
			enemyhero->setId(sid);
			enemyhero->setName(GlobalInstance::map_AllResources[sid].name);
			enemyhero->setVocation(GlobalInstance::map_Npcs[sid].vocation);
			enemyhero->setPotential(propty.intPara1);
			enemyhero->setLevel(propty.intPara2 - 1);

			if (GlobalInstance::challangeType == CH_SUPERBOSS)
				enemyhero->setHp(GlobalInstance::supperbossinfo.bosslefthp);
			else
				enemyhero->setHp(enemyhero->getMaxHp());
			vec_enemys.push_back(enemyhero);
		}
	}

	for (unsigned int i = 0; i < mbolck->vec_RewardsRes.size(); i++)
	{
		FOURProperty propty = mbolck->vec_RewardsRes[i];
		int rnd = propty.floatPara3 * 100;
		int r2 = GlobalInstance::getInstance()->createRandomNum(10000);
		if (r2 < rnd)
			vec_winrewards.push_back(propty);
	}
}

void MapBlockScene::updateHeroUI(int which)
{
	FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(which);
	fnode->updateHp();

	GlobalInstance::getInstance()->saveHero(GlobalInstance::myCardHeros[which]);
}

void MapBlockScene::showFightResult(int result)
{
	int mycr = mycurRow*blockColCount + mycurCol;
	MapBlock* mapblock = map_mapBlocks[mycr];

	if (GlobalInstance::challangeType == CH_SUPERBOSS)
	{
		return;
	}

	if (result == 1)//胜利
	{
		if (Quest::getMainQuestMap(m_mapid) && Quest::getMainQuestNpc(mapblock->getPosNpcID()) && (mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS || mapblock->getPosType() == POS_NPC))
		{
			Quest::finishTaskMain();
			//showUnlockChapter();
		}
		else if (Quest::getBranchQuestMap(m_mapid) && Quest::getBranchQuestNpc(mapblock->getPosNpcID()) && (mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS || mapblock->getPosType() == POS_NPC))
		{
			Quest::finishTaskBranch();
		}
		Quest::setAchieveTypeCount(ACHIEVE_FIGHT, 1, mapblock->getPosNpcID());

		if (GlobalInstance::npcmasterfinish != 1)
		{
			GlobalInstance::starcontinuefightsucccount++;
			GlobalInstance::curmapcontinuefightsucccount++;

			int liveherocount = 0;

			for (int i = 0; i < 6; i++)
			{
				if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
					liveherocount++;
			}

			if (GlobalInstance::takeoutherocount == liveherocount)
			{
				if (GlobalInstance::starcontinuefightsucccount >= 10)
					calcStar(SA_NODEATH);
			}
			else
			{
				GlobalInstance::starcontinuefightsucccount = 0;
			}
			calcStar(SA_FIGHTSUCC);
		}

		if (mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS)
		{
			if (!isMaze)
			{
				GlobalInstance::fightwinbosscount++;

				if (GlobalInstance::fightwinbosscount >= totalBoxcount)
				{
					DataSave::getInstance()->setFightWinAllBoss(m_mapid, true);
				}
			}
		}

		isMoving = true;
		int bindex = (mycurRow)*blockColCount + mycurCol;
		if (map_mapBlocks[bindex]->getPosType() == POS_BOSS)
		{
			map_mapBlocks[bindex]->removePosIcon();
			map_mapBlocks[bindex]->setPosType(POS_NOTHING);
		}
		else if (map_mapBlocks[bindex]->getPosType() == POS_TBOSS)
		{
			map_mapBlocks[bindex]->removePosIcon();
			map_mapBlocks[bindex]->setPosType(POS_NOTHING);
		}
	}
	else
	{
		if (GlobalInstance::npcmasterfinish != 1)
		{
			GlobalInstance::starcontinuefightsucccount = 0;
			GlobalInstance::curmapcontinuefightsucccount = 0;
		}
	}

	//出师战斗结果
	if (GlobalInstance::npcmasterfinish == 1)
	{
		GlobalInstance::npcmasterfinish = 0;
		if (result == 1)
		{
			MovingLabel::show(ResourceLang::map_lang["masterfinishsuccess"]);
			std::vector<int>::iterator it;
			for (it = GlobalInstance::map_myfriendly[mapblock->getPosNpcID()].relation.begin(); it != GlobalInstance::map_myfriendly[mapblock->getPosNpcID()].relation.end(); it++)
			{
				if (*it == NPC_MASTER)
				{
					GlobalInstance::map_myfriendly[mapblock->getPosNpcID()].relation.erase(it);
					break;
				}
			}
			GlobalInstance::getInstance()->saveNpcFriendly();
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["masterfinishfail"]);
		}
	}

	this->scheduleOnce(schedule_selector(MapBlockScene::delayShowFightResult), 1.0f);
}

void MapBlockScene::showSuperBossLayer()
{
	SupperBossRankLayer* slayer = SupperBossRankLayer::create();
	this->addChild(slayer);
	AnimationEffect::openAniEffect(slayer);
	isRoutingBreakOff = false;
}

void MapBlockScene::showUnlockChapter()
{
	//设置解锁章节
	std::string mapid = GlobalInstance::myCurMainData.place;
	int c = GlobalInstance::getInstance()->getUnlockChapter();
	int s = atoi(mapid.substr(1, mapid.find_first_of("-") - 1).c_str());
	if (s > c)
	{
		GlobalInstance::getInstance()->saveUnlockChapter(s);
		NewPopLayer* layer = NewPopLayer::create(s);
		if (g_MapBlockScene != NULL)
		{
			g_MapBlockScene->addChild(layer, 1);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
	}
	setMapOrderData();
}

void MapBlockScene::setMapOrderData()
{
	int index = m_mapid.find_last_of("-");
	std::string mainid = m_mapid.substr(0, index);
	int finishOrder = atoi(m_mapid.substr(index+1, 1).c_str());
	DataSave::getInstance()->setMapOrderCount(mainid, finishOrder + 1);
}

void MapBlockScene::openAllMap()
{
	mapFogScale = 1.0f;

	if (_fogrender != NULL)
	{
		this->schedule(schedule_selector(MapBlockScene::allOpenAnim));
	}
	mapIsAllOpen = true;
	std::string str = StringUtils::format("-1;%d", randStartPos);
	DataSave::getInstance()->setMapVisibleArea(m_mapid, str);

	std::map<int, MapBlock*>::iterator it;

	for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
	{
		MapBlock* block = map_mapBlocks[it->first];
		if (block->getWalkable())
			block->setIsCanSee(true);
	}
}

void MapBlockScene::allOpenAnim(float dt)
{
	if (_fogrender != NULL)
	{
		_fogrender->begin();

		if (mapAllOpenFog == NULL)
		{
			mapAllOpenFog = Sprite::createWithSpriteFrameName("mapui/fog.png");
			mapAllOpenFog->setBlendFunc({ GL_ZERO, GL_ONE_MINUS_SRC_ALPHA });
			mapAllOpenFog->setAnchorPoint(Vec2(0.5, 0.5));
			mapAllOpenFog->setPosition(Vec2(mycurCol*MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, mycurRow*MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2));
			_fogrender->addChild(mapAllOpenFog, 0, "allopenfog");
		}
		mapAllOpenFog->setScale(mapFogScale);

		mapAllOpenFog->visit();
		_fogrender->end();
		Director::getInstance()->getRenderer()->render();
		mapFogScale += 0.8f;

		int maxscale = MAX(mycurRow, blockRowCount - mycurRow);
		if (mapFogScale > maxscale*2.5f)
		{
			this->unschedule(schedule_selector(MapBlockScene::allOpenAnim));
			_fogrender->removeFromParentAndCleanup(true);
			_fogrender = NULL;
		}
	}
}

int MapBlockScene::getWinExp()
{
	int exp = 0;
	for (unsigned int i = 0; i < vec_enemys.size(); i++)
	{
		if (vec_enemys[i] != NULL)
		{	
			exp += GlobalInstance::map_NpcAttrData[vec_enemys[i]->getVocation()].vec_bnsexp[vec_enemys[i]->getLevel()];
		}
	}
	return exp;
}

void MapBlockScene::delayShowFightResult(float dt)
{
	int count = 0;
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL &&  GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
			count++;
	}
	int winexp = 0;
	if (count > 0)
	{
		winexp = getWinExp() / count;
		if (winexp <= 0)
			winexp = 1;
	}

	FightingResultLayer* FRlayer = FightingResultLayer::create(vec_winrewards, winexp);
	this->addChild(FRlayer);
	AnimationEffect::openAniEffect((Layer*)FRlayer);

	for (unsigned int i = 0; i < vec_enemys.size(); i++)
	{
		if (vec_enemys[i] != NULL)
		{
			delete vec_enemys[i];
			vec_enemys[i] = NULL;
		}
	}
	isMoving = false;

	if (winexp <= 0)
	{
		CannotTouchLayer* notTouchLayer = CannotTouchLayer::create();
		this->addChild(notTouchLayer, 1, "cannottouchlayer");
		this->scheduleOnce(schedule_selector(MapBlockScene::delayShowStarResult), 0.5f);
	}
}

void MapBlockScene::delayShowStarResult(float dt)
{
	checkShowStarUi(1);
	this->removeChildByName("cannottouchlayer");
}

void MapBlockScene::onBlockClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::BEGAN)
	{

	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		//if (usingprop == -1 && !isMaze)
		//	return;

		if (isDraging)
		{
			//isDraging = false;
			return;
		}
		if (isRoutingBreakOff)
			return;

		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (!isMaze)
		{
			if (usingprop == MAP_USEINGTORCH)
			{
				showThrowTorchParticle(tag / blockColCount, tag%blockColCount);
				MyRes::Use("z001", 1);
				GlobalInstance::getInstance()->usePropsCount(COSTPROP_TORCH, 1);
				usingprop = -1;

				Node* focus = torchbtn->getChildByName("focus");
				focus->stopAllActions();
				focus->setVisible(false);

				calcStar(SA_USETORCH);
			}
			else if (usingprop == MAP_USEINGTRANSER)
			{
				int b = checkNearestIndex(tag, 3);
				if (b >= 0)//可传送
				{
					int c = b % blockColCount;
					int r = b / blockColCount;
					int needfood = abs(c - mycurCol) + abs(r - mycurRow) - 1;
					if (MyRes::getMyResCount("r001", MYPACKAGE) >= needfood*2/5)
					{
						setBtnEnable(false);

						mycurCol = b % blockColCount;
						mycurRow = b / blockColCount;
						this->scheduleOnce(schedule_selector(MapBlockScene::heroDelayTranse), 0.2f);
						GlobalInstance::getInstance()->usePropsCount(COSTPROP_TRANSER, 1);
						usingprop = -1;
						MyRes::Use("r001", needfood, MYPACKAGE);

						sitelbl->runAction(Sequence::create(DelayTime::create(2.0f), CallFunc::create(CC_CALLBACK_0(MapBlockScene::showTransFoodDesc, this, needfood)), NULL));
						Node* focus = transbtn->getChildByName("focus");
						focus->stopAllActions();
						focus->setVisible(false);
					}
					else
					{
						MovingLabel::show(ResourceLang::map_lang["nofoodtransdesc"]);

						std::vector< MSGAWDSDATA> vec_res;
						int count[] = { 50, 100, 200 };
						for (unsigned int i = 0; i < 3; i++)
						{
							MSGAWDSDATA rdata;
							rdata.rid = "r001";
							rdata.count = count[i];
							rdata.qu = 0;
							vec_res.push_back(rdata);
						}

						BuySelectLayer* layer = BuySelectLayer::create(vec_res, MYPACKAGE);
						this->addChild(layer);
						AnimationEffect::openAniEffect(layer);
					}
				}
				else
				{
					MovingLabel::show(ResourceLang::map_lang["cannottransdesc"]);
				}
			}
			else
			{
				destblockindex = checkNearestIndex(tag, 6);

				if (guideBossPos >= 0)
				{
					destblockindex = guideBossPos;
					guideBossPos = -1;
				}
				if (destblockindex < 0 || (tag != mycurRow * blockColCount + mycurCol && destblockindex == mycurRow * blockColCount + mycurCol))
					MovingLabel::show(ResourceLang::map_lang["norouting"]);
				else
				{
					if (isMovingRouting)
					{
						sitetext->stopAllActions();
						removeAllRoutingBlock();
						checkMazeStoneHint();
						return;
					}
					astarrouting->moveToPosByAStar(Vec2(mycurCol, mycurRow), Vec2(destblockindex % blockColCount, destblockindex / blockColCount));
				}
			}

		}
		else
		{
			int col = tag % blockColCount;
			int row = tag / blockColCount;
			std::string bname = map_mapBlocks[tag]->getBuildName();
			if ((mycurRow >= row - 1 && mycurRow <= row + 1) && (mycurCol >= col - 1 && mycurCol <= col + 1) &&  (bname.compare("8.png") == 0 || bname.compare("51.png") == 0))
			{
				std::string buildname = StringUtils::format("mapui/focusbuildblock_%s", bname.c_str());
				buildfocus->setPosition(Vec2(col * MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, MAPBLOCKHEIGHT / 2 + row * MAPBLOCKHEIGHT));
				buildfocus->loadTexture(buildname, cocos2d::ui::Widget::TextureResType::PLIST);
				buildfocus->setVisible(true);
				buildfocus->setTag(tag);
				buildfocus->getChildByName("z002")->setTag(tag);
				std::string zcountstr = StringUtils::format("%d", MyRes::getMyResCount("z002", MYPACKAGE) + MyRes::getMyResCount("z002", MYSTORAGE));
				z002countlbl->setString(zcountstr);
			}
			else
			{
				buildfocus->setVisible(false);
				destblockindex = checkNearestIndex(tag, 6);
				if (destblockindex < 0 || (tag != mycurRow* blockColCount + mycurCol && destblockindex == mycurRow * blockColCount + mycurCol))
					MovingLabel::show(ResourceLang::map_lang["norouting"]);
				else
				{
					if (isMovingRouting)
					{
						sitetext->stopAllActions();
						removeAllRoutingBlock();
						checkMazeStoneHint();
						return;
					}
					astarrouting->moveToPosByAStar(Vec2(mycurCol, mycurRow), Vec2(destblockindex % blockColCount, destblockindex / blockColCount));
				}
			}
		}
        isoverscreen = false;
        ishalfscreen = false;
		if (destblockindex > 0)
		{
			int mycurindex = mycurRow * blockColCount + mycurCol;
			if ((map_mapBlocks[mycurindex]->getPositionX()* scrollView->getZoomScale() < fabs(scrollView->getContentOffset().x) || map_mapBlocks[mycurindex]->getPositionY()*scrollView->getZoomScale() < fabs(scrollView->getContentOffset().y))
				|| (map_mapBlocks[mycurindex]->getPositionX()* scrollView->getZoomScale() >= fabs(scrollView->getContentOffset().x) + scrollView->getViewSize().width || map_mapBlocks[mycurindex]->getPositionY() * scrollView->getZoomScale() >= fabs(scrollView->getContentOffset().y) + scrollView->getViewSize().height)
				)
				isoverscreen = true;
            
            if (fabs(map_mapBlocks[mycurindex]->getPositionX() - map_mapBlocks[destblockindex]->getPositionX())* scrollView->getZoomScale() >= scrollView->getViewSize().width/2 || fabs(map_mapBlocks[mycurindex]->getPositionY() - map_mapBlocks[destblockindex]->getPositionY())*scrollView->getZoomScale() >= scrollView->getViewSize().height/2)
                ishalfscreen = true;
        }
	}
}

void MapBlockScene::showTransFoodDesc(int foodcount)
{
	std::string str = StringUtils::format(ResourceLang::map_lang["transfooddesc"].c_str(), foodcount * 2 / 5, foodcount * 3 / 5);
	MovingLabel::show(str);
}

int MapBlockScene::checkNearestIndex(int blockindex, int gridrowcol)
{
	int col = blockindex % blockColCount;
	int row = blockindex / blockColCount;

	if (map_mapBlocks[blockindex]->getWalkable())
	{
		return blockindex;
	}

	for (int i = 1; i <= (gridrowcol+1)/2; i++)
	{
		int r0 = row - i;
		if (r0 < 0)
			r0 = 0;
		int r1 = row + i;
		if (r1 >= blockRowCount)
			r1 = blockRowCount - 1;

		int c0 = col - i;
		if (c0 < 0)
			c0 = 0;
		int c1 = col + i;
		if (c1 >= blockColCount)
			c1 = blockColCount - 1;

		for (int n = r0; n <= r1; n++)
		{
			for (int m = c0; m <= c1; m++)
			{
				int b = n * blockColCount + m;
				if (map_mapBlocks[b]->getWalkable())
				{
					return b;
				}
			}
		}
	}
	return -1;
}

void MapBlockScene::onUsePropClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;

		int cwhere = MYPACKAGE;
		int count = MyRes::getMyResCount("z002", MYPACKAGE);
		if (count <= 0)
		{
			cwhere = MYSTORAGE;
			count = MyRes::getMyResCount("z002", MYSTORAGE);
		}
		if (count <= 0)
		{
			std::vector< MSGAWDSDATA> vec_res;
			int count[] = { 5, 10, 20 };
			for (unsigned int i = 0; i < 3; i++)
			{
				MSGAWDSDATA rdata;
				rdata.rid = "z002";
				rdata.count = count[i];
				rdata.qu = 0;
				vec_res.push_back(rdata);
			}
			BuySelectLayer* layer = BuySelectLayer::create(vec_res);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		else
		{
			int tag = node->getTag();
			removeMazeStone(tag);
			MyRes::Use("z002", 1, cwhere);
		}
	}
}

void MapBlockScene::parseMapXml(std::string mapname)
{
	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();

#if encrypt_jsonxml
	std::string filename = StringUtils::format("mapdata/%s.xml", mapname.c_str());
	std::string contentstr = FileUtils::getInstance()->getStringFromFile(filename);
	Encrypt((char*)contentstr.c_str(), false);
	contentstr = parseData(contentstr.c_str());
#else
	std::string filename = StringUtils::format("jsonxml_original/mapdata/%s.xml", mapname.c_str());
	std::string contentstr = FileUtils::getInstance()->getStringFromFile(filename);
#endif
	int err = pDoc->Parse(contentstr.c_str());
	if (err != 0)
	{
		delete pDoc;
		return;
	}
	//根节点
	tinyxml2::XMLElement *rootele = pDoc->RootElement();
	tinyxml2::XMLElement *dataele = rootele->FirstChildElement();
	while (dataele != NULL)
	{
		if (strcmp(dataele->Name(), "Monstors") == 0)
		{
			MapBlock::randMonstersMinCount = dataele->IntAttribute("minc");
			MapBlock::randMonstersMaxCount = dataele->IntAttribute("maxc");
			tinyxml2::XMLElement *msele = dataele->FirstChildElement();
			while (msele != NULL)
			{
				if (strcmp(msele->Name(), "m") == 0)
				{
					FOURProperty mdata;
					mdata.sid = msele->Attribute("id");
					mdata.intPara1 = msele->IntAttribute("minlv") * 1000 + msele->IntAttribute("maxlv");
					mdata.intPara2 = msele->IntAttribute("minqu") * 1000 + msele->IntAttribute("maxqu");
					mdata.floatPara3 = atof(msele->GetText());
					MapBlock::vec_randMonsters.push_back(mdata);
				}
				else if (strcmp(msele->Name(), "ma") == 0)
				{
					FOURProperty mdata;
					mdata.sid = msele->Attribute("id");
					mdata.intPara1 = msele->IntAttribute("c");
					mdata.intPara2 = msele->IntAttribute("qu");
					mdata.floatPara3 = atof(msele->GetText());
					MapBlock::vec_randMonstersRes.push_back(mdata);
				}
				msele = msele->NextSiblingElement();
			}
		}
		else if (strcmp(dataele->Name(), "Cells") == 0)
		{
			blockRowCount = dataele->IntAttribute("rs");
			blockColCount = dataele->IntAttribute("cs");
			createMap();
			//获取第一个节点属性
			//const XMLAttribute *attribute = rootEle->FirstAttribute();
			//打印节点属性名和值
			//log("attribute<em>name = %s,attribute</em>value = %s", attribute->Name(), attribute->Value());</p>
			tinyxml2::XMLElement *element = dataele->FirstChildElement();
			while (element != NULL)
			{
				if (strcmp(element->Name(), "Cell") == 0)
				{
					int r = element->IntAttribute("r");
					int c = element->IntAttribute("c");
					int postype = element->IntAttribute("p");
					bool walkable = element->IntAttribute("w") == 1 ? true : false;
					std::string boardname = element->Attribute("m");
					const char* buildchar = element->Attribute("d");
					std::string buildname;
					if (buildchar != NULL)
						buildname = buildchar;

					MapBlock* mb = MapBlock::create(blockRowCount - 1 - r, c, boardname);
					int rc = (blockRowCount - 1 - r)*blockColCount + c;
					int zorder = r*blockColCount + c;
					m_mapscrollcontainer->addChild(mb, zorder);
					mb->setBuilding(buildname);
					mb->getTexture()->setAliasTexParameters();

					mb->setPosType(postype);
						
					mb->setWalkable(walkable);
					map_mapBlocks[rc] = mb;
					mb->setTag(rc);

					cocos2d::ui::ImageView* blockclick = cocos2d::ui::ImageView::create("mapui/blankdot.png", cocos2d::ui::Widget::TextureResType::PLIST);
					blockclick->setScale(72);
					blockclick->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBlockClick, this));
					blockclick->setAnchorPoint(Vec2(0, 0));
					blockclick->setPosition(mb->getPosition());
					blockclick->setTouchEnabled(true);
					blockclick->setSwallowTouches(isMaze);
					blockclick->setTag(rc);
					m_mapscrollcontainer->addChild(blockclick, zorder);
					if (postype == POS_START)
					{
						mb->setWalkable(true);
						vec_startpos.push_back(rc);
					}
					else if (postype == POS_BOX)
					{
						totalBoxcount++;
						//v2.01版本之前是用行列计算宝箱位置，领取后不可领取，后期修改位置会重复领取，现在加入序号
						mb->setPosType(postype);
						vec_boxblock.push_back(mb);
					}
					if ((postype > POS_START && postype < POS_MAZETRANS) || postype == POS_MAZEENTRY)
					{
						int posrange = -1;
						element->QueryIntAttribute("prs1", &posrange);
						if (posrange >= 0)
							mb->range_start.push_back((blockRowCount - 1 - posrange/ blockColCount)*blockColCount + posrange % blockColCount);
						element->QueryIntAttribute("pre1", &posrange);
						if (posrange >= 0)
							mb->range_end.push_back((blockRowCount - 1 - posrange / blockColCount)*blockColCount + posrange % blockColCount);

						element->QueryIntAttribute("prs2", &posrange);
						if (posrange >= 0)
							mb->range_start.push_back((blockRowCount - 1 - posrange / blockColCount)*blockColCount + posrange % blockColCount);
						element->QueryIntAttribute("pre2", &posrange);
						if (posrange >= 0)
							mb->range_end.push_back((blockRowCount - 1 - posrange / blockColCount)*blockColCount + posrange % blockColCount);
					}

					tinyxml2::XMLElement* e0 = element->FirstChildElement();
					while (e0 != NULL)
					{
						std::string ename = e0->Name();
						if (ename.compare("event") == 0)
						{
							std::vector<std::string> vec_tmp;
							CommonFuncs::split(e0->GetText(), vec_tmp, ";");
							int ernd = 0;
							for (unsigned int i = 0; i < vec_tmp.size(); i++)
							{
								int rnd = atoi(vec_tmp[i].c_str());
								if (rnd > 0)
								{
									ernd += rnd;
									mb->map_eventrnd[i] = ernd;
								}
							}

							if (mb->map_eventrnd.size() == 1 && mb->map_eventrnd[mb->map_eventrnd.begin()->first] >= 100)
							{
#if MAP_BET
								mb->setEventIcon(mb->map_eventrnd.begin()->first);
#else
								if (mb->map_eventrnd.begin()->first != POS_BET)//去掉bet
									mb->setEventIcon(mb->map_eventrnd.begin()->first);
#endif

							}
						}
						else if (ename.compare("npcid") == 0)
						{
							mb->setPosNpcID(e0->GetText());
							if (postype == POS_MAZETRANS)
							{
								mb->setPosNpcRnd(100);
								MAZE_POS madata;
								madata.maid = atoi(e0->GetText());
								madata.blockindex = rc;
								vec_mazetranspoints.push_back(madata);
							}

							if (postype >= POS_NPC && postype <= POS_TBOSS)
							{
								vec_npcOrBossid.push_back(mb->getPosNpcID());
							}
						}
						else if (ename.compare("npcrnd") == 0)
						{
							mb->setPosNpcRnd(atoi(e0->GetText()));
						}
						else if (ename.compare(0, 5, "msatt") == 0)
						{
							FOURProperty mdata;
							int index = atoi(ename.substr(ename.length() - 1).c_str()) - 1;
							mdata.sid = e0->Attribute("id");
							mdata.intPara1 = e0->IntAttribute("qu");
							mdata.intPara2 = atoi(e0->GetText());
							mdata.floatPara3 = 100.0f;
							mb->npcs[index] = mdata;
							if (GlobalInstance::challangeType == CH_SUPERBOSS && (postype == POS_BOSS || postype == POS_TBOSS))
								mb->npcs[index].intPara2 = GlobalInstance::supperbossinfo.bosslv;
						}
						else if (ename.compare(0, 5, "msawd") == 0)
						{
							FOURProperty mdata;
							mdata.sid = e0->Attribute("id");
							mdata.intPara1 = atoi(e0->GetText());
							mdata.intPara2 = e0->IntAttribute("qu");
							mdata.floatPara3 = e0->FloatAttribute("rnd");
							mb->vec_RewardsRes.push_back(mdata);
						}
						else if (ename.compare("A") == 0 || ename.compare("B") == 0)
						{
							ChoiceData cdata;
							cdata.content = e0->Attribute("cname");
							tinyxml2::XMLElement* e01 = e0->FirstChildElement();
							while (e01 != NULL)
							{
								std::string e01name = e0->Name();
								if (e01name.compare("lossres") == 0)
								{
									FOURProperty mdata;
									mdata.sid = e01->Attribute("id");
									mdata.intPara1 = atoi(e01->GetText());
									mdata.intPara2 = 0;
									mdata.floatPara3 = 100.0f;
									cdata.lostRes = mdata;
								}
								else if (e01name.compare("eboss") == 0)
								{
									cdata.effectNpc = e01->GetText();
								}
								else if (e01name.compare("rettype") == 0)
								{
									cdata.result = atoi(e01->GetText());
								}
								else
								{
									FOURProperty mdata;
									mdata.sid = e01->Attribute("id");
									mdata.intPara1 = atoi(e01->GetText());
									mdata.intPara2 = e01->IntAttribute("qu");
									mdata.floatPara3 = e01->FloatAttribute("rnd");
									cdata.vec_getRes.push_back(mdata);
								}
								e01 = e01->NextSiblingElement();
							}
							mb->vec_choiceDatas.push_back(cdata);
						}
						e0 = e0->NextSiblingElement();
					}

					if ((postype >= POS_NPC && postype <= POS_TBOSS) || postype == POS_MAZETRANS || postype == POS_MAZEENTRY)//起点有多个，只会显示一个，不在这里设置
					{
						bool showPosIcon = true;
						int r = GlobalInstance::getInstance()->createRandomNum(100);
						if (r >= mb->getPosNpcRnd())
							showPosIcon = false;

						if (postype == POS_MAZEENTRY)
						{
							iscfgmazeentry = true;
							if (GlobalInstance::ishasmazeentry)
								showPosIcon = true;
						}

						if (postype >= POS_BOSS && postype <= POS_TBOSS)
						{
							totalBoxcount++;
						}

						if (showPosIcon)
						{
							mb->setPosType(postype);

							if (mb->range_start.size() <= 0)//需要随机出现时不在这里显示
								mb->setPosIcon();
							else
								map_rangeblocks[postype].push_back(mb);
						}
						else
						{
							mb->setPosType(-1);
						}
						if (isMaze && showPosIcon && (mb->getBuildName().compare("8.png") == 0 || mb->getBuildName().compare("51.png") == 0))
						{
							mb->setPosIconVisable(false);
						}
					}
				}
				element = element->NextSiblingElement();
			}
		}
		dataele = dataele->NextSiblingElement();
	}
	delete pDoc;

	//宝箱位置， 加上ID
	std::sort(vec_boxblock.begin(), vec_boxblock.end(), sortByBoxPos);

	int count = 0;
	std::vector<MapBlock*>::iterator it;
	for (it = vec_boxblock.begin(); it != vec_boxblock.end();)
	{
		MapBlock* mblock = *it;

		int mid = atoi(mblock->getPosNpcID().c_str());
		std::string key = StringUtils::format("%s-%d", m_mapid.c_str(), mblock->getTag());
		if (DataSave::getInstance()->getOldMapBoxRewards(key))
		{
			DataSave::getInstance()->deleteDataByKey(key);
			DataSave::getInstance()->setMapBoxRewards(m_mapid, mid, 1);
		}

		if (DataSave::getInstance()->getMapBoxRewards(m_mapid, mid) < mblock->getPosNpcRnd())
		{
			if (mblock->range_start.size() <= 0)//需要随机出现时不在这里显示
			{
				mblock->setPosIcon();

				if (isMaze && (mblock->getBuildName().compare("8.png") == 0 || mblock->getBuildName().compare("51.png") == 0))
				{
					mblock->setPosIconVisable(false);
				}
			}
			else
				map_rangeblocks[POS_BOX].push_back(mblock);

			it++;
		}
		else
		{
			mblock->setPosType(-1);
			it = vec_boxblock.erase(it);
		}
		count++;
	}
}

bool MapBlockScene::sortByBoxPos(MapBlock* a, MapBlock *b)
{
	if (a->getTag() < b->getTag())
		return true;
	return false;
}

void MapBlockScene::setBlockRange()
{

	std::map<int, std::vector<MapBlock*>>::iterator rit;

	for (rit = map_rangeblocks.begin(); rit != map_rangeblocks.end();)
	{
		for (unsigned int n = 0; n < rit->second.size(); n++)
		{
			MapBlock* mblock = rit->second.at(n);
			int rindex = GlobalInstance::getInstance()->createRandomNum(mblock->range_start.size());
			int s = mblock->range_start[rindex];
			int e = mblock->range_end[rindex];
			int r1 = s / blockColCount;
			int c1 = s % blockColCount;
			int r2 = e / blockColCount;
			int c2 = e % blockColCount;

			if (r1 > r2)
			{
				int swap = r1;
				r1 = r2;
				r2 = swap;
			}
			if (c1 > c2)
			{
				int swap = c1;
				c1 = c2;
				c2 = swap;
			}
			//int rnd_r = GlobalInstance::getInstance()->createRandomNum(abs(r2 - r1) + 1);
			//if (r2 - r1 < 0)
			//	rnd_r = r1 - rnd_r;
			//else
			//	rnd_r = r1 + rnd_r;

			//int rnd_c = GlobalInstance::getInstance()->createRandomNum(abs(c2 - c1) + 1);
			//if (c2 - c1 < 0)
			//	rnd_c = c1 - rnd_c;
			//else
			//	rnd_c = c1 + rnd_c;

			//int rc = rnd_r * blockColCount + rnd_c;

			std::vector<MapBlock*> normalblock;
			normalblock.push_back(mblock);
			for (int i = r1; i <= r2; i++)
			{
				for (int m = c1; m <= c2; m++)
				{
					int rc = i * blockColCount + m;
					if (map_mapBlocks[rc]->getPosType() == POS_NOTHING && map_mapBlocks[rc]->getWalkable() && map_mapBlocks[rc]->map_eventrnd.size() <= 0)
					{
						normalblock.push_back(map_mapBlocks[rc]);
					}
				}
			}

			int rselect = GlobalInstance::getInstance()->createRandomNum(normalblock.size());
			if (normalblock[rselect]->getTag() == mblock->getTag())
			{
				mblock->setPosIcon();
			}
			else
			{
				int postype = mblock->getPosType();
				mblock->removePosIcon();
				mblock->setPosType(-1);

				bool issee = mblock->getIsCanSee();
				mblock->setIsCanSee(normalblock[rselect]->getIsCanSee());
				std::string npcid = mblock->getPosNpcID();
				mblock->setPosNpcID("");
				int npcrnd = mblock->getPosNpcRnd();
				mblock->setPosNpcRnd(0);

				normalblock[rselect]->setPosType(postype);
				normalblock[rselect]->setIsCanSee(issee);
				normalblock[rselect]->setPosNpcID(npcid);
				normalblock[rselect]->setPosNpcRnd(npcrnd);
				normalblock[rselect]->vec_RewardsRes = mblock->vec_RewardsRes;
				normalblock[rselect]->vec_choiceDatas = mblock->vec_choiceDatas;
				for (unsigned int m = 0; m < 6; m++)
				{
					FOURProperty propty = mblock->npcs[m];
					if (propty.sid.length() > 0)
					{
						normalblock[rselect]->npcs[m] = propty;
					}
					FOURProperty epropty;
					mblock->npcs[m] = epropty;
				}
				normalblock[rselect]->setPosIcon();

				mblock->vec_RewardsRes.clear();
				mblock->vec_choiceDatas.clear();
			}

		}
		rit++;
	}
}

void MapBlockScene::calcStar(int ctype)
{
	if (!isMaze && !isNewerGuideMap)
	{
		bool isfind = false;
		for (unsigned int i = 0; i < GlobalInstance::vec_stardata.size(); i++)
		{
			if (GlobalInstance::vec_stardata[i].sid == ctype)
			{
				GlobalInstance::vec_stardata[i].finishcount++;
				if (GlobalInstance::vec_stardata[i].finishcount == GlobalInstance::vec_stardata[i].needcount)
				{
					std::string str = DataSave::getInstance()->getFinishStar(m_mapid);
					if (str.length() > 0)
						str.append(",");
					std::string  fstr = StringUtils::format("%d", ctype);
					str.append(fstr);
					DataSave::getInstance()->setFinishStar(m_mapid, str);
				}
				break;
			}
		}
	}
}

void MapBlockScene::checkotherstar() 
{
	if (!isMaze && !isNewerGuideMap)
	{
		int boxcount = 0;
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();

		std::string content = GlobalInstance::getInstance()->getUserDefaultXmlString(1);

		int err = pDoc->Parse(content.c_str());
		if (err != 0)
		{
			delete pDoc;
		}
		tinyxml2::XMLElement *rootEle = pDoc->RootElement();

		tinyxml2::XMLElement *element = rootEle->FirstChildElement();
		while (element != NULL)
		{
			std::string key = element->Name();

			std::string findstr = StringUtils::format("jhbr%s", m_mapid.c_str());
			if (key.find(findstr) != std::string::npos)
			{
				boxcount++;
			}
			element = element->NextSiblingElement();
		}

		if (boxcount >= totalBoxcount)
		{
			calcStar(SA_GETALLBOX);
		}

		for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
		{
			if (GlobalInstance::vec_TaskMain[i].place.compare(m_mapid) == 0 && GlobalInstance::vec_TaskMain[i].isfinish >= QUEST_FINISH)
			{
				calcStar(SA_FINISH_MAINTASK);
				break;
			}
		}
		for (unsigned int i = 0; i < GlobalInstance::vec_TaskBranch.size(); i++)
		{
			if (GlobalInstance::vec_TaskBranch[i].place.compare(m_mapid) == 0 && GlobalInstance::vec_TaskBranch[i].isfinish >= QUEST_FINISH)
			{
				calcStar(SA_FINISH_BRANCHTASK);
				break;
			}
		}

		std::string str = DataSave::getInstance()->getMapVisibleArea(m_mapid);

		std::vector<std::string> vec_cfg;
		CommonFuncs::split(str, vec_cfg, ";");


		if (vec_cfg.size() >= 2)
		{
			if (vec_cfg[0].compare("-1") == 0)
			{
				calcStar(SA_USEALLOPEN);
			}
		}

		for (unsigned int i = 0; i < vec_npcOrBossid.size(); i++)
		{
			std::string npcid = vec_npcOrBossid[i];

			std::map<std::string, NpcFriendly>::iterator it;

			for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); ++it)
			{
				std::string nid = it->first;
				if (npcid.compare(nid) == 0)
				{
					for (unsigned int n = 0; n < it->second.relation.size(); n++)
					{
						if (it->second.relation[n] == NPC_FRIEND)
						{
							calcStar(SA_BEFRIEND);
						}
						else if (it->second.relation[n] == NPC_MASTER)
						{
							calcStar(SA_BEMASTER);
						}
						else if (it->second.relation[n] == NPC_COUPEL)
						{
							calcStar(SA_BECOMPLE);
						}
					}
				}
			}
		}
	}
}

bool MapBlockScene::checkShowStarUi(int cwhere)
{
	int index = m_mapid.find_last_of("-");
	std::string mainid = m_mapid.substr(0, index);

	if (mainid.compare("m1-5") == 0 || mainid.compare("m1-6") == 0)
		return false;

	checkotherstar();

	int star = 0;
	for (unsigned int i = 0; i < GlobalInstance::vec_stardata.size(); i++)
	{
		if (GlobalInstance::vec_stardata[i].finishcount >= GlobalInstance::vec_stardata[i].needcount)
		{
			star++;
		}
	}
	if (star > GlobalInstance::curMapFinishStars)
	{
		std::string mapid = m_mapid;
		if (isMaze)
		{
			mapid = GlobalInstance::eventfrommapid;
		}
		StarResultLayer* layer = StarResultLayer::create(mapid, cwhere);
		this->addChild(layer, 1);
		AnimationEffect::openAniEffect(layer);

		return true;
	}
	return false;
}

bool MapBlockScene::isValidAtWallColRow(Vec2& colrow)
{
	if (colrow.x < 0 || colrow.y < 0 ||
		colrow.x >= blockColCount ||
		colrow.y >= blockRowCount) {
		return false;
	}
	else
	{
		if (map_mapBlocks[colrow.y * blockColCount + colrow.x]->getWalkable())
			return true;
		else
			return false;
	}
}

void MapBlockScene::goToDest(int row, int col)
{
	if (row > mycurRow && mycurCol == col)
	{
		//上；
		go(KEY_UP);
	}
	else if (row < mycurRow && mycurCol == col)
	{
		go(KEY_DOWN);
	}
	else if (row == mycurRow && col < mycurCol)
	{
		go(KEY_LEFT);
	}
	else if (row == mycurRow && col > mycurCol)
	{
		go(KEY_RIGHT);
	}
}

Node* MapBlockScene::getRoutingAnimNode()
{
	return sitetext;
}

void MapBlockScene::showRouting(std::vector<Vec2> vec_routs)
{
	//Vec2 mycurpos = Vec2(mycurCol, mycurRow);
	//vec_routs.insert(vec_routs.begin(), mycurpos);
	m_vecrouts = vec_routs;
	for (unsigned int i = 0; i < vec_routs.size(); i++)
	{
		int c = (int)vec_routs[i].x;
		int r = (int)vec_routs[i].y;
		Sprite* routblock = Sprite::createWithSpriteFrameName("ui/routblock.png");
		int bindex = r * blockColCount + c;
		routblock->setAnchorPoint(Vec2(0, 1));
		routblock->setPosition(Vec2(c*MAPBLOCKWIDTH, (r + 1)*MAPBLOCKHEIGHT));
		std::string routname = StringUtils::format("routblock%d", bindex);

		int zorder = map_mapBlocks[bindex]->getLocalZOrder() + 10000;
		if (!mapIsAllOpen && !map_mapBlocks[bindex]->getIsCanSee() && c > fogscale / 2-1 && r > fogscale / 2-1)
			zorder = 30000;
		map_mapBlocks[bindex]->getParent()->addChild(routblock, zorder, routname);
		routblock->setVisible(false);
		int routslen = vec_routs.size();
		if (i == routslen - 1)
		{
			Sprite* rdesc = Sprite::createWithSpriteFrameName("ui/routdesticon.png");
			rdesc->setAnchorPoint(Vec2(0.5f, 0));
			rdesc->setVisible(false);
			rdesc->setPosition(Vec2(routblock->getPositionX() + MAPBLOCKWIDTH/2, routblock->getPositionY() - MAPBLOCKWIDTH/2));
			std::string rdescname = StringUtils::format("routdest%d", bindex);
			map_mapBlocks[bindex]->getParent()->addChild(rdesc, 30000, rdescname);
			rdesc->runAction(Sequence::create(DelayTime::create(0.02f* routslen), Show::create(), NULL));
			rdesc->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.3f, Vec2(0, 5)), MoveBy::create(0.6f, Vec2(0, -5)), MoveBy::create(0.5f, Vec2(0, 0)), NULL)));
			
			std::string countstr = StringUtils::format("%d", routslen);
			Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 28);
			countlbl->setColor(Color3B(255, 255, 255));
			countlbl->setPosition(Vec2(rdesc->getContentSize().width / 2, -20));
			rdesc->addChild(countlbl);
		}

		routblock->runAction(Sequence::create(DelayTime::create(0.02f* i), Show::create(), NULL));
	}
}

void MapBlockScene::removeCurRouting(int row, int col)
{
	int bindex = row * blockColCount + col;
	std::string routname = StringUtils::format("routblock%d", bindex);
	Node* rnode = map_mapBlocks[bindex]->getParent()->getChildByName(routname);

	if (rnode != NULL)
	{
		map_mapBlocks[bindex]->getParent()->removeChildByName(routname);

		std::string rdescname = StringUtils::format("routdest%d", bindex);
		Node* dnode = map_mapBlocks[bindex]->getParent()->getChildByName(rdescname);
		if (dnode != NULL)
			map_mapBlocks[bindex]->getParent()->removeChildByName(rdescname);
	}

	
	for (unsigned int i = 0; i < m_vecrouts.size(); i++)
	{
		int c = (int)m_vecrouts[i].x;
		int r = (int)m_vecrouts[i].y;
		int bindex = r * blockColCount + c;

		std::string routname = StringUtils::format("routblock%d", bindex);
		Node* rnode = map_mapBlocks[bindex]->getParent()->getChildByName(routname);
		if (rnode != NULL)
		{
			int zorder = map_mapBlocks[bindex]->getLocalZOrder() + 10000;
			if (!mapIsAllOpen && !map_mapBlocks[bindex]->getIsCanSee() && c > fogscale / 2-1 && r > fogscale / 2-1)
				zorder = 30000;
			rnode->setLocalZOrder(zorder);
		}
	}
}

void MapBlockScene::removeAllRoutingBlock()
{
	isMovingRouting = false;
	for (unsigned int i = 0; i < m_vecrouts.size(); i++)
	{
		int c = (int)m_vecrouts[i].x;
		int r = (int)m_vecrouts[i].y;
		int bindex = r * blockColCount + c;

		std::string routname = StringUtils::format("routblock%d", bindex);
		Node* rnode = map_mapBlocks[bindex]->getParent()->getChildByName(routname);
		if (rnode != NULL)
		{
			map_mapBlocks[bindex]->getParent()->removeChildByName(routname);

			std::string rdescname = StringUtils::format("routdest%d", bindex);
			Node* dnode = map_mapBlocks[bindex]->getParent()->getChildByName(rdescname);
			if (dnode != NULL)
				map_mapBlocks[bindex]->getParent()->removeChildByName(rdescname);
		}
	}

	map_mapBlocks[0]->getParent()->removeChildByName("bossquan");
	g_MapBlockScene->removeChildByName("cannottouchlayer");

}

void MapBlockScene::showBuySelectFood()
{
	std::vector< MSGAWDSDATA> vec_res;
	int count[] = { 50, 100, 200 };
	for (unsigned int i = 0; i < 3; i++)
	{
		MSGAWDSDATA rdata;
		rdata.rid = "r001";
		rdata.count = count[i];
		rdata.qu = 0;
		vec_res.push_back(rdata);
	}
	BuySelectLayer* layer = BuySelectLayer::create(vec_res, MYPACKAGE);
	this->addChild(layer);
	AnimationEffect::openAniEffect(layer);
}

void MapBlockScene::checkMazeStoneHint()
{

	if (isMaze)
	{
		int checkret = 0;

		std::vector<int> vec_;

		for (int i = m_walkDirection + KEY_UP; i <= KEY_RIGHT; i++)
		{
			vec_.push_back(i);
		}

		for (int i = KEY_UP; i < m_walkDirection + KEY_UP; i++)
		{
			vec_.push_back(i);
		}

		int bindex = 0;

		for (unsigned int i = 0; i < vec_.size(); i++)
		{
			switch (vec_[i])
			{
				case KEY_UP:
					bindex = (mycurRow + 1)*blockColCount + mycurCol;
					break;
				case KEY_DOWN:
					bindex = (mycurRow - 1)*blockColCount + mycurCol;
					break;
				case KEY_LEFT:
					bindex = (mycurRow)*blockColCount + mycurCol - 1;
					break;
				case KEY_RIGHT:
					bindex = (mycurRow)*blockColCount + mycurCol + 1;
					break;
				default:
					break;
			}

			if (!map_mapBlocks[bindex]->getWalkable())
			{
				std::string bname = map_mapBlocks[bindex]->getBuildName();
				if (isMaze && (bname.compare("8.png") == 0 || bname.compare("51.png") == 0))
				{
					std::string buildname = StringUtils::format("mapui/focusbuildblock_%s", bname.c_str());
					buildfocus->loadTexture(buildname, cocos2d::ui::Widget::TextureResType::PLIST);

					buildfocus->setPosition(Vec2(bindex % blockColCount * MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, MAPBLOCKHEIGHT / 2 + bindex / blockColCount * MAPBLOCKHEIGHT));
					buildfocus->setVisible(true);

					buildfocus->setTag(bindex);
					buildfocus->getChildByName("z002")->setTag(bindex);
					std::string zcountstr = StringUtils::format("%d", MyRes::getMyResCount("z002", MYPACKAGE) + MyRes::getMyResCount("z002", MYSTORAGE));
					z002countlbl->setString(zcountstr);
					//map_mapBlocks[bindex]->focusBuild();

					if (GlobalInstance::showz002hinttextcount < 3)
					{
						MovingLabel::show(ResourceLang::map_lang["mazestone"]);
						GlobalInstance::showz002hinttextcount++;
					}
					break;
				}
			}

		}
	}
}

void MapBlockScene::zoomGuideEnd()
{
	scrollView->setZoomScale(0.71f);

	if (NewGuideLayer::checkifNewerGuide(1))
	{
		CannotTouchLayer* notTouchLayer = CannotTouchLayer::create();
		this->addChild(notTouchLayer, 1, "cannottouchlayer");
		this->scheduleOnce(schedule_selector(MapBlockScene::delayShowNewerGuide), 0.5f);
	}
}

void MapBlockScene::onFinish(int code)
{
	Director::getInstance()->getRunningScene()->removeChildByName("waitingprogress");
	ErrorHintLayer* networkerrLayer = (ErrorHintLayer*)this->getChildByName("networkerrorlayer");
	if (code == SUCCESS)
	{
		if (networkerrLayer != NULL)
			networkerrLayer->removeFromParentAndCleanup(true);

		updateTime(0);
		this->schedule(schedule_selector(MapBlockScene::updateTime), 1);

		long long int timeSec = GlobalInstance::servertime + 8 * 60 * 60;
		time_t timep = timeSec;
		tm* timeStuct = gmtime(&timep);

		if (timeStuct->tm_hour < GlobalInstance::supperbossinfo.starthour || timeStuct->tm_hour >= GlobalInstance::supperbossinfo.endhour)
		{
			std::string str = StringUtils::format(ResourceLang::map_lang["supperbosstimetext"].c_str(), GlobalInstance::supperbossinfo.starthour, GlobalInstance::supperbossinfo.endhour);
			MovingLabel::show(str);
			return;
		}

		if (GlobalInstance::supperbossinfo.leftfreecount <= 0 && GlobalInstance::supperbossinfo.leftcoincount > 0)
		{
			DynamicValueInt dv;
			dv.setValue(20);
			GlobalInstance::getInstance()->costMyCoinCount(dv);
		}

		showFightingLayer(vec_enemys);
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

void MapBlockScene::updateTime(float dt)
{
	GlobalInstance::servertime++;

	int zerotime = GlobalInstance::servertime + 8 * 60 * 60;
}

void MapBlockScene::showBossGuideAnim(Vec2 pos)
{
	auto quan = Sprite::create("images/bossguide/1.png");
	quan->setAnchorPoint(Vec2(0.5, 0.5));
	quan->setPosition(pos.x+ MAPBLOCKWIDTH/2, pos.y+ MAPBLOCKWIDTH/2);
	map_mapBlocks[0]->getParent()->addChild(quan, 40000, "bossquan");

	//创建帧动画序列，名词形式
	auto animation = Animation::create();
	for (int i = 1; i <= 20; i++)
	{
		std::string str = StringUtils::format("images/bossguide/%d.png", i);
		animation->addSpriteFrameWithFile(str);
	}
	//设置帧动画属性
	animation->setDelayPerUnit(0.1f);//每一帧停留的时间
	animation->setRestoreOriginalFrame(true);//播放完后回到第一帧
	auto animate = Animate::create(animation);
	quan->runAction(RepeatForever::create(Sequence::create(animate, DelayTime::create(0.2f), NULL)));
}
