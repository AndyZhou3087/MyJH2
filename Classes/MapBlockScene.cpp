#include "MapBlockScene.h"
#include "Resource.h"
#include "GlobalInstance.h"
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
#include "TaskBranchTalkLayer.h"
#include "MyPackageLayer.h"

MapBlockScene* g_MapBlockScene = NULL;

MapBlockScene::MapBlockScene()
{
	myposParticle = NULL;
	isMoving = false;

	m_isLongPress = false;
	m_longTouchNode = NULL;
	walkcount = 0;
	monsterComeRnd = 10;
}


MapBlockScene::~MapBlockScene()
{
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getState() == HS_DEAD)
		{
			GlobalInstance::myCardHeros[i] = NULL;
		}
	}

	g_MapBlockScene = NULL;
}

Scene* MapBlockScene::createScene(std::string mapname)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	g_MapBlockScene = MapBlockScene::create(mapname);

	// add layer as a child to scene
	scene->addChild(g_MapBlockScene);

	// return the scene
	return scene;
}

MapBlockScene* MapBlockScene::create(std::string mapname)
{
	MapBlockScene *pRet = new(std::nothrow)MapBlockScene();
	if (pRet && pRet->init(mapname))
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
	std::string str;
	std::map<int, MapBlock*>::iterator it;
	for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
	{
		std::string onestr = StringUtils::format("%d,", map_mapBlocks[it->first]->getIsCanSee() ? 1 : 0);
		str.append(onestr);
	}
	DataSave::getInstance()->setMapVisibleArea(m_mapid, str.substr(0, str.length()-1));

	Layer::onExit();
}

bool MapBlockScene::init(std::string mapname)
{
	m_mapid = mapname;

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("mapBlockLayer.csb"));
	this->addChild(m_csbnode);

	Node* topnode = m_csbnode->getChildByName("mapblocktop");

	carrycountlbl = (cocos2d::ui::Text*)topnode->getChildByName("carrycount");
	foodcountlbl = (cocos2d::ui::Text*)topnode->getChildByName("r001count");
	solivercountlbl = (cocos2d::ui::Text*)topnode->getChildByName("solivercountlbl");

	lackfoodlbl = (cocos2d::ui::Text*)topnode->getChildByName("lackfood");
	lackfoodlbl->setVisible(false);
	lackfoodlbl->runAction(RepeatForever::create(Blink::create(1.0f, 1)));

	sitelbl = (cocos2d::ui::Text*)topnode->getChildByName("site");

	parseMapXml(mapname);

	createBlackFog();

	int count = vec_startpos.size();

	randStartPos = GlobalInstance::getInstance()->createRandomNum(count);
	mycurCol = vec_startpos[randStartPos] % blockColCount;
	mycurRow = vec_startpos[randStartPos] / blockColCount;

	setMyPos();

	updateLabel();

	Node* bottomnode = m_csbnode->getChildByName("mapblockbottom");

	cocos2d::ui::Widget* gocitybtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("gocitybtn");
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
	}

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(60 + 120 * i, 1196);
		fightHeroNode->setScale(0.84f);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i], F_HERO, FS_READY);
		addChild(fightHeroNode, 0, i);
	}

	return true;
}

void MapBlockScene::updateLabel()
{
	int foodcount = MyRes::getMyResCount("r001", MYPACKAGE);
	std::string str = StringUtils::format("%d/%d", MyRes::getMyPackageCount(), GlobalInstance::getInstance()->getTotalCaryy());
	carrycountlbl->setString(str);
	str = StringUtils::format("%d", foodcount);
	foodcountlbl->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	solivercountlbl->setString(str);

	lackfoodlbl->setVisible(foodcount <= 5);
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
			Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MainScene::createScene()));
			break;
		case BTN_MAP:
			Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MainMapScene::createScene()));
			break;
		case BTN_PACKAGE:
		{
			MyPackageLayer* layer = MyPackageLayer::create();
			this->addChild(layer);
		}
		break;
		default:
			break;
		}
	}
}
void MapBlockScene::onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(MapBlockScene::longTouchUpdate)))
			schedule(schedule_selector(MapBlockScene::longTouchUpdate), 0.7f);
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

	if (MyRes::getMyResCount("r001", MYPACKAGE) <= 0)
	{

		return;
	}
	if (!checkRoad(keyArrow))
		return;

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
	setMyPos();
	walkcount++;
	monsterComeRnd += (5 + walkcount);

	MyRes::Use("r001", 1, MYPACKAGE);
	updateLabel();

	if (mycurCol == vec_startpos[randStartPos] % blockColCount && mycurRow == vec_startpos[randStartPos] / blockColCount)
	{
		cacelLongTouch();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
	}
}

