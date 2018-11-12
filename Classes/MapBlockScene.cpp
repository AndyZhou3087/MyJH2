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

MapBlockScene* g_MapBlockScene = NULL;

#define DEFAULTRND 10
#define HEROOFFSET_Y 20
std::string walkname[] = {"walk_up", "walk_d", "walk_l", "walk_l"};
std::string standname[] = { "stand_up", "stand_d", "stand_l", "stand_l" };
MapBlockScene::MapBlockScene()
{
	myposHero = NULL;
	isMoving = false;

	m_isLongPress = false;
	m_longTouchNode = NULL;
	walkcount = 0;
	monsterComeRnd = DEFAULTRND;
	fogscale = 5.0f;
	m_fightbgtype = 0;
	m_walkDirection = 0;
	m_lastWalkDirection = 0;
	_fogrender = NULL;
	mapIsAllOpen = false;
	randStartPos = -1;
	firstpostype = -1;
}


MapBlockScene::~MapBlockScene()
{
	GlobalInstance::getInstance()->recoveCardHeroMaxHp();
	g_MapBlockScene = NULL;

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

	m_fightbgtype = bgtype;

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("mapBlockLayer.csb"));
	this->addChild(m_csbnode);

	std::u32string utf32String;
	StringUtils::UTF8ToUTF32(GlobalInstance::map_AllResources[mapname].name, utf32String);

	int u32strlen = utf32String.length();
	cocos2d::ui::ImageView* mapnamebox = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("mapnamebox");
	cocos2d::ui::Text* mapnamelbl = (cocos2d::ui::Text*)m_csbnode->getChildByName("mapname");
	mapnamebox->setContentSize(Size(70 + (mapnamelbl->getFontSize() + 1) * u32strlen, mapnamebox->getContentSize().height));
	mapnamelbl->setString(GlobalInstance::map_AllResources[mapname].name);

	Node* topnode = m_csbnode->getChildByName("mapblocktop");

	carrycountlbl = (cocos2d::ui::Text*)topnode->getChildByName("carrycount");
	foodcountlbl = (cocos2d::ui::Text*)topnode->getChildByName("r001count");
	solivercountlbl = (cocos2d::ui::Text*)topnode->getChildByName("solivercountlbl");

	sitelbl = (cocos2d::ui::Text*)topnode->getChildByName("site");

	int vsionLv = MyRes::getMyResCount("v001", MYSTORAGE);

	fogscale += vsionLv;

	resetBlockData();

	parseMapXml(mapname);

	createFog();

	int count = vec_startpos.size();

	if (randStartPos < 0)
	{
		int startposindex = GlobalInstance::getInstance()->createRandomNum(count);
		randStartPos = vec_startpos[startposindex];
	}
	mycurCol = randStartPos % blockColCount;
	mycurRow = randStartPos / blockColCount;
	map_mapBlocks[randStartPos]->setPosIcon();

	//地图引导过后是否有记录
	if (!NewGuideLayer::checkifNewerGuide(FIRSTGUIDESTEP) && DataSave::getInstance()->getExitScene() == 2)
	{
		std::string str = DataSave::getInstance()->getMapScenePos();
		std::vector<std::string> vec_map;
		CommonFuncs::split(str, vec_map, ",");
		if (vec_map.size() > 1)
		{
			int myStartPos = atoi(vec_map[1].c_str());
			mycurCol = myStartPos % blockColCount;
			mycurRow = myStartPos / blockColCount;
		}
	}

	std::map<int, MapBlock*>::iterator it;

	for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
	{
		MapBlock* block = map_mapBlocks[it->first];
		if (block->getPosType() == POS_NOTHING && block->getWalkable() && block->map_eventrnd.size() <= 0)
			vec_normalBlocks.push_back(block);
	}

	std::random_shuffle(vec_normalBlocks.begin(), vec_normalBlocks.end());

	int tcount = vec_normalBlocks.size()/18;

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
			if (off <= 3)
			{
				ret = false;
				break;
			}
		}
		if (ret)
		{
			vec_monsterBlocks.push_back(*mit);
			count++;
		}
		mit = vec_normalBlocks.erase(mit);

	}

	for (unsigned int i = 0; i < vec_monsterBlocks.size(); i++)
	{
		vec_monsterBlocks[i]->setPosType(POS_MONSTER);
		vec_monsterBlocks[i]->setPosIcon();
	}

	setMyPos();

	updateLabel(0);
	this->schedule(schedule_selector(MapBlockScene::updateLabel), 1.0f);

	Node* bottomnode = m_csbnode->getChildByName("mapblockbottom");

	gocitybtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("gocitybtn");
	gocitybtn->setTag(BTN_GOCITY);
	gocitybtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));

	cocos2d::ui::Widget* mapbtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("mapbtn");
	mapbtn->setTag(BTN_MAP);
	mapbtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));

	cocos2d::ui::Widget* mypackagebtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("packagebtn");
	mypackagebtn->setTag(BTN_PACKAGE);
	mypackagebtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));

	std::string keyname[] = {"upbtn", "downbtn", "leftbtn", "rightbtn"};

	for (int i = 0; i < 4; i++)
	{
		cocos2d::ui::Widget* keybtn = (cocos2d::ui::Widget*)bottomnode->getChildByName(keyname[i]);
		keybtn->setTag(i + KEY_UP);
		keybtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onArrowKey, this));
		keybtnArr[i] = keybtn;
	}

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(60 + 120 * i, 1196);
		fightHeroNode->setScale(0.84f);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i], F_HERO, FS_READY);
		addChild(fightHeroNode, 0, i);
	}

	//scheduleUpdate();

	loadTaskUI();

	int r = GlobalInstance::getInstance()->createRandomNum(5);
	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_SUBMAP_0 + r);

	this->scheduleOnce(schedule_selector(MapBlockScene::closeTaskTipNode), 10.0f);

	this->scheduleOnce(schedule_selector(MapBlockScene::delayShowNewerGuide), 0.1f);

    //记录位置
    DataSave::getInstance()->setExitScene(2);
	DataSave::getInstance()->setMapScenePos(m_mapid, mycurRow*blockColCount + mycurCol);
	DataSave::getInstance()->setHeroMapCarryCount(GlobalInstance::myOutMapCarry);

	return true;
}

