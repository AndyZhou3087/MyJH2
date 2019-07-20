#include "MapEventLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "MyRes.h"
#include "EventBusinessLayer.h"
#include "AnimationEffect.h"
#include "MapBlockScene.h"
#include "DynamicValue.h"
#if MAP_BET
#include "GambleBoxLayer.h"
#endif
#include "MovingLabel.h"
#include "BuyCoinLayer.h"
#include "MazeTransitionScene.h"
#include "Quest.h"

#define BETPRO 460

int maxqupr[5] = { 585,885,985,995,1000 };

USING_NS_CC;

std::vector<EventData> MapEventLayer::vec_eventdata;
std::map<std::string, EventData> MapEventLayer::map_eventdata;

MapEventLayer::MapEventLayer()
{
	lastBetIndex = -1;
	winbs = -1;
	isWin = -1;
	hdcount = 0;
	betCostCoin.setValue(BETCOSTCOIN);
}

MapEventLayer::~MapEventLayer()
{
	g_MapBlockScene->isRoutingBreakOff = false;
}


MapEventLayer* MapEventLayer::create(int eventindex)
{
	MapEventLayer *pRet = new(std::nothrow)MapEventLayer();
	if (pRet && pRet->init(eventindex))
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

// on "init" you need to initialize your instance
bool MapEventLayer::init(int eventindex)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_eventindex = eventindex;

	LayerColor* color = LayerColor::create(Color4B(0, 0, 0, 255));
	this->addChild(color,0,"colorLayer");

	//读取event数据
	loadEventData("event");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mapEventLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	std::string str = StringUtils::format("images/event%d.jpg", eventindex);
	eventimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("eventimg");
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	coin = (cocos2d::ui::Widget*)csbnode->getChildByName("coin");
	coincount = (cocos2d::ui::Text*)coin->getChildByName("count");
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	coincount->setString(str);
	if (m_eventindex == POS_BET)
	{
		coin->setVisible(true);
	}

	eventnode_1 = csbnode->getChildByName("eventnode_1");
	eventnode_2 = csbnode->getChildByName("eventnode_2");
	eventnode_3 = csbnode->getChildByName("eventnode_3");
	eventnode_4 = csbnode->getChildByName("eventnode_4");
	eventnode_1->setVisible(true);
	eventnode_2->setVisible(false);
	eventnode_3->setVisible(false);
	eventnode_4->setVisible(false);

	m_closebtn = (cocos2d::ui::Button*)eventnode_4->getChildByName("closebtn");
	m_closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	m_closebtn->setTag(0);
	cocos2d::ui::ImageView* m_text = (cocos2d::ui::ImageView*)m_closebtn->getChildByName("text");
	m_text->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	m_text->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("backbtn_text", langtype))->getContentSize());

	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_1->getChildByName("text");
	str = StringUtils::format("event%d_0", eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_1->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	if (m_eventindex != POS_WOMAN && m_eventindex != POS_THIEF)
	{
		closebtn->setTag(0);
	}
	else
	{
		closebtn->setTag(-1);
	}
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	str = StringUtils::format("mapeventtext_%d_0", eventindex);
	text->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	text->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath(str, langtype))->getContentSize());

	cocos2d::ui::Button* actionbtn = (cocos2d::ui::Button*)eventnode_1->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	actionbtn->setTag(1);
	cocos2d::ui::ImageView* text2 = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	str = StringUtils::format("mapeventtext_%d_1", eventindex);
	text2->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	text2->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath(str, langtype))->getContentSize());

	if (m_eventindex == POS_ELDER && GlobalInstance::getInstance()->getMySoliverCount().getValue() < 5000)
	{
		actionbtn->setVisible(false);
		closebtn->setPositionX(0);
		text->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		text->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype))->getContentSize());
		textdesc->setString(ResourceLang::map_lang["nosilvertohelp"]);
	}

	loadPrData();

	this->schedule(schedule_selector(MapEventLayer::updateCoin), 1.0f);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MapEventLayer::loadEventData(std::string filename)
{
	map_eventdata.clear();
	/*int avelv = GlobalInstance::getInstance()->getFightHerosLevel();
	int i = avelv / 10 + 1;*/
	int i = GlobalInstance::getInstance()->getUnlockChapter();
	std::string str;
	
	if (filename.compare("shopevent") == 0)
		str = StringUtils::format("json/%s.json", filename.c_str());
	else
		str = StringUtils::format("json/%s%d.json", filename.c_str(), i);

	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath(str));
	rapidjson::Value& allData = doc["b"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			EventData data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["pr"];
			data.pr = atoi(v.GetString());

			v = jsonvalue["max"];
			data.max = atoi(v.GetString());

			v = jsonvalue["min"];
			data.min = atoi(v.GetString());

			v = jsonvalue["mqu"];
			data.maxqu = atoi(v.GetString());

			map_eventdata[data.id] = data;
		}
	}
}

void MapEventLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* node = (Node*)pSender;
	int tag = node->getTag();
	if (tag!=100 && tag!=101)
	{
		CommonFuncs::BtnAction(pSender, type);
	}
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		//摔伤下死亡回城
		int hcount = 0;
		for (int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL)
			{
				hcount++;
			}
		}
		if (hdcount == hcount)
		{
			AnimationEffect::closeAniEffect((Layer*)this);
			MapBlockScene* block = (MapBlockScene*)this->getParent();
			if (block != NULL)
			{
				block->showNewerGuideGoBack();
			}
			return;
		}

		switch (tag)
		{
		case -1:
			if (m_eventindex == POS_WOMAN)
			{
				eventnode_1->setVisible(false);
				eventnode_2->setVisible(false);
				eventnode_3->setVisible(true);
				getSliverByEventW();
			}
			else if (m_eventindex == POS_THIEF)
			{
				eventnode_1->setVisible(false);
				eventnode_2->setVisible(false);
				eventnode_3->setVisible(true);
				eventElderExtort(m_eventindex);
			}
			break;
		case 0:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1:
			if (m_eventindex == MAP_JUMP || m_eventindex == POS_CAVE)
			{
				int r = GlobalInstance::getInstance()->createRandomNum(100);
				if (r < 10)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(true);
					eventnode_3->setVisible(false);
					boxEventNode();
				}
				else if (r < 45)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventNothing();
				}
				else
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventHurt();
				}
			}
			else if (m_eventindex == POS_BUSINESS)
			{
				Layer* layer = EventBusinessLayer::create();
				g_MapBlockScene->addChild(layer, 100, "buisinesslayer");
				AnimationEffect::openAniEffect(layer);
				AnimationEffect::closeAniEffect(this);
			}
			else if (m_eventindex == POS_ELDER)
			{
				int r = GlobalInstance::getInstance()->createRandomNum(10) + 1;
				if (r <= 4)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(true);
					eventnode_3->setVisible(false);
					boxEventNode();
				}
				else
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventElderExtort(m_eventindex);
				}

				Quest::setAchieveTypeCount(SAFE_OLDER, 1);
			}
			else if (m_eventindex == POS_WOMAN)
			{
				int r = GlobalInstance::getInstance()->createRandomNum(100) + 1;
				if (r <= 25)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(true);
					eventnode_3->setVisible(false);
					boxEventNode();
				}
				else if (r <= 50)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventNothing();
				}
				else if (r <= 75)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventElderExtort(m_eventindex);
				}
				else
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventFight();
				}

				Quest::setAchieveTypeCount(SAFE_WOMAN, 1);
			}
			else if (m_eventindex == POS_THIEF)
			{
				AnimationEffect::closeAniEffect((Layer*)this);
				if (g_MapBlockScene != NULL)
				{
					g_MapBlockScene->eventFight();
				}
			}
#if MAP_BET
			else if (m_eventindex == POS_BET)
			{
				eventnode_1->setVisible(false);
				eventnode_2->setVisible(false);
				eventnode_3->setVisible(false);
				eventnode_4->setVisible(true);
				doGambling();
			}