void MapBlockScene::stopMoving()
{
	doMyStatus();
	isMoving = false;
	if (m_isLongPress) 
	{
		go((MAP_KEYTYPE)m_longTouchNode->getTag());
	}
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
		}
		break;
		case KEY_RIGHT:
		{
			bindex = (mycurRow)*blockColCount + mycurCol + 1;
			if (mycurCol >= blockColCount - 1)
				return false;
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
	scrollView->setViewSize(Size(720, 800));
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
	return;

}


void MapBlockScene::setMyPos()
{
	int px = mycurCol *MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2;
	int py = mycurRow * MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2;
	if (myposParticle == NULL)
	{
		myposParticle = ParticleSystemQuad::create("particle/hr.plist");
		m_mapscrollcontainer->addChild(myposParticle, 10000);
		myposParticle->setPosition(Vec2(px, py));
	}
	else
	{
		isMoving = true;
		myposParticle->runAction(Sequence::create(MoveTo::create(0.5f, Vec2(px, py)), CallFunc::create(CC_CALLBACK_0(MapBlockScene::stopMoving, this)), NULL));
	}

	ajustMyPos();

	updateFogVisible();
	std::string str = StringUtils::format("%d,%d", mycurRow + 1, mycurCol + 1);
	sitelbl->setString(str);
}

void MapBlockScene::ajustMyPos()
{
	float offsetx = myposParticle->getPosition().x  - scrollView->getViewSize().width / 2;
	float offsety = myposParticle->getPosition().y - scrollView->getViewSize().height / 2;

	if (offsetx < 0)
		offsetx = 0;
	if (offsety < 0)
		offsety = 0;

	if (offsetx > m_mapscrollcontainer->getContentSize().width - scrollView->getViewSize().width)
		offsetx = m_mapscrollcontainer->getContentSize().width - scrollView->getViewSize().width;
	if (offsety > m_mapscrollcontainer->getContentSize().height - scrollView->getViewSize().height)
		offsety = m_mapscrollcontainer->getContentSize().height - scrollView->getViewSize().height;


	scrollView->setContentOffsetInDuration(Vec2(-offsetx, -offsety), 0.5f);
}

FightHeroNode* MapBlockScene::getFightHeroNode(int index)
{
	return (FightHeroNode*)this->getChildByTag(index);
}

void MapBlockScene::createBlackFog()
{
	std::map<int, MapBlock*>::iterator it;

	std::string str = DataSave::getInstance()->getMapVisibleArea(m_mapid);

	if (str.length() > 0)
	{
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(str, vec_tmp, ",");
		int i = 0;
		for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
		{
			int val = atoi(vec_tmp[i].c_str());
			map_mapBlocks[it->first]->setIsCanSee(val==1?true:false);
			i++;
		}
	}
	for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
	{
		if (!checkBlockVisible(it->first))
		{
			Sprite* black = Sprite::createWithSpriteFrameName("mapui/blockblack.png");
			black->setAnchorPoint(Vec2(0.5, 0.5));
			black->setPosition(Vec2(it->first%blockColCount*MAPBLOCKWIDTH + MAPBLOCKWIDTH / 2, it->first / blockColCount*MAPBLOCKHEIGHT + MAPBLOCKHEIGHT / 2));
			m_mapscrollcontainer->addChild(black, 3000);
			map_mapFogBlacks[it->first] = black;
		}
	}
}

bool MapBlockScene::checkBlockVisible(int mapiter)
{
	int mycr = mycurRow*blockColCount + mycurCol;
	MapBlock* mapblock = map_mapBlocks[mapiter];
	bool isvisible = false;
	if (mapiter == mycr || mapiter == (mycr + 1) || mapiter == (mycr - 1) || mapiter == (mycr + blockColCount) || mapiter == (mycr - blockColCount))
	{
		isvisible = true;
	}
	else
	{
		if (mapblock->getIsCanSee())
			isvisible = true;
	}
	return isvisible;

}

void MapBlockScene::updateFogVisible()
{
	std::map<int, MapBlock*>::iterator it;
	for (it = map_mapBlocks.begin(); it != map_mapBlocks.end(); it++)
	{
		if (checkBlockVisible(it->first))
		{
			map_mapBlocks[it->first]->setIsCanSee(true);
			std::map<int, Sprite*>::iterator fit = map_mapFogBlacks.find(it->first);
			if (fit != map_mapFogBlacks.end())
			{
				if (map_mapFogBlacks[it->first] != NULL)
					map_mapFogBlacks[it->first]->runAction(Sequence::create(FadeOut::create(1.2f), CallFunc::create(CC_CALLBACK_0(MapBlockScene::removeBlackFog, this, it->first)), NULL));
			}
		}
	}
}

void MapBlockScene::removeBlackFog(int mapiter)
{
	map_mapFogBlacks[mapiter]->removeFromParentAndCleanup(true);
	map_mapFogBlacks[mapiter] = NULL;
}

