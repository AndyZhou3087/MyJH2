#include "MatchMainLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "MainScene.h"
#include "Const.h"
#include "SelectMyHerosLayer.h"
#include "HttpDataSwap.h"
#include "WaitingProgress.h"
#include "ErrorHintLayer.h"
#include "MatchVSLayer.h"
#include "MatchRuleLayer.h"
#include "FightingResultLayer.h"
#include "MatchRewardLayer.h"
#include "RewardLayer.h"
#include "MatchRankLayer.h"
#include "Equip.h"
#include "MacthFightSucAwdLayer.h"
#include "BuyCoinLayer.h"

USING_NS_CC;

MatchMainLayer::MatchMainLayer()
{
	clickHero = -1;
	httptag = 0;
	isspecifiedfight = false;

	mynormalstageformaion = GlobalInstance::myTakeOnFormation;
}

MatchMainLayer::~MatchMainLayer()
{
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myOnChallengeHeros[i] != NULL)
		{
			delete GlobalInstance::myOnChallengeHeros[i];
			GlobalInstance::myOnChallengeHeros[i] = NULL;
		}
	}
	GlobalInstance::myTakeOnFormation = mynormalstageformaion;
}


MatchMainLayer* MatchMainLayer::create()
{
	MatchMainLayer *pRet = new(std::nothrow)MatchMainLayer();
	if (pRet && pRet->init())
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
bool MatchMainLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
	GlobalInstance::myTakeOnFormation = 0;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchMainLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("matchfighttitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);
	scrollview->setSwallowTouches(true);

	formationInfoNode = csbnode->getChildByName("formationnode");

	studybtn = (cocos2d::ui::Widget*)formationInfoNode->getChildByName("studybtn");
	studybtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onFormationClick, this));
	studybtntxt = (cocos2d::ui::ImageView*)studybtn->getChildByName("btntext");
	studybtntxt->loadTexture(ResourcePath::makeTextImgPath("learnformation_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	addFormationUi();

	carryoutherobox = csbnode->getChildByName("carryoutherobox");

	bigformation = csbnode->getChildByName("formation");
	bigformation->setVisible(false);

	//更换队形按钮
	cocos2d::ui::Button* changebtn = (cocos2d::ui::Button*)csbnode->getChildByName("changebtn");
	changebtn->setTag(1000);
	changebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* changebtntxt = (cocos2d::ui::ImageView*)changebtn->getChildByName("text");
	changebtntxt->loadTexture(ResourcePath::makeTextImgPath("changequene_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//保存队形按钮
	//cocos2d::ui::Button* savebtn = (cocos2d::ui::Button*)csbnode->getChildByName("savebtn");
	//savebtn->setTag(1001);
	//savebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	//cocos2d::ui::ImageView* savebtntxt = (cocos2d::ui::ImageView*)savebtn->getChildByName("text");
	//savebtntxt->loadTexture(ResourcePath::makeTextImgPath("matchsave_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//规则按钮
	cocos2d::ui::Button* rulebtn = (cocos2d::ui::Button*)csbnode->getChildByName("rulebtn");
	rulebtn->setTag(1002);
	rulebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::ImageView* rulebtntxt = (cocos2d::ui::ImageView*)rulebtn->getChildByName("text");
	rulebtntxt->loadTexture(ResourcePath::makeTextImgPath("rule_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//匹配按钮
	matchbtn = (cocos2d::ui::Button*)csbnode->getChildByName("matchbtn");
	matchbtn->setTag(1003);
	matchbtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	matchbtntxt = (cocos2d::ui::ImageView*)matchbtn->getChildByName("text");
	matchbtntxt->loadTexture(ResourcePath::makeTextImgPath("matchbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	matchbtntxt->ignoreContentAdaptWithSize(true);

	//排行榜
	cocos2d::ui::Button* rankbtn = (cocos2d::ui::Button*)csbnode->getChildByName("rankbtn");
	rankbtn->setTag(1006);
	rankbtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));
	cocos2d::ui::ImageView* rankbtnbtntxt = (cocos2d::ui::ImageView*)rankbtn->getChildByName("text");
	rankbtnbtntxt->loadTexture(ResourcePath::makeTextImgPath("ranking_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1004);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	changehintlbl = (cocos2d::ui::Text*)csbnode->getChildByName("changehint");
	changehintlbl->setString(ResourceLang::map_lang["changelineuphint"]);

	changehintindex = GlobalInstance::getInstance()->createRandomNum(3);
	std::string changehintkey = StringUtils::format("changelineuphint%d", changehintindex);
	changehintlbl = (cocos2d::ui::Text*)csbnode->getChildByName("changehint");
	changehintlbl->setString(ResourceLang::map_lang[changehintkey]);

	this->schedule(schedule_selector(MatchMainLayer::updateChangeHint), 3);

	cocos2d::ui::Text* matchexptext = (cocos2d::ui::Text*)csbnode->getChildByName("matchexptext");
	matchexptext->setString(ResourceLang::map_lang["matchexptext"]);

	matchlv = (cocos2d::ui::Text*)csbnode->getChildByName("matchlv");
	matchlv->setString(ResourceLang::map_lang["matchlvname_0"]);

	matchexp = (cocos2d::ui::Text*)csbnode->getChildByName("matchexp");
	matchexp->setString("0");

	needexp = (cocos2d::ui::Text*)csbnode->getChildByName("needexp");
	std::string str = StringUtils::format(ResourceLang::map_lang["neednextexpdesc"].c_str(), 0);
	needexp->setString(str);

	nextlvtext = (cocos2d::ui::Text*)csbnode->getChildByName("nextlvtext");
	nextlvtext->setString("");

	cocos2d::ui::Text* matchwintext = (cocos2d::ui::Text*)csbnode->getChildByName("matchwintext");
	matchwintext->setString(ResourceLang::map_lang["matchwintext"]);

	matchwin = (cocos2d::ui::Text*)csbnode->getChildByName("matchwin");

	endtimetxt = (cocos2d::ui::Text*)csbnode->getChildByName("endtimetxt");
	endtimetxt->setString(ResourceLang::map_lang["matchendtimetext"]);

	endtime = (cocos2d::ui::Text*)csbnode->getChildByName("endtime");
	endtime->setString("");
	
	leftcountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("leftcount");

	for (int i = 0; i < 6; i++)
	{
		Vec2 pos = Vec2(150 + i % 3 * 210, 705 - i / 3 * 220);
		Sprite* cardnodebg = Sprite::create(ResourcePath::makeImagePath("cardherobox_.png"));
		cardnodebg->setPosition(Vec2(pos.x, pos.y+14));
		this->addChild(cardnodebg, 0);

		m_myCardHerosNode[i] = CardHeroNode::create(1);
		m_myCardHerosNode[i]->setPosition(pos);
		this->addChild(m_myCardHerosNode[i], 1, i);
		m_myCardHerosNode[i]->setData(NULL);
	}

	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	this->addChild(wp, 10000, "waitingprogress");

	HttpDataSwap::init(this)->getMyMatchHeros();

	this->schedule(schedule_selector(MatchMainLayer::updatelbl), 1.0f);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(MatchMainLayer::onTouchBegan, this);

	listener->onTouchMoved = CC_CALLBACK_2(MatchMainLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(MatchMainLayer::onTouchEnded, this);
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

bool MatchMainLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
	clickHero = -1;
	beginTouchPoint = touch->getLocation();
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myOnChallengeHeros[i] != NULL)
		{
			float x = m_myCardHerosNode[i]->getPositionX();
			float y = m_myCardHerosNode[i]->getPositionY();

			if (beginTouchPoint.x >= x - 70 && beginTouchPoint.x <= x + 70 && beginTouchPoint.y >= y - 70 && beginTouchPoint.y <= y + 70)
			{
				m_myCardHerosNode[i]->setLocalZOrder(2);
				clickHero = i;
				break;
			}
		}
	}

	return true;
}

void MatchMainLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
	auto movedPoint = touch->getLocation();
	if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
	{
		if (clickHero >= 0)
		{
			auto touchPos = touch->getLocation();
			m_myCardHerosNode[clickHero]->setPosition(touchPos);
			m_myCardHerosNode[clickHero]->setIsDrading(true);
		}
	}
}

void MatchMainLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
	if (clickHero >= 0)
	{
		auto touchPos = touch->getLocation();
		log("touchPos.x = %.f, touchPos.y = %.f", touchPos.x, touchPos.y);
		m_myCardHerosNode[clickHero]->setPosition(touchPos);

		bool ischange = false;
		for (int i = 0; i < 6; i++)
		{
			if (i != clickHero)
			{
				float x = m_myCardHerosNode[i]->getPositionX();
				float y = m_myCardHerosNode[i]->getPositionY();

				if (touchPos.x >= x - 70 && touchPos.x <= x + 70 && touchPos.y >= y - 70 && touchPos.y <= y + 70)
				{
					CardHeroNode* cardnode = m_myCardHerosNode[clickHero];

					m_myCardHerosNode[clickHero]->setLocalZOrder(1);
					m_myCardHerosNode[clickHero]->runAction(MoveTo::create(0.2f, Vec2(150 + i % 3 * 210, /*745 + */705 - i / 3 * 220)));
					m_myCardHerosNode[clickHero]->setTag(i);
					GlobalInstance::myOnChallengeHeros[clickHero]->setOnchallengepos(i + 1);
					
					m_myCardHerosNode[i]->setTag(clickHero);
					if (GlobalInstance::myOnChallengeHeros[i] != NULL)
					{
						m_myCardHerosNode[i]->runAction(MoveTo::create(0.2f, Vec2(150 + clickHero % 3 * 210, /*745 + */705 - clickHero / 3 * 220)));
						GlobalInstance::myOnChallengeHeros[i]->setOnchallengepos(clickHero + 1);
					}
					else
					{
						m_myCardHerosNode[i]->setPosition(Vec2(150 + clickHero % 3 * 210, /*745 + */705 - clickHero / 3 * 220));
						m_myCardHerosNode[i]->updateSelPosLbl();
					}

					m_myCardHerosNode[clickHero] = m_myCardHerosNode[i];
					m_myCardHerosNode[i] = cardnode;

					Hero * myhero = GlobalInstance::myOnChallengeHeros[clickHero];
					GlobalInstance::myOnChallengeHeros[clickHero] = GlobalInstance::myOnChallengeHeros[i];
					GlobalInstance::myOnChallengeHeros[i] = myhero;
					
					ischange = true;
					break;
				}
			}
		}
		if (!ischange)
		{
			if (!carryoutherobox->getBoundingBox().containsPoint(m_myCardHerosNode[clickHero]->getPosition()))
			{
				m_myCardHerosNode[clickHero]->setData(NULL);
				GlobalInstance::myOnChallengeHeros[clickHero]->setOnchallengepos(0);
				delete GlobalInstance::myOnChallengeHeros[clickHero];
				GlobalInstance::myOnChallengeHeros[clickHero] = NULL;
				checkFormation();
			}
			m_myCardHerosNode[clickHero]->setPosition(Vec2(150 + clickHero % 3 * 210, /*745 + */705 - clickHero / 3 * 220));
			m_myCardHerosNode[clickHero]->setLocalZOrder(1);
		}
		else
		{
			checkFormation();
		}
		clickHero = -1;
	}
}


void MatchMainLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::Widget* clicknode = (cocos2d::ui::Widget*)pSender;
	int tag = clicknode->getTag();
	if (tag != 1005)
	{
		CommonFuncs::BtnAction(pSender, type);
	}
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 1000://更换队形
		{
			Layer* layer = SelectMyHerosLayer::create(HS_ONCHALLENGE);
			this->addChild(layer, 3, "selectmyheroslayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case 1001://save http data
			if (checkEmptyHeros())
			{
				MovingLabel::show(ResourceLang::map_lang["mymatchherosempty"]);
				return;
			}
			httptag = 1;
			bindHeroData();
			HttpDataSwap::init(this)->postMyMatchHeros();
			break;
		case 1002://rule
		{
			MatchRuleLayer* layer = MatchRuleLayer::create();
			this->addChild(layer, 1000);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case 1003://getmatch
		{
			if (checkEmptyHeros())
			{
				MovingLabel::show(ResourceLang::map_lang["mymatchherosempty"]);
				return;
			}
			setMatchBtnStatus(1);
			httptag = 2;
			bindHeroData();
			HttpDataSwap::init(this)->postMyMatchHeros();
		}
			break;
		case 1004:
		{
			if (checkEmptyHeros())
			{
				MovingLabel::show(ResourceLang::map_lang["mymatchherosempty"]);
				return;
			}
			HttpDataSwap::init(NULL)->postMyMatchHeros();
			AnimationEffect::closeAniEffect(this);
		}
		break;
		case 1006:
		{
			MatchRankLayer* layer = MatchRankLayer::create();
			this->addChild(layer, 1000, "matchranklayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		default:
			break;
		}
	}
}

bool MatchMainLayer::checkEmptyHeros()
{
	int count = 0;

	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myOnChallengeHeros[i] == NULL)
			count++;
	}
	if (count == 6)
		return true;
	return false;
}

void MatchMainLayer::setMatchBtnStatus(int s)
{
	int langtype = GlobalInstance::getInstance()->getLang();
	if (s == 0)
	{
		matchbtn->setEnabled(true);
		matchbtntxt->loadTexture(ResourcePath::makeTextImgPath("matchbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
	else if (s == 1)
	{
		matchbtn->setEnabled(false);
		matchbtntxt->loadTexture(ResourcePath::makeTextImgPath("matchdoing_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
}

void MatchMainLayer::updatelbl(float dt)
{
	int lv = 0;

	int lvscores[] = { 100, 300, 700, 1000, INT32_MAX };
	int lvsize = sizeof(lvscores) / sizeof(lvscores[0]);

	for (int i = 0; i < lvsize; i++)
	{
		if (GlobalInstance::myMatchInfo.matchscore < lvscores[i])
		{
			lv = i;
			break;
		}
	}

	std::string lvnamekey = StringUtils::format("matchlvname_%d", lv);
	matchlv->setString(ResourceLang::map_lang[lvnamekey]);

	if (lv >= lvsize)
	{
		needexp->setVisible(false);
		nextlvtext->setVisible(false);
	}
	else
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["neednextexpdesc"].c_str(), lvscores[lv] - GlobalInstance::myMatchInfo.matchscore);
		needexp->setString(str);

		str = StringUtils::format("matchlvname_%d", lv + 1);
		nextlvtext->setString(ResourceLang::map_lang[str]);
	}
	std::string str = StringUtils::format("%d", GlobalInstance::myMatchInfo.matchscore);
	matchexp->setString(str);

	int totalcount = GlobalInstance::myMatchInfo.wincount + GlobalInstance::myMatchInfo.lostcount;
	if (totalcount == 0)
		str = "100.00%";
	else
		str = StringUtils::format("%.2f%%", GlobalInstance::myMatchInfo.wincount*100.0f / totalcount);
	matchwin->setString(str);

	std::string leftcountstr = StringUtils::format(ResourceLang::map_lang["matchfightcount1"].c_str(), GlobalInstance::myMatchInfo.matchcount);
	leftcountlbl->setString(leftcountstr);
}

void MatchMainLayer::updateUI()
{
	updatelbl(0);
	endtime->setString(GlobalInstance::myMatchInfo.endtime);

	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		GlobalInstance::vec_myHeros[i]->setOnchallengepos(0);
	}

	std::map<std::string, std::string>::iterator it;

	for (it = GlobalInstance::myMatchInfo.map_myheros.begin(); it != GlobalInstance::myMatchInfo.map_myheros.end(); it++)
	{
		std::string herodata = GlobalInstance::myMatchInfo.map_myheros[it->first];
		int index = atoi(it->first.substr(it->first.length() - 1).c_str());
		if (herodata.length() > 0)
		{
			std::vector<std::string> vec_heros;
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(herodata, vec_heros, ";");
			CommonFuncs::split(vec_heros[0], vec_tmp, "-");

			std::string heroid = vec_tmp[5];

			for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
			{
				if (GlobalInstance::vec_myHeros[i]->getState() != HS_DEAD && GlobalInstance::vec_myHeros[i]->getId().compare(heroid) == 0)
				{
					Hero* myownhero = GlobalInstance::vec_myHeros[i];
					Hero* hero = new Hero(myownhero);
					DynamicValueInt dvint;
					dvint.setValue(myownhero->getExp().getValue());
					hero->setExp(dvint);

					hero->setState(HS_ONCHALLENGE);
					hero->setOnchallengepos(index + 1);
					GlobalInstance::myOnChallengeHeros[index] = hero;
					GlobalInstance::vec_myHeros[i]->setOnchallengepos(index + 1);

					m_myCardHerosNode[index]->setData(GlobalInstance::vec_myHeros[i]);

				}
			}
			GlobalInstance::getInstance()->saveMyHeros();
		}
		else
		{
			GlobalInstance::myOnChallengeHeros[index] = NULL;
		}
	}
}

void MatchMainLayer::onExit()
{
	Layer::onExit();
}

void MatchMainLayer::showFightResult(int ret)
{
	fightret = ret;
	this->scheduleOnce(schedule_selector(MatchMainLayer::delayShowFightResultLayer), 1.0f);
}

void MatchMainLayer::delayShowFightResultLayer(float dt)
{
	std::vector<FOURProperty> vec_empty;
	
	FightingResultLayer* FRlayer = FightingResultLayer::create(vec_empty, fightret, 1);
	this->addChild(FRlayer, 100);
	AnimationEffect::openAniEffect((Layer*)FRlayer);

	for (unsigned int i = 0; i < 6; i++)
	{
		if (GlobalInstance::matchPairHeros[i] != NULL)
		{
			for (int k = T_ARMOR; k <= T_NG; k++)
			{
				Equipable* eres = GlobalInstance::matchPairHeros[i]->getEquipable(k);
				if (eres != NULL)
				{
					delete eres;
					eres = NULL;
				}
			}
			delete GlobalInstance::matchPairHeros[i];
			GlobalInstance::matchPairHeros[i] = NULL;
		}
	}

	if (fightret == 1)
		this->scheduleOnce(schedule_selector(MatchMainLayer::showFightSuccAwdLayer), 0.5f);
}

CardHeroNode* MatchMainLayer::getMyCardHeroNode(int index)
{
	return m_myCardHerosNode[index];
}

void MatchMainLayer::bindHeroData()
{
	for (int m = 0; m < 6; m++)
	{
		if (GlobalInstance::myOnChallengeHeros[m] != NULL)
		{
			for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
			{
				Hero* myhero = GlobalInstance::vec_myHeros[i];
				if (myhero->getState() != HS_DEAD && myhero->getId().compare(GlobalInstance::myOnChallengeHeros[m]->getId()) == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(myhero->getExp().getValue());
					GlobalInstance::myOnChallengeHeros[m]->setExp(dvint);
					GlobalInstance::myOnChallengeHeros[m]->setName(myhero->getName());
					GlobalInstance::myOnChallengeHeros[m]->setVocation(myhero->getVocation());
					GlobalInstance::myOnChallengeHeros[m]->setChangeCount(myhero->getChangeCount());

					for (int k = T_ARMOR; k <= T_NG; k++)
					{
						Equipable* eres = myhero->getEquipable(k);
						if (eres != NULL)
							GlobalInstance::myOnChallengeHeros[m]->setEquipable(eres, k);
					}
					GlobalInstance::myOnChallengeHeros[m]->setState(HS_ONCHALLENGE);
					GlobalInstance::myOnChallengeHeros[m]->setHp(GlobalInstance::myOnChallengeHeros[m]->getMaxHp());
				}
			}
		}
	}

	for (int i = 0; i < 6; i++)
	{
		std::string herokey = StringUtils::format("hero%d", i);
		std::string herodata;
		if (GlobalInstance::myOnChallengeHeros[i] != NULL)
		{
			Hero* hero = GlobalInstance::myOnChallengeHeros[i];
			herodata = StringUtils::format("%s-%d-%d-%d-%d-%s-%d;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex(), hero->getId().c_str(), hero->getChangeCount());

			std::string equipstr;

			for (int k = T_ARMOR; k <= T_NG; k++)
			{
				ResBase* eqres = hero->getEquipable(k);
				if (eqres != NULL)
				{
					std::string onestr;
					if (eqres->getType() >= T_ARMOR && eqres->getType() <= T_FASHION)
					{
						Equip* eres = (Equip*)eqres;
						std::string stonestr;
						int estonesize = eres->vec_stones.size();
						for (int i = 0; i < estonesize; i++)
						{
							if (i < estonesize - 1)
							{
								stonestr.append(eres->vec_stones[i] + ",");
							}
							else
								stonestr.append(eres->vec_stones[i]);
						}
						onestr = StringUtils::format("%s-%d-%d-%s#", eres->getId().c_str(), eres->getQU().getValue(), eres->getLv().getValue(), stonestr.c_str());
					}
					else if (eqres->getType() >= T_WG && eqres->getType() <= T_NG)
					{
						GongFa* gres = (GongFa*)eqres;

						onestr = StringUtils::format("%s-%d-%d#", gres->getId().c_str(), gres->getQU().getValue(), gres->getExp().getValue());
					}
					equipstr.append(onestr);
				}
			}

			if (equipstr.length() > 0)
			{
				equipstr = equipstr.substr(0, equipstr.length() - 1);
			}
			herodata.append(equipstr);
		}
		GlobalInstance::myMatchInfo.map_myheros[herokey] = herodata;
	}
}

void MatchMainLayer::getMatchVsPairData(std::string playerid)
{
	if (checkEmptyHeros())
	{
		MovingLabel::show(ResourceLang::map_lang["mymatchherosempty"]);
		return;
	}
	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	this->addChild(wp, 0, "waitingprogress");

	if (playerid.length() > 0)
		isspecifiedfight = true;
	httptag = 3;
	HttpDataSwap::init(this)->getMatchPairData(playerid);
}

void MatchMainLayer::onFinish(int code)
{
	this->removeChildByName("waitingprogress");

	ErrorHintLayer* networkerrLayer = (ErrorHintLayer*)this->getChildByName("networkerrorlayer");

	if (code == 0)
	{
		if (httptag == 0)
		{
			if (networkerrLayer != NULL)
				networkerrLayer->removeFromParentAndCleanup(true);
			updateUI();
			bindHeroData();
			checkFormation();
		}
		else if (httptag == 1)
		{
			MovingLabel::show(ResourceLang::map_lang["matchnetsaveok"]); 
		}
		else if (httptag == 2)
		{
			getMatchVsPairData();
		}
		else if (httptag == 3)
		{
			//需要重新设置GlobalInstance::myOnChallengeHeros的数据

			this->removeChildByName("matchranklayer");

			bindHeroData();
			MatchVSLayer* layer = MatchVSLayer::create();
			this->addChild(layer, 100);
			setMatchBtnStatus(0);

			if (isspecifiedfight)
				GlobalInstance::myMatchInfo.specifiedcount--;
			else
				GlobalInstance::myMatchInfo.matchcount--;
		}
	}
	else
	{
		if (httptag == 0)
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

		else if (httptag == 2 || httptag == 3)
		{
			setMatchBtnStatus(0);

			if (httptag == 3)
			{
				if (code == 3)
				{
					if (isspecifiedfight)
						MovingLabel::show(ResourceLang::map_lang["specifiednomatch"]);
					else
						MovingLabel::show(ResourceLang::map_lang["nomatch"]);
				}
				else if (code == 4)
					MovingLabel::show(ResourceLang::map_lang["matchfightnocount"]);
				return;
			}
		}
		
		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	}
	isspecifiedfight = false;
}

void MatchMainLayer::showFightSuccAwdLayer(float dt)
{
	MacthFightSucAwdLayer* layer = MacthFightSucAwdLayer::create();
	this->addChild(layer, 2000);
}

void MatchMainLayer::addFormationUi()
{
	int itemwidth = 140;

	int ressize = GlobalInstance::map_formations.size();
	int innerwidth = itemwidth * ressize;

	int contentwidth = scrollview->getContentSize().width;
	if (innerwidth < contentwidth)
		innerwidth = contentwidth;
	scrollview->setInnerContainerSize(Size(innerwidth, scrollview->getContentSize().height));

	std::vector<S_FORMATION*> vec_formationdatas;
	std::map<std::string, S_FORMATION>::iterator it;

	for (it = GlobalInstance::map_formations.begin(); it != GlobalInstance::map_formations.end(); it++)
	{
		vec_formationdatas.push_back(&it->second);
		vec_formationboxs.push_back(NULL);
	}

	std::sort(vec_formationdatas.begin(), vec_formationdatas.end(), sortbylearned);

	for (int m = 0; m < ressize; m++)
	{
		std::string formationid = vec_formationdatas[m]->id;

		std::string boxstr = "ui/formationbox_n.png";

		cocos2d::ui::ImageView* box = cocos2d::ui::ImageView::create(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
		box->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onFormationClick, this));
		box->setTouchEnabled(true);
		int findex = atoi(formationid.substr(2).c_str());
		box->setTag(findex);
		//box->setUserData((void*)GlobalInstance::map_formations[formationid].id.c_str());
		box->setPosition(Vec2(box->getContentSize().width / 2 + m * itemwidth, scrollview->getContentSize().height / 2 + 10));
		scrollview->addChild(box);
		vec_formationboxs[findex - 1] = box;
		std::string iconstr = "ui/formation1_d.png";

		if (GlobalInstance::map_formations[formationid].lv >= 0)
			iconstr = "ui/formation1_n.png";
		cocos2d::ui::ImageView* icon = cocos2d::ui::ImageView::create(iconstr, cocos2d::ui::Widget::TextureResType::PLIST);
		icon->setPosition(Vec2(box->getContentSize().width / 2, box->getContentSize().height / 2));
		box->addChild(icon, 0, "f");


		Label* namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[formationid].name, FONT_NAME, 20);
		namelbl->setColor(Color3B(34, 74, 79));
		namelbl->setPosition(Vec2(box->getContentSize().width / 2, -5));
		box->addChild(namelbl);
	}
}

bool MatchMainLayer::sortbylearned(S_FORMATION* a, S_FORMATION* b)
{
	if (a->lv > b->lv)
		return true;
	return false;
}

void MatchMainLayer::onFormationClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{

	cocos2d::ui::Widget* clicknode = (cocos2d::ui::Widget*)pSender;
	int clicktag = clicknode->getTag();

	if (clicktag > 2000)
		CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		clickflag = true;
		beginTouchPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));
	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{
		Vec2 movedPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));

		if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
			clickflag = false;
	}

	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		if (clicktag < 2000)
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

			if (GlobalInstance::getInstance()->getUnlockChapter() >= 2)
			{
				selectFormation(clicknode->getTag());
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["unlockformationhint"]);
			}
		}
		else if (clicktag < 3000)
		{
			int learnindex = clicktag % 2000;

			int needcoin = 200;

			cocos2d::ui::Text* coincountlbl = (cocos2d::ui::Text*)formationInfoNode->getChildByName("countlbl");
			std::string str = StringUtils::format("%d", needcoin);
			coincountlbl->setString(str);

			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= needcoin)
			{
				cocos2d::ui::ImageView* boxf = (cocos2d::ui::ImageView*)vec_formationboxs[learnindex - 1]->getChildByName("f");
				boxf->loadTexture("ui/formation1_n.png", cocos2d::ui::Widget::TextureResType::PLIST);

				std::string formationid = StringUtils::format("zx%03d", learnindex);

				GlobalInstance::map_formations[formationid].lv = 0;
				GlobalInstance::getInstance()->saveMyFormation(mynormalstageformaion);
				updateFormationInfo(learnindex);
				DynamicValueInt dvint;
				dvint.setValue(needcoin);
				GlobalInstance::getInstance()->costMyCoinCount(dvint);
				std::string showstr = StringUtils::format(ResourceLang::map_lang["learnsucc"].c_str(), GlobalInstance::map_AllResources[formationid].name.c_str());
				MovingLabel::show(showstr);

			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
				Layer* layer = BuyCoinLayer::create(needcoin - GlobalInstance::getInstance()->getMyCoinCount().getValue());
				Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
			}
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["formationlvupnotopen"]);
		}
	}
}