void MapBlockScene::showNewerGuideFight()
{
	showFightingLayer(vec_enemys);
}

void MapBlockScene::delayShowNewerGuide(float dt)
{
	if (NewGuideLayer::checkifNewerGuide(FIRSTGUIDESTEP))
	{
		if (NewGuideLayer::checkifNewerGuide(0))
		{
			showNewerGuide(0);
		}
		else if (NewGuideLayer::checkifNewerGuide(1))
		{
			showNewerGuide(1);
		}
		else if (NewGuideLayer::checkifNewerGuide(2))
		{
			showNewerGuide(2);
		}
		else if (NewGuideLayer::checkifNewerGuide(3))
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
		else if (NewGuideLayer::checkifNewerGuide(11))
		{
			showNewerGuide(11);
		}
		/*else if (NewGuideLayer::checkifNewerGuide(12))
		{
			showNewerGuide(12);
		}
		else if (NewGuideLayer::checkifNewerGuide(13))
		{
			showNewerGuide(13);
		}*/
	}
	else if (getFirstFightBoss() && NewGuideLayer::checkifNewerGuide(86))
	{
		showNewerGuide(86);
	}
}

void MapBlockScene::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 0 || step == 1)
	{
		nodes.push_back(keybtnArr[0]);
	}
	else if ((step >= 2 && step <= 5) || step == 12)
	{

	}
	else if (step == 11 || step == 13)
	{
		nodes.push_back(keybtnArr[2]); 
	}
	else if (step == 86)
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

void MapBlockScene::goBackMainHomeScene()
{
	cacelLongTouch();
	for (int i = 0; i < 4; i++)
	{
		keybtnArr[i]->setEnabled(false);
	}
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
}