#endif
			break;
		case 2:
		{
			for (unsigned int i = 0; i < vec_eventrewards.size(); i++)
			{
				std::vector<std::string> vec_rewards = vec_eventrewards[i];
				std::string resid = vec_rewards[0];
				int count = atoi(vec_rewards[1].c_str());
				int qu = atoi(vec_rewards[2].c_str());
				int stonesCount = GlobalInstance::getInstance()->generateStoneCount(qu);

				if (resid.compare("r006") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->addMySoliverCount(dvint);
				}
				else if (resid.compare("r012") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->addMyCoinCount(dvint);
				}
				else
					MyRes::Add(resid, count, MYPACKAGE, qu, stonesCount);
			}

			std::string namestr;

			std::string templetstr = "";

			if (m_eventindex == POS_ELDER)
				templetstr = "newtemplet3";
			else if (m_eventindex == MAP_JUMP)
				templetstr = "newtemplet4";
			else if (m_eventindex == POS_WOMAN)
				templetstr = "newtemplet5";
			else if (m_eventindex == POS_CAVE)
				templetstr = "newtemplet6";
			for (unsigned int i = 0; i < vec_eventrewards.size(); i++)
			{
				if (namestr.length() > 0)
				{
					namestr.append(ResourceLang::map_lang["dunhao"]);
				}
				std::vector<std::string> vec_rewards = vec_eventrewards[i];
				std::string resid = vec_rewards[0];
				int count = atoi(vec_rewards[1].c_str());
				int qu = atoi(vec_rewards[2].c_str());
				if (qu >= 4)
				{
					std::string st = StringUtils::format("potential_%d", qu);
					namestr = StringUtils::format(ResourceLang::map_lang["libraryinfoequipname"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str(), ResourceLang::map_lang[st].c_str());
				}
			}
			if (namestr.length() > 0)
			{
				std::string contentstr = StringUtils::format(ResourceLang::map_lang[templetstr].c_str(), GlobalInstance::getInstance()->getMyNickName().c_str(), namestr.c_str());
				MainScene::addNews(contentstr, 2);
			}
			//this->removeFromParentAndCleanup(true);
			AnimationEffect::closeAniEffect((Layer*)this);
		}
			break;
		case 3:
		{
#if MAP_BET
			if (lastBetIndex == -1)
			{
				MovingLabel::show(ResourceLang::map_lang["bettipstext"]);
				return;
			}
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= betCostCoin.getValue())
			{
				DynamicValueInt dvl;
				dvl.setValue(-betCostCoin.getValue());
				//GlobalInstance::getInstance()->costMyCoinCount(dvl);
				GlobalInstance::getInstance()->addMyCoinCount(dvl);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);

				Layer* layer = BuyCoinLayer::create(betCostCoin.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
				return;
			}
			for (int i = 0; i < 2; i++)
			{
				betArr[i]->setTouchEnabled(false);
			}
			betActionbtn->setTouchEnabled(false);
			m_closebtn->setTouchEnabled(false);
			animnode->getChildByName("node")->getChildByName("cover")->setVisible(true);
			anim_action = CSLoader::createTimeline("rollDiceAnim.csb");
			animnode->runAction(anim_action);
			anim_action->gotoFrameAndPlay(0, true);

			int r = GlobalInstance::getInstance()->createRandomNum(3);
			this->scheduleOnce(schedule_selector(MapEventLayer::showResult), r + 1.0f);
#endif
		}

			break;
		case 100:
		{
			betselectArr[0]->setVisible(true);
			betselectArr[1]->setVisible(false);
			lastBetIndex = 0;
		}
			break;
		case 101:
		{
			betselectArr[1]->setVisible(true);
			betselectArr[0]->setVisible(false);
			lastBetIndex = 1;
		}
			break;
		case 1000:
		{
			AnimationEffect::closeAniEffect((Layer*)this);
			if (g_MapBlockScene != NULL)
			{
				g_MapBlockScene->eventFight();
			}
		}
		break;
		default:
			break;
		}
	}
}

void MapEventLayer::getSliverByEventW()
{
	cocos2d::ui::Widget* costsliver = (cocos2d::ui::Widget*)eventnode_3->getChildByName("costsliver");
	costsliver->setVisible(true);
	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)costsliver->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["event2_5"]);
	desc->setTextColor(Color4B(0, 255, 0, 255));
	cocos2d::ui::Text* count = (cocos2d::ui::Text*)costsliver->getChildByName("count");

	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_3->getChildByName("text");
	textdesc->setString(ResourceLang::map_lang["event2_6"]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_3->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(0);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	text->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype))->getContentSize());

	std::string str = "images/event2-0.jpg";
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	int rsliver = GlobalInstance::getInstance()->createRandomNum(3001) + 2000;
	str = StringUtils::format("%d", rsliver);
	count->setString(str);
	DynamicValueInt val;
	val.setValue(rsliver);
	GlobalInstance::getInstance()->addMySoliverCount(val);
}

void MapEventLayer::eventFight()
{
	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_3->getChildByName("text");
	std::string str = StringUtils::format("event%d_4", m_eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_3->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(1000);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("images/event%d-4.jpg", m_eventindex);
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);
}

