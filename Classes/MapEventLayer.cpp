#include "MapEventLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "MyRes.h"
#include "EventBusinessLayer.h"
#include "AnimationEffect.h"
#include "MapBlockScene.h"
#include "DynamicValue.h"
#include "GambleBoxLayer.h"
#include "MovingLabel.h"

#define BETCOSTCOIN 50

USING_NS_CC;

MapEventLayer::MapEventLayer()
{
	lastBetIndex = -1;
	winbs = -1;
	isWin = -1;
}

MapEventLayer::~MapEventLayer()
{
	
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
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

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
	closebtn->setTag(0);
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
		switch (tag)
		{
		case 0:
			AnimationEffect::closeAniEffect((Layer*)this);
			//this->removeFromParentAndCleanup(true);
			break;
		case 1:
			if (m_eventindex == MAP_JUMP || m_eventindex == POS_CAVE)
			{
				int r = GlobalInstance::getInstance()->createRandomNum(10) + 1;
				if (r <= 1)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(true);
					eventnode_3->setVisible(false);
					boxEventNode();
				}
				else if (r <= 5)
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
				this->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			else if (m_eventindex == POS_ELDER)
			{
				int r = GlobalInstance::getInstance()->createRandomNum(10) + 1;
				if (r <= 5)
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
					eventElderExtort();
				}
			}
			else if (m_eventindex == POS_BET)
			{
				eventnode_1->setVisible(false);
				eventnode_2->setVisible(false);
				eventnode_3->setVisible(false);
				eventnode_4->setVisible(true);
				doGambling();
			}
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
				MyRes::Add(resid, count, MYPACKAGE, qu, stonesCount);
			}
			//this->removeFromParentAndCleanup(true);
			AnimationEffect::closeAniEffect((Layer*)this);
		}
			break;
		case 3:
		{
			if (lastBetIndex == -1)
			{
				MovingLabel::show(ResourceLang::map_lang["bettipstext"]);
				return;
			}
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= BETCOSTCOIN)
			{
				DynamicValueInt dvl;
				dvl.setValue(BETCOSTCOIN);
				GlobalInstance::getInstance()->costMyCoinCount(dvl);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
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
		default:
			break;
		}
	}
}

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
	int r = GlobalInstance::getInstance()->createRandomNum(10) + 1;
	if (r < 4)
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

void MapEventLayer::updateCoin(float dt)
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	coincount->setString(str);
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

void MapEventLayer::eventElderExtort()
{
	cocos2d::ui::Widget* costsliver = (cocos2d::ui::Widget*)eventnode_3->getChildByName("costsliver");
	costsliver->setVisible(true);
	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)costsliver->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["elderextort"]);

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

	if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= 5000)
	{
		DynamicValueInt val;
		val.setValue(5000);
		GlobalInstance::getInstance()->costMySoliverCount(val);
	}
	else
	{
		GlobalInstance::getInstance()->costMySoliverCount(GlobalInstance::getInstance()->getMySoliverCount());
	}
}

void MapEventLayer::loadPrData()
{
	std::map<std::string, EventData>::iterator it;
	for (it = GlobalInstance::map_eventdata.begin(); it != GlobalInstance::map_eventdata.end(); it++)
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

int MapEventLayer::getEquipQuRand()
{
	int r = GlobalInstance::getInstance()->createRandomNum(1000) + 1;
	if (r <= 585)
	{
		return 0;
	}
	else if (r <= 885)
	{
		return 1;
	}
	else if (r <= 985)
	{
		return 2;
	}
	else if (r <= 995)
	{
		return 3;
	}
	else
	{
		return 4;
	}
	return 0;
}

int MapEventLayer::getResCountRand(std::string id)
{
	int max = GlobalInstance::map_eventdata[id].max;
	int min = GlobalInstance::map_eventdata[id].min;
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
		qu = getEquipQuRand();
		str = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		str = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	box->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
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