void MapBlockScene::showNewerGuideGoBack()
{
	goBackMainHomeScene();
}

void MapBlockScene::showFightingLayer(std::vector<Npc*> enemys)
{
	/*FightingLayer* layer = FightingLayer::create(enemys, m_fightbgtype);
	this->addChild(layer);*/
	CutScenesLayer* cutlayer = CutScenesLayer::create(enemys, m_fightbgtype);
	this->addChild(cutlayer, 0, "CutScenesLayer");
}

void MapBlockScene::loadTaskUI()
{
	int langtype = GlobalInstance::getInstance()->getLang();
	//添加任务提示框
	m_tasknode = CSLoader::createNode(ResourcePath::makePath("taskTipsNode.csb"));
	this->addChild(m_tasknode);
	m_tasknode->setPosition(Vec2(589, 955));

	cocos2d::ui::ImageView* tasktitle = (cocos2d::ui::ImageView*)m_tasknode->getChildByName("title");
	tasktitle->loadTexture(ResourcePath::makeTextImgPath("maptask_title", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* hidebtn = (cocos2d::ui::ImageView*)m_tasknode->getChildByName("hidebtn");
	hidebtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onTaskAction, this));
	hidebtn->setTag(1);
	questbtn = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("questbtn");
	questbtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onTaskAction, this));
	questbtn->setTag(0);
	questbtn->setVisible(false);
	cocos2d::ui::ImageView* questbtn2 = (cocos2d::ui::ImageView*)questbtn->getChildByName("questbtn2");
	questbtn2->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onTaskAction, this));
	questbtn2->setTag(0);

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
		std::string mainid = GlobalInstance::myCurMainData.place.substr(0, GlobalInstance::myCurMainData.place.find_last_of("-"));
		S_SubMap submap = GlobalInstance::map_mapsdata[mainid].map_sublist[GlobalInstance::myCurMainData.place];
		std::string str = StringUtils::format(ResourceLang::map_lang["tasktips"].c_str(), GlobalInstance::myCurMainData.name.c_str(), GlobalInstance::map_AllResources[submap.id].name.c_str(), GlobalInstance::map_AllResources[GlobalInstance::myCurMainData.npcid].name.c_str());
		textmain->setString(str);
	}
	else
	{
		if (GlobalInstance::myCurMainData.id == 1)
		{
			textmain->setString(ResourceLang::map_lang["nottasktext"]);
		}
		else
		{
			textmain->setTextColor(Color4B(255, 0, 0, 255));
			textmain->setString(ResourceLang::map_lang["finishtasktext"]);
		}
	}

	//支线
	if (Quest::getBranchQuest())
	{
		std::string mainid = GlobalInstance::myCurBranchData.place.substr(0, GlobalInstance::myCurBranchData.place.find_last_of("-"));
		S_SubMap submap = GlobalInstance::map_mapsdata[mainid].map_sublist[GlobalInstance::myCurBranchData.place];
		std::string str = StringUtils::format(ResourceLang::map_lang["tasktips"].c_str(), GlobalInstance::myCurBranchData.name.c_str(), GlobalInstance::map_AllResources[submap.id].name.c_str(), GlobalInstance::map_AllResources[GlobalInstance::myCurBranchData.npcid].name.c_str());
		textbranch->setString(str);
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
			textbranch->setTextColor(Color4B(255, 0, 0, 255));
			textbranch->setString(ResourceLang::map_lang["finishtasktext"]);
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
		case BTN_MAP:
		{
			GoBackLayer* layer = GoBackLayer::create(1);
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case BTN_PACKAGE:
		{
			MyPackageLayer* layer = MyPackageLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		break;
		default:
			break;
		}
	}
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
			questbtn->setVisible(false);
			MoveTo* moveto = MoveTo::create(0.15f, Vec2(589, m_tasknode->getPositionY()));
			m_tasknode->runAction(moveto);
		}
			break;
		case 1:
		{
			questbtn->setVisible(true);
			MoveTo* moveto = MoveTo::create(0.15f, Vec2(860, m_tasknode->getPositionY()));
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
	questbtn->setVisible(true);
	MoveTo* moveto = MoveTo::create(0.15f, Vec2(860, m_tasknode->getPositionY()));
	m_tasknode->runAction(moveto);
}