#if MAP_BET
void MapEventLayer::showResult(float dt)
{
	anim_action = CSLoader::createTimeline("rollDiceAnim.csb");
	animnode->runAction(anim_action);
	anim_action->gotoFrameAndPlay(0, 0, false);
	animnode->stopAllActions();

	this->scheduleOnce(schedule_selector(MapEventLayer::openDice), 1.0f);
}

void MapEventLayer::openDice(float dt)
{
	int r = GlobalInstance::getInstance()->createRandomNum(1000) + 1;
	if (r < BETPRO)
	{
		if (lastBetIndex == 0)
		{
			winbs = GlobalInstance::getInstance()->createRandomNum(3) + 1;
		}
		else
		{
			winbs = GlobalInstance::getInstance()->createRandomNum(3) + 4;
		}
		isWin = 1;
	}
	else
	{
		if (lastBetIndex == 0)
		{
			winbs = GlobalInstance::getInstance()->createRandomNum(3) + 4;
		}
		else
		{
			winbs = GlobalInstance::getInstance()->createRandomNum(3) + 1;
		}
		isWin = 0;
	}

	animnode->getChildByName("node")->getChildByName("cover")->setVisible(false);
	std::string dicestr = StringUtils::format("ui/dice%d.png", winbs);

	Sprite* dicesprite = Sprite::createWithSpriteFrameName(dicestr);
	dicesprite->setPosition(animnode->getPosition());
	eventnode_4->addChild(dicesprite, 1, "dice");

	this->scheduleOnce(schedule_selector(MapEventLayer::playGamblebox), 1.0f);
}

void MapEventLayer::playGamblebox(float dt)
{
	GambleBoxLayer* gamble = GambleBoxLayer::create(isWin, winbs, BETCOSTCOIN);
	this->addChild(gamble);
	AnimationEffect::openAniEffect((Layer*)gamble);
}


void MapEventLayer::continueGamble()
{
	eventnode_4->removeChildByName("dice");
	animnode->getChildByName("node")->getChildByName("cover")->setVisible(true);
	for (int i = 0; i < 2; i++)
	{
		betArr[i]->setTouchEnabled(true);
	}
	betActionbtn->setTouchEnabled(true);
	m_closebtn->setTouchEnabled(true);
}


void MapEventLayer::doGambling()
{
	std::string str;
	for (int i = 0; i < 2; i++)
	{
		str = StringUtils::format("bet_%d", i);
		cocos2d::ui::ImageView* bet = (cocos2d::ui::ImageView*)eventnode_4->getChildByName(str);
		bet->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
		bet->setTag(100 + i);
		betArr[i] = bet;
		str = StringUtils::format("betselect%d", i);
		cocos2d::ui::Widget* betselect = (cocos2d::ui::Widget*)eventnode_4->getChildByName(str);
		betselect->setVisible(false);
		betselectArr[i] = betselect;
		/*if (i == 0)
		{
			betselect->setVisible(true);
			lastBetIndex = i;
		}*/
	}
	cocos2d::ui::Text* selecttext = (cocos2d::ui::Text*)eventnode_4->getChildByName("selecttext");
	selecttext->setString(ResourceLang::map_lang["event5_1"]);
	cocos2d::ui::Text* betdesc = (cocos2d::ui::Text*)eventnode_4->getChildByName("betdesc");
	betdesc->setString(ResourceLang::map_lang["event5_2"]);
	betActionbtn = (cocos2d::ui::Button*)eventnode_4->getChildByName("actionbtn");
	betActionbtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	betActionbtn->setTag(3);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)betActionbtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	animnode = CSLoader::createNode("rollDiceAnim.csb");
	animnode->setPosition(Vec2(0, 150));
	eventnode_4->addChild(animnode);
}
#endif
void MapEventLayer::updateCoin(float dt)
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	coincount->setString(str);
}