void MapBlockScene::initBlockData()
{
	MapBlock::vec_randMonsters.clear();
	MapBlock::vec_randMonstersRes.clear();
	MapBlock::randMonstersMinCount = 0;
	MapBlock::randMonstersMaxCount = 0;
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
	if (ret >= 0)
	{
		status = MAP_S_EVENT;
		//触发过一次不再触发
		for (rnd_it = mapblock->map_eventrnd.begin(); rnd_it != mapblock->map_eventrnd.end(); rnd_it++)
		{
			mapblock->map_eventrnd[rnd_it->first] = 0;
		}

		MapEventLayer* mlayer = MapEventLayer::create(ret);
		this->addChild(mlayer);
	}
	else
	{
		vec_enemys.clear();
		if (mapblock->getPosType() == POS_NPC || mapblock->getPosType() == POS_BOSS )
		{
			creatNpcOrBoss(mapblock);
		}
		else if (mapblock->getPosType() == POS_NOTHING)
		{
			createRndMonsters();
		}
		if (vec_enemys.size() > 0)
		{
			//先判断任务
			bool isTask = false;
			for (unsigned int i = 0; i < vec_enemys.size(); i++)
			{
				if (vec_enemys[i]!=NULL)
				{
					if (Quest::getMainQuestNpc(vec_enemys[i]->getId()))
					{
						isTask = true;
						this->addChild(TaskTalkLayer::create(vec_enemys[i]->getId(), vec_enemys));
						break;
					}
					else if (Quest::getBranchQuestNpc(vec_enemys[i]->getId()))
					{
						isTask = true;
						this->addChild(TaskBranchTalkLayer::create(vec_enemys[i]->getId(), vec_enemys));
						break;
					}
				}
			}
			if (!isTask)
			{
				this->addChild(FightingLayer::create(vec_enemys));
			}
		}
	}
	if (status != MAP_S_NOTING)
	{
		cacelLongTouch();
	}
}

void MapBlockScene::createRndMonsters()
{
	int r = GlobalInstance::getInstance()->createRandomNum(100);
	if (r < monsterComeRnd)
	{
		monsterComeRnd = 10;
		walkcount = 0;
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
					enemyhero->setLevel(rlv - 1);
					enemyhero->setHp(enemyhero->getMaxHp());
					vec_enemys.push_back(enemyhero);
					break;
				}
			}

		}
		vec_winrewards.clear();
		for (unsigned int i = 0; i < MapBlock::vec_randMonstersRes.size(); i++)
		{
			FOURProperty propty = MapBlock::vec_randMonstersRes[i];
			int rnd = propty.floatPara3 * 100;
			int r2 = GlobalInstance::getInstance()->createRandomNum(10000);
			if (r2 < rnd)
				vec_winrewards.push_back(propty);
		}
	}

}

void MapBlockScene::creatNpcOrBoss(MapBlock* mbolck)
{
	int r1 = GlobalInstance::getInstance()->createRandomNum(100);
	if (r1 < mbolck->getPosNpcRnd())
	{
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
	}
}

void MapBlockScene::updateHeroUI(int which)
{
	FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(which);
	fnode->updateHp();
}

void MapBlockScene::showFightResult(int result)
{
	if (result == 0)
	{
		std::vector<FOURProperty> vec;
		FightingResultLayer* FRlayer = FightingResultLayer::create(vec, 0);
		this->addChild(FRlayer);
	}
	else
	{
		for (unsigned int i = 0; i < vec_enemys.size(); i++)
		{
			if (vec_enemys[i] != NULL)
			{
				if (Quest::getMainQuestNpc(vec_enemys[i]->getId()))
				{
					Quest::finishFightMain(QUEST_FIGHT);
				}
				else if (Quest::getBranchQuestNpc(vec_enemys[i]->getId()))
				{
					Quest::finishBranchQuest();
				}
			}
		}
		int count = 0;
		for (int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL &&  GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
				count++;
		}
		FightingResultLayer* FRlayer = FightingResultLayer::create(vec_winrewards, getWinExp()/count);
		this->addChild(FRlayer);
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


void MapBlockScene::parseMapXml(std::string mapname)
{
	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
	std::string filename = StringUtils::format("mapdata/%s.xml", mapname.c_str());
	std::string contentstr = FileUtils::getInstance()->getStringFromFile(filename);
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

					MapBlock* mb = MapBlock::create(blockRowCount - 1 - r, c, boardname, buildname);
					
					int rc = (blockRowCount - 1 - r)*blockColCount + c;
					int zorder = (blockRowCount - 1 - r)*blockColCount + blockColCount - c;
					m_mapscrollcontainer->addChild(mb, zorder);
					mb->Col = c;
					mb->Row = r;
					mb->setPosType(postype);
					mb->setWalkable(walkable);
					map_mapBlocks[rc] = mb;
					if (postype == 0)
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
				}
				element = element->NextSiblingElement();
			}
		}
		dataele = dataele->NextSiblingElement();
	}
	delete pDoc;
}