void MapBlockScene::onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(MapBlockScene::longTouchUpdate)))
			schedule(schedule_selector(MapBlockScene::longTouchUpdate), 0.45f);
		if (!m_isLongPress)
			go((MAP_KEYTYPE)clicknode->getTag());
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		//if (!m_isLongPress)
		//	go((MAP_KEYTYPE)clicknode->getTag());
		cacelLongTouch();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cacelLongTouch();
	}
}

void MapBlockScene::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		go((MAP_KEYTYPE)m_longTouchNode->getTag());
		unschedule(schedule_selector(MapBlockScene::longTouchUpdate));
	}
}

void MapBlockScene::cacelLongTouch()
{
	unschedule(schedule_selector(MapBlockScene::longTouchUpdate));
	m_longTouchNode = NULL;
	m_isLongPress = false;
}

void MapBlockScene::go(MAP_KEYTYPE keyArrow)
{
	if (isMoving)
		return;

	if (!checkRoad(keyArrow))
	{
		myposHero->setAnimation(0, standname[m_walkDirection], false);
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

	m_walkDirection = keyArrow - KEY_UP;

	setMyPos();
	walkcount++;
	monsterComeRnd += (5 + walkcount);

	MyRes::Use("r001", 1, MYPACKAGE);

	if (mycurCol == randStartPos % blockColCount && mycurRow == randStartPos / blockColCount)
	{
		cacelLongTouch();
		this->scheduleOnce(schedule_selector(MapBlockScene::delayShowExit), 0.45f);
	}
	else
	{
		checkFood();
	}
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
	doMyStatus();
	isMoving = false;
	if (m_isLongPress) 
	{
		go((MAP_KEYTYPE)m_longTouchNode->getTag());
	}
	else
	{
		myposHero->setAnimation(0, standname[m_walkDirection], false);
		//myposHero->clearTracks();
	}
}

void MapBlockScene::checkFood()
{
	int foodcount = MyRes::getMyResCount("r001", MYPACKAGE);

	if (foodcount <= 10 && foodcount > 0)
	{
		if (foodcount % 5 == 0)
			MovingLabel::show(ResourceLang::map_lang["lackfoodhint"]);
	}
	else if (foodcount <= 0)
	{
		MovingLabel::show(ResourceLang::map_lang["nofoodhint"], Color4B(Color3B(204,4,4)));

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
		if (!checklive())
		{
			goBackMainHomeScene();
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

bool MapBlockScene::checkRoad(MAP_KEYTYPE keyArrow)
{
	int bindex = 0;
	switch (keyArrow)
	{
		case KEY_UP:
		{
			bindex = (mycurRow + 1)*blockColCount + mycurCol;
			if (mycurRow >= blockRowCount - 1)
				return false;
		}
		break;
		case KEY_DOWN:
		{
			bindex = (mycurRow - 1)*blockColCount + mycurCol;
			if (mycurRow <= 0)
				return false;
		}
		break;
		case KEY_LEFT:
		{
			bindex = (mycurRow)*blockColCount + mycurCol - 1;
			if (mycurCol <= 0)
				return false;
			myposHero->getSkeleton()->flipX = 0;
		}
		break;
		case KEY_RIGHT:
		{
			bindex = (mycurRow)*blockColCount + mycurCol + 1;
			if (mycurCol >= blockColCount - 1)
				return false;
			myposHero->getSkeleton()->flipX = 1;
		}
		break;
		default:
			break;
	}
	if (!map_mapBlocks[bindex]->getWalkable())
	{
		MovingLabel::show(ResourceLang::map_lang["noroad"]);
		return false;
	}
	return true;
}

void MapBlockScene::createMap()
{
	scrollView = ScrollView::create();
	scrollView->setViewSize(Size(720, 825));
	scrollView->setPosition(0, 242);
	m_csbnode->addChild(scrollView, -1);

	int mapW = blockColCount * MAPBLOCKWIDTH;
	int mapH = blockRowCount* MAPBLOCKHEIGHT;
	m_mapscrollcontainer = LayerColor::create(Color4B(0, 0, 0, 255));
	m_mapscrollcontainer->setContentSize(Size(blockColCount * MAPBLOCKWIDTH, blockRowCount* MAPBLOCKHEIGHT));

	float scalex = scrollView->getViewSize().width / mapW;
	float scaley = scrollView->getViewSize().height / mapH;
	scrollView->setContainer(m_mapscrollcontainer);

	scrollView->setTouchEnabled(true);
	scrollView->setDirection(ScrollView::Direction::BOTH);
	scrollView->setMinScale(MIN(scalex, scaley));
	scrollView->setMaxScale(2);
	scrollView->setBounceable(true);
	scrollView->setDelegate(this);

	float offsetx = m_mapscrollcontainer->getContentSize().width / 2 - scrollView->getViewSize().width / 2;
	float offsety = m_mapscrollcontainer->getContentSize().height / 2 - scrollView->getViewSize().height / 2;

	scrollView->setContentOffset(Vec2(-offsetx, -offsety));
}

void MapBlockScene::scrollViewDidScroll(ScrollView* view)
{
	return;
}

void MapBlockScene::scrollViewDidZoom(ScrollView* view)
{
	log("zhou scrollViewDidZoom = %.2f", scrollView->getZoomScale());
	ajustMyPos();
	return;
}


void MapBlockScene::setMyPos()
{
	int px = mycurCol * MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2;
	int py = mycurRow * MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2;
	if (myposHero == NULL)
	{
		myposHero = spine::SkeletonAnimation::createWithJsonFile("herowalk/herowalk.json", "herowalk/herowalk.atlas", 0.5f);//0.5是设置图片的缩放比例

		myposHero->setPosition(Vec2(px, py + HEROOFFSET_Y));
		myposHero->setTimeScale(2);
		m_mapscrollcontainer->addChild(myposHero, 19999);
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

void MapBlockScene::ajustMyPos()
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


	scrollView->setContentOffsetInDuration(Vec2(-offsetx, -offsety), 0.5f);
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


	if (vec_cfg.size() >= 2)
	{
		randStartPos = atoi(vec_cfg[1].c_str());

		if (vec_cfg[0].compare("-1") == 0)
		{
			mapIsAllOpen = true;
			return;
		}

		if (vec_cfg[0].length() > 0)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(vec_cfg[0], vec_tmp, ",");
			int i = 0;
			for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
			{
				int val = atoi(vec_tmp[i].c_str());
				map_mapBlocks[it->first]->setIsCanSee(val == 1 ? true : false);
				i++;
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
			addFogBlock(it->first / blockColCount, it->first%blockColCount);
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
		addFogBlock(mycurRow, mycurCol);
		_fogrender->end();
		Director::getInstance()->getRenderer()->render();
		map_mapBlocks[mycurRow*blockColCount + mycurCol]->setIsCanSee(true);
	}
}

void MapBlockScene::addFogBlock(int row, int col)
{
	Sprite * sp = Sprite::createWithSpriteFrameName("mapui/fog.png");
	sp->setBlendFunc({ GL_ZERO, GL_ONE_MINUS_SRC_ALPHA });
	sp->setAnchorPoint(Vec2(0.5, 0.5));
	sp->setPosition(Vec2(col*MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, row*MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2));
	sp->setScale(fogscale);
	_fogrender->addChild(sp);
	sp->visit();
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

		if (ret == 0 || ret == 1 || ret == 2 || ret == 3 || ret == 4 || ret == 5 || ret == 6)//其它事件美术没有准备好，会崩溃，有图后打开这里
		{
			MapEventLayer* mlayer = MapEventLayer::create(ret);
			this->addChild(mlayer);
			AnimationEffect::openAniEffect((Layer*)mlayer);
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
			createRndMonsters();
			mapblock->setPosType(POS_NOTHING);
			mapblock->removePosIcon();
		}
		else if (mapblock->getPosType() == POS_BOX)
		{
			std::string str = StringUtils::format("%s-%d", m_mapid.c_str(), mycurRow*blockColCount + mycurCol);
			if (!DataSave::getInstance()->getMapBoxRewards(str))
			{
				status = MAP_S_EVENT;
				createBoxRewards(mapblock);
				DataSave::getInstance()->setMapBoxRewards(str, true);
			}
		}
		if (vec_enemys.size() > 0)
		{
			cacelLongTouch();
			//先判断任务
			bool isTask = false;
			int mycr = mycurRow*blockColCount + mycurCol;
			MapBlock* mapblock = map_mapBlocks[mycr];
			if (Quest::getMainQuestMap(m_mapid) && Quest::getMainQuestNpc(mapblock->getPosNpcID()))
			{
				isTask = true;
				this->addChild(TaskTalkLayer::create(mapblock->getPosNpcID(), vec_enemys));
			}
			else if (Quest::getBranchQuestMap(m_mapid) && Quest::getBranchQuestNpc(mapblock->getPosNpcID()))
			{
				isTask = true;
				this->addChild(TaskTalkLayer::create(mapblock->getPosNpcID(), vec_enemys, 1));
			}
			else if (m_mapid.compare("m0-0-0") != 0 && (mapblock->getPosType() == POS_BOSS || mapblock->getPosType() == POS_TBOSS || mapblock->getPosType() == POS_NPC))
			{
				isTask = true;
				NpcLayer* layer = NpcLayer::create(mapblock->getPosNpcID(), vec_enemys);
				this->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			if (!isTask)
			{
				if (m_mapid.compare("m0-0-0") == 0)
				{
					for (int i = 0; i < 2; i++)
					{
						if (NewGuideLayer::checkifNewerGuide(i))
						{
							DataSave::getInstance()->setIsNewerGuide(i, 0);
							if (g_NewGuideLayer != NULL)
							{
								g_NewGuideLayer->removeFromParentAndCleanup(true);
							}
						}
					}
					delayShowNewerGuide(0);
				}
				else
				{
					showFightingLayer(vec_enemys);
				}
			}
		}
	}
	if (status != MAP_S_NOTING)
	{
		cacelLongTouch();
	}
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
		RewardLayer* layer = RewardLayer::create(vec_rewards);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["nothingbox"]);
	}

	mbolck->removePosIcon();
}

void MapBlockScene::createRndMonsters()
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

	int rndcount = MapBlock::randMonstersMinCount + GlobalInstance::getInstance()->createRandomNum(MapBlock::randMonstersMaxCount - MapBlock::randMonstersMinCount + 1);
	for (int i = 0; i < rndcount; i++)
	{
		int r1 = GlobalInstance::getInstance()->createRandomNum(100);
		int rnd = 0;
		for (unsigned int m = 0; m < MapBlock::vec_randMonsters.size(); m++)
		{
			FOURProperty propty = MapBlock::vec_randMonsters[m];
			rnd += propty.floatPara3;
			if (r1 < rnd)
			{
				int minlv = propty.intPara1 / 1000;
				int maxlv = propty.intPara1 % 1000;
				int minqu = propty.intPara2 / 1000;
				int maxqu = propty.intPara2 % 1000;
				int rlv = minlv + GlobalInstance::getInstance()->createRandomNum(maxlv - minlv + 1);
				int rqu = minqu + GlobalInstance::getInstance()->createRandomNum(maxqu - minqu + 1);

				Npc* enemyhero = new Npc();
				std::string sid = MapBlock::vec_randMonsters[m].sid;
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
	int rndcount = MapBlock::randMonstersMinCount + GlobalInstance::getInstance()->createRandomNum(MapBlock::randMonstersMaxCount - MapBlock::randMonstersMinCount + 1);
	for (int i = 0; i < rndcount; i++)
	{
		int r1 = GlobalInstance::getInstance()->createRandomNum(100);
		int rnd = 0;
		for (unsigned int m = 0; m < MapBlock::vec_randMonsters.size(); m++)
		{
			FOURProperty propty = MapBlock::vec_randMonsters[m];
			rnd += propty.floatPara3;
			if (r1 < rnd)
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
	if (result == 1)//胜利
	{
		if (Quest::getMainQuestMap(m_mapid) && Quest::getMainQuestNpc(mapblock->getPosNpcID()))
		{
			Quest::finishTaskMain(QUEST_FIGHT);
			//showUnlockChapter();

		}
		else if (Quest::getBranchQuestMap(m_mapid) && Quest::getBranchQuestNpc(mapblock->getPosNpcID()))
		{
			Quest::finishTaskBranch(QUEST_FIGHT);
		}
		Quest::setAchieveTypeCount(ACHIEVE_FIGHT, 1, mapblock->getPosNpcID());

		isMoving = true;
		int bindex = (mycurRow)*blockColCount + mycurCol;
		if (map_mapBlocks[bindex]->getPosType() == POS_BOSS)
		{
			map_mapBlocks[bindex]->removePosIcon();
			map_mapBlocks[bindex]->setPosType(POS_NOTHING);
			openAllMap();
		}
		else if (map_mapBlocks[bindex]->getPosType() == POS_TBOSS)
		{
			map_mapBlocks[bindex]->removePosIcon();
			map_mapBlocks[bindex]->setPosType(POS_NOTHING);
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
	//地图全开
	openAllMap();
}

void MapBlockScene::openAllMap()
{
	/*if (_fogrender != NULL)
	{
		_fogrender->removeFromParentAndCleanup(true);
		_fogrender = NULL;
	}
	mapIsAllOpen = true;
	std::string str = StringUtils::format("-1;%d", randStartPos);
	DataSave::getInstance()->setMapVisibleArea(m_mapid, str);*/

	std::string mainid = m_mapid.substr(0, m_mapid.find_last_of("-"));
	int finishOrder = DataSave::getInstance()->getMapOrderCount(mainid);
	DataSave::getInstance()->setMapOrderCount(mainid, finishOrder + 1);
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
		winexp = getWinExp() / count;
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
}


void MapBlockScene::parseMapXml(std::string mapname)
{
	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
	std::string filename = StringUtils::format("mapdata/%s.xml", mapname.c_str());
	std::string contentstr = FileUtils::getInstance()->getStringFromFile(filename);
#if encrypt_jsonxml
	Encrypt((char*)contentstr.c_str(), false);
	contentstr = parseData(contentstr.c_str());
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
					if (postype == POS_START)
					{
						mb->setWalkable(true);
						vec_startpos.push_back(rc);
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
								mb->setEventIcon(mb->map_eventrnd.begin()->first);
							}
						}
						else if (ename.compare("npcid") == 0)
						{
							mb->setPosNpcID(e0->GetText());
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

					if (postype > POS_START)//起点有多个，只会显示一个，不在这里设置
					{
						bool showPosIcon = true;
						if (postype == POS_BOX)
						{
							std::string str = StringUtils::format("%s-%d", m_mapid.c_str(), rc);
							if (DataSave::getInstance()->getMapBoxRewards(str))
								showPosIcon = false;
						}
						if (postype == POS_NPC || postype == POS_BOSS || postype == POS_TBOSS)
						{
							int r = GlobalInstance::getInstance()->createRandomNum(100);
							if (r >= mb->getPosNpcRnd())
								showPosIcon = false;
						}
						if (showPosIcon)
						{
							mb->setPosType(postype);
							mb->setPosIcon();
						}
						else
						{
							mb->setPosType(-1);
						}
					}
				}
				element = element->NextSiblingElement();
			}
		}
		dataele = dataele->NextSiblingElement();
	}
	delete pDoc;
}