void MapEventLayer::eventElderExtort(int type)
{
	cocos2d::ui::Widget* costsliver = (cocos2d::ui::Widget*)eventnode_3->getChildByName("costsliver");
	costsliver->setVisible(true);
	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)costsliver->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["elderextort"]);
	cocos2d::ui::Text* count = (cocos2d::ui::Text*)costsliver->getChildByName("count");
	//std::string str = StringUtils::format("%d", COSTSLIVER);
	//count->setString(str);

	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_3->getChildByName("text");
	std::string str;
	if (type == POS_ELDER)
	{
		str = StringUtils::format("event%d_2", m_eventindex);
	}
	else if (type == POS_WOMAN)
	{
		str = StringUtils::format("event%d_3", m_eventindex);
	}
	else if (type == POS_THIEF)
	{
		str = StringUtils::format("event%d_1", m_eventindex);
	}
	textdesc->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_3->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(0);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	DynamicValueInt costslivercount;
	costslivercount.setValue(5000);
	if (type != POS_THIEF)
	{
		str = StringUtils::format("images/event%d-2.jpg", m_eventindex);
		eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);
	}
	else
	{

	}
	str = StringUtils::format("%d", costslivercount.getValue());
	count->setString(str);

	if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= costslivercount.getValue())
	{
		DynamicValueInt val;
		val.setValue(costslivercount.getValue());
		GlobalInstance::getInstance()->costMySoliverCount(val);
	}
	else
	{
		GlobalInstance::getInstance()->costMySoliverCount(GlobalInstance::getInstance()->getMySoliverCount());
	}
}

void MapEventLayer::loadPrData()
{
	vec_eventdata.clear();
	std::map<std::string, EventData>::iterator it;
	for (it = map_eventdata.begin(); it != map_eventdata.end(); it++)
	{
		EventData data = it->second;
		vec_eventdata.push_back(data);
	}
	sort(vec_eventdata.begin(), vec_eventdata.end(), larger_callback);
}

bool MapEventLayer::larger_callback(EventData a, EventData b)
{
	int prA = a.pr;
	int prB = b.pr;
	if (prA < prB)
		return true;
	else
		return false;
}

int MapEventLayer::getAllPr()
{
	int allpr = 0;
	for (unsigned int i = 0; i < vec_eventdata.size(); i++)
	{
		int pr = vec_eventdata[i].pr;
		allpr += pr;
	}
	return allpr;
}

std::string MapEventLayer::getDataIdByPr()
{
	int r = GlobalInstance::getInstance()->createRandomNum(getAllPr()) + 1;
	std::string id = "";
	int pr = 0;
	for (unsigned int i = 0; i < vec_eventdata.size(); i++)
	{
		pr += vec_eventdata[i].pr;
		if (pr >= r)
		{
			id = vec_eventdata[i].id;
			vec_eventdata.erase(vec_eventdata.begin() + i);
			return id;
		}
	}
	return id;
}

int MapEventLayer::getEquipQuRand(std::string resid)
{
	EventData data;
	for (unsigned int i = 0; i < vec_eventdata.size(); i++)
	{
		data = vec_eventdata[i];
		if (data.id.compare(resid) == 0)
		{
			break;
		}
	}
	int rnd = getEventMaxQu(data.maxqu);
	int r = GlobalInstance::getInstance()->createRandomNum(rnd) + 1;
	for (int i = 0; i < sizeof(maxqupr) / sizeof(maxqupr[0]); i++)
	{
		if (r <= maxqupr[i])
		{
			return i;
		}
	}

	return 0;
}

int MapEventLayer::getEventMaxQu(int maxqu)
{
	int rnd = maxqupr[maxqu];
	return rnd;
}

int MapEventLayer::getResCountRand(std::string id)
{
	int max = map_eventdata[id].max;
	int min = map_eventdata[id].min;
	int r = GlobalInstance::getInstance()->createRandomNum(max - min + 1) + min;
	return r;
}

void MapEventLayer::loadBoxUI(cocos2d::ui::ImageView* box, std::string resid)
{
	std::vector<std::string> vec_data;
	int qu = 0;
	std::string str = "ui/resbox.png";

	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		qu = getEquipQuRand(resid);
		str = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		str = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_RENS && t <= T_BOX)
	{
		qu = atoi(resid.substr(1).c_str()) - 1;
		str = StringUtils::format("ui/resbox_qu%d.png", qu);
	}

	else if (t == T_EPIECE)
	{
		Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
		pieceicon->setAnchorPoint(Vec2(0, 1));
		pieceicon->setPosition(10, box->getContentSize().height - 10);
		box->addChild(pieceicon);
	}
	box->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	CommonFuncs::playResBoxEffect(box, t, qu, 0);

	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)box->getChildByName("icon");

	str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);
	icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)box->getChildByName("name");
	name->setString(GlobalInstance::map_AllResources[resid].name);
	cocos2d::ui::Text* count = (cocos2d::ui::Text*)box->getChildByName("count");
	int m_count = getResCountRand(resid);
	str = StringUtils::format("%d", m_count);
	count->setString(str);

	vec_data.push_back(resid);
	vec_data.push_back(str);
	str = StringUtils::format("%d", qu);
	vec_data.push_back(str);
	vec_eventrewards.push_back(vec_data);
}