void MatchMainLayer::selectFormation(int index)
{
	if (index > 0)
	{
		if (lastselectformation > 0)
			vec_formationboxs[lastselectformation - 1]->loadTexture("ui/formationbox_n.png", cocos2d::ui::Widget::TextureResType::PLIST);

		vec_formationboxs[index - 1]->loadTexture("ui/formationbox_s.png", cocos2d::ui::Widget::TextureResType::PLIST);

		lastselectformation = index;

		std::string formationid = StringUtils::format("zx%03d", index);
		if (GlobalInstance::map_formations[formationid].lv >= 0)
		{
			takeOnFormation(index);
			GlobalInstance::myTakeOnFormation = index;
		}

	}
	else if (lastselectformation > 0)
	{
		vec_formationboxs[lastselectformation - 1]->loadTexture("ui/formationbox_n.png", cocos2d::ui::Widget::TextureResType::PLIST);
	}
	updateFormationInfo(index);

	GlobalInstance::getInstance()->saveMyFormation(mynormalstageformaion);

	for (int m = 0; m < 6; m++)
	{
		if (GlobalInstance::myOnChallengeHeros[m] != NULL)
		{
			for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
			{
				if (GlobalInstance::vec_myHeros[i]->getState() != HS_DEAD && GlobalInstance::vec_myHeros[i]->getId().compare(GlobalInstance::myOnChallengeHeros[m]->getId()) == 0)
				{
					GlobalInstance::vec_myHeros[i]->setOnchallengepos(GlobalInstance::myOnChallengeHeros[m]->getOnchallengepos());
				}
			}
		}
	}
}

