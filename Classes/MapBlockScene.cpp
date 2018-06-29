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

MapBlockScene* g_MapBlockScene = NULL;

MapBlockScene::MapBlockScene()
{
	myposParticle = NULL;
	isMoving = false;
}


MapBlockScene::~MapBlockScene()
{
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

bool MapBlockScene::init(std::string mapname)
{
	m_csbnode = CSLoader::createNode(ResourcePath::makePath("mapBlockLayer.csb"));
	this->addChild(m_csbnode);

	Node* topnode = m_csbnode->getChildByName("mapblocktop");

	carrycountlbl = (cocos2d::ui::Text*)topnode->getChildByName("carrycount");
	foodcountlbl = (cocos2d::ui::Text*)topnode->getChildByName("r001count");
	solivercountlbl = (cocos2d::ui::Text*)topnode->getChildByName("solivercountlbl");

	parseMapXml(mapname);

	createBlackFog();

	int count = vec_startpos.size();

	int r = GlobalInstance::getInstance()->createRandomNum(count);
	mycurCol = vec_startpos[r] % blockColCount;
	mycurRow = vec_startpos[r] / blockColCount;

	setMyPos();

	ajustStartPos();

	updateLabel();

	Node* bottomnode = m_csbnode->getChildByName("mapblockbottom");

	cocos2d::ui::Widget* gocitybtn = (cocos2d::ui::Widget*)bottomnode->getChildByName("gocitybtn");
	gocitybtn->setTag(BTN_GOCITY);
	gocitybtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));

	std::string keyname[] = {"upbtn", "downbtn", "leftbtn", "rightbtn"};

	for (int i = 0; i < 4; i++)
	{
		cocos2d::ui::Widget* keybtn = (cocos2d::ui::Widget*)bottomnode->getChildByName(keyname[i]);
		keybtn->setTag(i+ KEY_UP);
		keybtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onArrowKey, this));
	}

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(60 + 120 * i, 1196);
		fightHeroNode->setScale(0.84f);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i]);
		addChild(fightHeroNode, 0, i);
	}

	return true;
}

void MapBlockScene::updateLabel()
{
	std::string str = StringUtils::format("%d/%d", MyRes::getMyPackageCount(), GlobalInstance::getInstance()->getTotalCaryy());
	carrycountlbl->setString(str);
	str = StringUtils::format("%d", MyRes::getMyResCount("r001", MYPACKAGE));
	foodcountlbl->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	foodcountlbl->setString(str);
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
		default:
			break;
		}
	}
}
void MapBlockScene::onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		bool isStartPos = false;

		if (isMoving)
			return;

		switch (tag)
		{
			case KEY_UP:
			{
				if (mycurRow >= blockRowCount - 1)
					return;
				int bindex = (mycurRow + 1)*blockColCount + mycurCol;
				if (!checkRoad(bindex))
					return;
				if (map_mapBlocks[bindex]->getPosType() == 0)
					isStartPos = true;

				mycurRow += 1;
				setMyPos();
				break;
			}
			case KEY_DOWN:
			{
				if (mycurRow <= 0)
					return;
				int bindex = (mycurRow - 1)*blockColCount + mycurCol;
				if (!checkRoad(bindex))
					return;
				if (map_mapBlocks[bindex]->getPosType() == 0)
					isStartPos = true;
				mycurRow -= 1;
				setMyPos();
				break;
			}
			case KEY_LEFT:
			{
				if (mycurCol <= 0)
					return;

				int bindex = (mycurRow)*blockColCount + mycurCol - 1;
				if (!checkRoad(bindex))
					return;
				if (map_mapBlocks[bindex]->getPosType() == 0)
					isStartPos = true;
				mycurCol -= 1;
				setMyPos();
				break;
			}
			case KEY_RIGHT:
			{
				if (mycurCol >= blockColCount - 1)
					return;

				int bindex = (mycurRow)*blockColCount + mycurCol + 1;
				if (!checkRoad(bindex))
					return;
				if (map_mapBlocks[bindex]->getPosType() == 0)
					isStartPos = true;
				mycurCol += 1;
				setMyPos();
				break;
			}

			default:
				break;
		}
		if (isStartPos)
		{
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
		}
	}
}

void MapBlockScene::stopMoving()
{

	doMyStatus();
	isMoving = false;
}

bool MapBlockScene::checkRoad(int blockindex)
{
	if (!map_mapBlocks[blockindex]->getWalkable())
	{
		MovingLabel::show(ResourceLang::map_lang["noroad"]);
		return false;
	}
	else
	{
		return true;
	}
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

	updateFogVisible();

}

void MapBlockScene::ajustStartPos()
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


	scrollView->setContentOffset(Vec2(-offsetx, -offsety));
}

FightHeroNode* MapBlockScene::getFightHeroNode(int index)
{
	return (FightHeroNode*)this->getChildByTag(index);
}

void MapBlockScene::createBlackFog()
{
	std::map<int, MapBlock*>::iterator it;
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
	int mycr = mycurRow*blockColCount + mycurCol;
	MapBlock* mapblock = map_mapBlocks[mycr];
	std::map<int, int>::iterator rnd_it;
	int r = GlobalInstance::getInstance()->createRandomNum(100);
	
	for (rnd_it = mapblock->map_eventrnd.begin(); rnd_it != mapblock->map_eventrnd.end(); rnd_it++)
	{
		if (r < mapblock->map_eventrnd[rnd_it->first])
		{
			ret = rnd_it->first;
			break;
		}
	}
	if (ret >= 0)
	{
		MapEventLayer* mlayer = MapEventLayer::create(ret);
		this->addChild(mlayer);
	}
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
					mdata.intPara1 = msele->IntAttribute("minlv") * 1000+ msele->IntAttribute("maxlv");
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
							mdata.intPara1 = e0->IntAttribute("qu");
							mdata.intPara2 = atoi(e0->GetText());
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
									mdata.intPara1 = e01->IntAttribute("qu");
									mdata.intPara2 = atoi(e01->GetText());
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