void MapEventLayer::boxEventNode()
{
	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_2->getChildByName("text");
	std::string str = StringUtils::format("event%d_1", m_eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);

	str = StringUtils::format("images/event%d-1.jpg", m_eventindex);
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	cocos2d::ui::Button* actionbtn = (cocos2d::ui::Button*)eventnode_2->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	actionbtn->setTag(2);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("allgetbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* resbox_0 = (cocos2d::ui::ImageView*)eventnode_2->getChildByName("resbox_0");
	cocos2d::ui::ImageView* resbox_1 = (cocos2d::ui::ImageView*)eventnode_2->getChildByName("resbox_1");
	cocos2d::ui::ImageView* resbox_2 = (cocos2d::ui::ImageView*)eventnode_2->getChildByName("resbox_2");

	int r = GlobalInstance::getInstance()->createRandomNum(3) + 1;
	std::string arr[3];
	for (int i = 0; i < r; i++)
	{
		arr[i] = getDataIdByPr();
	}
	if (r == 1)
	{
		resbox_0->setVisible(false);
		resbox_2->setVisible(false);
		loadBoxUI(resbox_1, arr[0]);
	}
	else if (r == 2)
	{
		resbox_1->setVisible(false);
		resbox_0->setPositionX(-134);
		resbox_2->setPositionX(118);
		loadBoxUI(resbox_0, arr[0]);
		loadBoxUI(resbox_2, arr[1]);
	}
	else
	{
		loadBoxUI(resbox_0, arr[0]);
		loadBoxUI(resbox_1, arr[1]);
		loadBoxUI(resbox_2, arr[2]);
	}
}

void MapEventLayer::eventNothing()
{
	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_3->getChildByName("text");
	std::string str = StringUtils::format("event%d_2", m_eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_3->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(0);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("images/event%d-2.jpg", m_eventindex);
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);
}

void MapEventLayer::eventHurt()
{
	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_3->getChildByName("text");
	std::string str = StringUtils::format("event%d_3", m_eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);
	textdesc->setVisible(false);

	Label* m_wordlbl = Label::createWithTTF(ResourceLang::map_lang[str], FONT_NAME, 25);
	m_wordlbl->setLineBreakWithoutSpace(true);
	m_wordlbl->setMaxLineWidth(600);
	m_wordlbl->setPosition(textdesc->getPosition());
	m_wordlbl->setLineSpacing(6.0f);
	m_wordlbl->setAlignment(TextHAlignment::CENTER);
	m_wordlbl->setVerticalAlignment(TextVAlignment::CENTER);
	eventnode_3->addChild(m_wordlbl);
	std::vector<std::string> xstr;
	CommonFuncs::split(ResourceLang::map_lang[str], xstr, ResourceLang::map_lang["juhao"]);
	std::string temp = ResourceLang::map_lang[str];
	std::string gfname = xstr[xstr.size() - 2];
	int findpos = temp.find(gfname);
	if (findpos != std::string::npos)
	{
		temp = temp.substr(findpos + gfname.length(), temp.length() - gfname.length());
		int sindex = (ResourceLang::map_lang[str].length() - temp.length() - gfname.length()) / 3;
		int len = gfname.length() / 3;
		for (int i = sindex; i < sindex + len; i++)
		{
			m_wordlbl->getLetter(i)->setColor(Color3B(255, 0, 0));
		}
	}

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_3->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(0);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("images/event%d-3.jpg", m_eventindex);
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	MapBlockScene* block = (MapBlockScene*)this->getParent();

	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			float hp = GlobalInstance::myCardHeros[i]->getHp();
			hp -= GlobalInstance::myCardHeros[i]->getMaxHp()*0.3;
			if (hp <= 0)
			{
				hp = 0;
				GlobalInstance::myCardHeros[i]->setState(HS_DEAD);
				GlobalInstance::myCardHeros[i]->setPos(0);
				hdcount++;
			}
			GlobalInstance::myCardHeros[i]->setHp(hp);
			if (block!=NULL)
			{
				block->updateHeroUI(i);
			}
		}
	}
}

void MapEventLayer::onExit()
{
	Layer::onExit();
}