void MatchMainLayer::updateFormationInfo(int index)
{
	std::string formationid = StringUtils::format("zx%03d", index);

	cocos2d::ui::Text* fname = (cocos2d::ui::Text*)formationInfoNode->getChildByName("fname");

	cocos2d::ui::Text* fdesc = (cocos2d::ui::Text*)formationInfoNode->getChildByName("desc");

	Label* desclbl = (Label*)fdesc->getVirtualRenderer();
	desclbl->setLineSpacing(10);

	cocos2d::ui::Text* table = (cocos2d::ui::Text*)formationInfoNode->getChildByName("formationinfotable");

	if (index == 0)
	{
		fname->setString(ResourceLang::map_lang["npcrelation_0"]);
		fdesc->setString(ResourceLang::map_lang["npcrelation_0"]);
		table->setVisible(false);

		studybtn->setVisible(false);
		formationInfoNode->getChildByName("countlbl")->setVisible(false);
		formationInfoNode->getChildByName("coin")->setVisible(false);
	}
	else
	{


		fname->setString(GlobalInstance::map_AllResources[formationid].name);

		if (GlobalInstance::map_formations[formationid].lv >= 0)
		{
			studybtn->setTag(3000 + index);
			studybtn->setVisible(true);
			studybtntxt->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

			table->setVisible(true);

			std::string descstr = GlobalInstance::map_AllResources[formationid].desc;
			for (unsigned int i = 0; i < GlobalInstance::map_formations[formationid].vec_addattr.size(); i++)
			{
				float attrval = GlobalInstance::map_formations[formationid].vec_addattr[i];

				if (attrval > 0.00001)
				{
					std::string astr = StringUtils::format("%.2f", attrval * 100);
					CommonFuncs::replace(descstr, "#", astr);
				}
			}
			fdesc->setString(descstr);
			formationInfoNode->getChildByName("countlbl")->setVisible(false);
			formationInfoNode->getChildByName("coin")->setVisible(false);
		}
		else
		{
			studybtn->setTag(2000 + index);
			studybtn->setVisible(true);
			studybtntxt->loadTexture(ResourcePath::makeTextImgPath("learnformation_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			table->setVisible(false);

			fdesc->setString(ResourceLang::map_lang["notlearntext"]);
			formationInfoNode->getChildByName("countlbl")->setVisible(studybtn->isVisible());
			formationInfoNode->getChildByName("coin")->setVisible(studybtn->isVisible());
		}

		for (int i = 0; i < 6; i++)
		{
			std::string herokey = StringUtils::format("vocationbox%d", i);

			if (index > 0)
			{
				std::string formationid = StringUtils::format("zx%03d", index);
				cocos2d::ui::ImageView* headimg = (cocos2d::ui::ImageView*)table->getChildByName(herokey)->getChildByName("v");
				herokey = StringUtils::format("ui/cardvocation%d.png", GlobalInstance::map_formations[formationid].vec_formation[i] - 1);
				headimg->loadTexture(herokey, cocos2d::ui::Widget::TextureResType::PLIST);
			}

			std::string namekey = StringUtils::format("vocname%d", i);

			if (index > 0)
			{
				std::string formationid = StringUtils::format("zx%03d", index);
				cocos2d::ui::Text* herovocname = (cocos2d::ui::Text*)table->getChildByName(namekey);
				namekey = StringUtils::format("vocation_%d", GlobalInstance::map_formations[formationid].vec_formation[i] - 1);
				herovocname->setString(ResourceLang::map_lang[namekey]);
			}
		}
	}

	//formationInfoNode->getChildByName("countlbl")->setVisible(studybtn->isVisible());
	//formationInfoNode->getChildByName("coin")->setVisible(studybtn->isVisible());
}

void MatchMainLayer::updateChangeHint(float dt)
{
	changehintindex++;
	if (changehintindex > 2)
		changehintindex = 0;
	std::string changehintkey = StringUtils::format("changelineuphint%d", changehintindex);
	changehintlbl->setString(ResourceLang::map_lang[changehintkey]);
}

void MatchMainLayer::checkFormation()
{
	GlobalInstance::myTakeOnFormation = 0;
	std::string matchformation;
	std::map<std::string, S_FORMATION>::iterator it;
	for (it = GlobalInstance::map_formations.begin(); it != GlobalInstance::map_formations.end(); it++)
	{
		int matchcount = 0;
		for (int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myOnChallengeHeros[i] != NULL && GlobalInstance::myOnChallengeHeros[i]->getState() != HS_DEAD && GlobalInstance::myOnChallengeHeros[i]->getVocation() + 1 == it->second.vec_formation[i])
			{
				matchcount++;
			}
		}
		if (matchcount >= 6)
		{
			matchformation = it->first;
			break;
		}
	}

	if (matchformation.length() > 0)
	{
		int takeonf = atoi(matchformation.substr(2).c_str());
		if (GlobalInstance::map_formations[matchformation].lv >= 0)
		{
			GlobalInstance::myTakeOnFormation = takeonf;
		}
	}
	else
	{
		bigformation->stopAllActions();
		bigformation->setVisible(false);
	}
	selectFormation(GlobalInstance::myTakeOnFormation);
}

void MatchMainLayer::takeOnFormation(int formationindex)
{
	vec_formationCandidate.clear();
	vec_selformation.clear();

	std::string formationid = StringUtils::format("zx%03d", formationindex);

	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		for (unsigned int j = 0; j < GlobalInstance::map_formations[formationid].vec_formation.size(); j++)
		{
			if (GlobalInstance::vec_myHeros[i]->getState() != HS_DEAD && GlobalInstance::vec_myHeros[i]->getVocation() + 1 == GlobalInstance::map_formations[formationid].vec_formation[j])
			{
				vec_formationCandidate.push_back(GlobalInstance::vec_myHeros[i]);
				break;
			}
		}
	}
	std::sort(vec_formationCandidate.begin(), vec_formationCandidate.end(), MatchMainLayer::formationCandidateSort);


	std::vector<Hero*>::iterator it;
	for (unsigned int j = 0; j < GlobalInstance::map_formations[formationid].vec_formation.size(); j++)
	{
		for (it = vec_formationCandidate.begin(); it != vec_formationCandidate.end();)
		{
			Hero* hero = *it;

			if (hero->getVocation() + 1 == GlobalInstance::map_formations[formationid].vec_formation[j])
			{
				vec_selformation.push_back(hero);
				it = vec_formationCandidate.erase(it);
				break;
			}

			else
			{
				it++;
			}
		}
	}

	if (vec_selformation.size() >= 6)
	{
		for (int i = 0; i < 6; i++)
		{
			Hero* myhero = vec_selformation[i];

			if (GlobalInstance::myOnChallengeHeros[i] == NULL)
			{
				Hero* hero = new Hero(myhero);
				GlobalInstance::myOnChallengeHeros[i] = hero;
				hero->setState(HS_ONCHALLENGE);
				hero->setOnchallengepos(i+1);
			}

			DynamicValueInt dvint;
			dvint.setValue(myhero->getExp().getValue());
			GlobalInstance::myOnChallengeHeros[i]->setId(myhero->getId());
			GlobalInstance::myOnChallengeHeros[i]->setExp(dvint);
			GlobalInstance::myOnChallengeHeros[i]->setName(myhero->getName());
			GlobalInstance::myOnChallengeHeros[i]->setVocation(myhero->getVocation());
			GlobalInstance::myOnChallengeHeros[i]->setChangeCount(myhero->getChangeCount());

			for (int k = T_ARMOR; k <= T_NG; k++)
			{
				Equipable* eres = myhero->getEquipable(k);
				if (eres != NULL)
					GlobalInstance::myOnChallengeHeros[i]->setEquipable(eres, k);
			}
			GlobalInstance::myOnChallengeHeros[i]->setState(HS_ONCHALLENGE);
			GlobalInstance::myOnChallengeHeros[i]->setHp(GlobalInstance::myOnChallengeHeros[i]->getMaxHp());
			m_myCardHerosNode[i]->setData(GlobalInstance::myOnChallengeHeros[i]);
		}

		bigformation->stopAllActions();
		bigformation->setOpacity(255);
		bigformation->setVisible(true);
		bigformation->runAction(Repeat::create(Sequence::create(FadeIn::create(0.5), FadeOut::create(0.5), FadeIn::create(0.5), NULL), 3));
	}
	else
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["noformation"].c_str(), GlobalInstance::map_AllResources[formationid].name.c_str());
		MovingLabel::show(str);
	}
}

bool MatchMainLayer::formationCandidateSort(Hero* a, Hero* b)
{
	if (a->getPotential() * 10000 + a->getLevel() > b->getPotential() * 10000 + b->getLevel())
	{
		return true;
	}
	return false;
}