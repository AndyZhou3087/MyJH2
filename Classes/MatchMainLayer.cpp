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


USING_NS_CC;

MatchMainLayer::MatchMainLayer()
{
	clickHero = -1;
	httptag = 0;

	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myOnChallengeHeros[i] != NULL)
		{
			delete GlobalInstance::myOnChallengeHeros[i];
			GlobalInstance::myOnChallengeHeros[i] = NULL;
		}
	}
}

MatchMainLayer::~MatchMainLayer()
{
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

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchMainLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("matchfighttitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//更换队形按钮
	cocos2d::ui::Button* changebtn = (cocos2d::ui::Button*)csbnode->getChildByName("changebtn");
	changebtn->setTag(1000);
	changebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* changebtntxt = (cocos2d::ui::ImageView*)changebtn->getChildByName("text");
	changebtntxt->loadTexture(ResourcePath::makeTextImgPath("changequene_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//保存队形按钮
	cocos2d::ui::Button* savebtn = (cocos2d::ui::Button*)csbnode->getChildByName("savebtn");
	savebtn->setTag(1001);
	savebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::ImageView* savebtntxt = (cocos2d::ui::ImageView*)savebtn->getChildByName("text");
	savebtntxt->loadTexture(ResourcePath::makeTextImgPath("matchsave_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//规则按钮
	cocos2d::ui::Button* rulebtn = (cocos2d::ui::Button*)csbnode->getChildByName("rulebtn");
	rulebtn->setTag(1002);
	rulebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::ImageView* rulebtntxt = (cocos2d::ui::ImageView*)rulebtn->getChildByName("text");
	rulebtntxt->loadTexture(ResourcePath::makeTextImgPath("matchrulebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//匹配按钮
	matchbtn = (cocos2d::ui::Button*)csbnode->getChildByName("matchbtn");
	matchbtn->setTag(1003);
	matchbtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	matchbtntxt = (cocos2d::ui::ImageView*)matchbtn->getChildByName("text");
	matchbtntxt->loadTexture(ResourcePath::makeTextImgPath("matchbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	matchbtntxt->ignoreContentAdaptWithSize(true);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1004);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MatchMainLayer::onBtnClick, this));

	cocos2d::ui::Text* changehint = (cocos2d::ui::Text*)csbnode->getChildByName("changehint");
	changehint->setString(ResourceLang::map_lang["changelineuphint"]);

	cocos2d::ui::Text* matchlvtext = (cocos2d::ui::Text*)csbnode->getChildByName("matchlvtext");
	matchlvtext->setString(ResourceLang::map_lang["matchlvtext"]);

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

	cocos2d::ui::Text* rewardtext = (cocos2d::ui::Text*)csbnode->getChildByName("rewardtext");
	rewardtext->setString(ResourceLang::map_lang["matchrewardtext"]);

	cocos2d::ui::Text* endtimetxt = (cocos2d::ui::Text*)csbnode->getChildByName("endtimetxt");
	endtimetxt->setString(ResourceLang::map_lang["matchendtimetext"]);

	endtime = (cocos2d::ui::Text*)csbnode->getChildByName("endtime");
	endtime->setString("");

	for (int i = 0; i < 6; i++)
	{
		Vec2 pos = Vec2(140 + i % 3 * 215, /*745 + */1060 - i / 3 * 250);
		Sprite* cardnodebg = Sprite::create(ResourcePath::makeImagePath("cardherobox_.png"));
		cardnodebg->setPosition(Vec2(pos.x, pos.y+14));
		this->addChild(cardnodebg, 0);

		m_myCardHerosNode[i] = CardHeroNode::create(1);
		m_myCardHerosNode[i]->setPosition(pos);
		this->addChild(m_myCardHerosNode[i], 1, i);
		m_myCardHerosNode[i]->setData(NULL);
	}

	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	this->addChild(wp, 0, "waitingprogress");

	HttpDataSwap::init(this)->getMyMatchHeros();

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
		if (GlobalInstance::myCardHeros[i] != NULL)
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
					m_myCardHerosNode[clickHero]->runAction(MoveTo::create(0.2f, Vec2(140 + i % 3 * 215, /*745 + */1060 - i / 3 * 250)));
					m_myCardHerosNode[clickHero]->setTag(i);
					GlobalInstance::myOnChallengeHeros[clickHero]->setOnchallengepos(i + 1);
					
					m_myCardHerosNode[i]->setTag(clickHero);
					if (GlobalInstance::myOnChallengeHeros[i] != NULL)
					{
						m_myCardHerosNode[i]->runAction(MoveTo::create(0.2f, Vec2(140 + clickHero % 3 * 215, /*745 + */1060 - clickHero / 3 * 250)));
						GlobalInstance::myOnChallengeHeros[i]->setOnchallengepos(clickHero + 1);
					}
					else
					{
						m_myCardHerosNode[i]->setPosition(Vec2(140 + clickHero % 3 * 215, /*745 + */1060 - clickHero / 3 * 250));
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
			m_myCardHerosNode[clickHero]->setPosition(Vec2(140 + clickHero % 3 * 215, /*745 + */1060 - clickHero / 3 * 250));
			m_myCardHerosNode[clickHero]->setLocalZOrder(1);
		}
		else
		{
			for (int m = 0; m < 6; m++)
			{
				if (GlobalInstance::myOnChallengeHeros[m] != NULL)
				{
					for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
					{
						if (GlobalInstance::vec_myHeros[i]->getState() != HS_DEAD && GlobalInstance::vec_myHeros[i]->getName().compare(GlobalInstance::myOnChallengeHeros[m]->getName()) == 0)
						{
							GlobalInstance::vec_myHeros[i]->setOnchallengepos(GlobalInstance::myOnChallengeHeros[m]->getOnchallengepos());
						}
					}
				}
			}
		}
		clickHero = -1;
	}
}


void MatchMainLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
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
			HttpDataSwap::init(this)->postMyMatchHeros();
		}
			break;
		case 1004:
			AnimationEffect::closeAniEffect(this);
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

void MatchMainLayer::updateUI()
{
	int lv = 0;

	int lvscores[] = {100, 300, 700, 1000, INT32_MAX};
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
	std::string str = StringUtils::format("%d", GlobalInstance::myMatchInfo.matchscore);
	matchexp->setString(str);

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

			std::string heroname = vec_tmp[0];

			for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
			{
				if (GlobalInstance::vec_myHeros[i]->getState() != HS_DEAD && GlobalInstance::vec_myHeros[i]->getName().compare(heroname) == 0)
				{
					Hero* myownhero = GlobalInstance::vec_myHeros[i];
					Hero* hero = new Hero();
					hero->setName(heroname);
					DynamicValueInt dvint;
					dvint.setValue(myownhero->getExp().getValue());
					hero->setExp(dvint);

					hero->setVocation(myownhero->getVocation());

					hero->setPotential(myownhero->getPotential());

					hero->setSex(myownhero->getSex());
					hero->setRandAttr(myownhero->getRandAttr());
					hero->setChangeCount(myownhero->getChangeCount());
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
	
	FightingResultLayer* FRlayer = FightingResultLayer::create(vec_empty, fightret);
	this->addChild(FRlayer);
	AnimationEffect::openAniEffect((Layer*)FRlayer);

	for (unsigned int i = 0; i < 6; i++)
	{
		if (GlobalInstance::matchPairHeros[i] != NULL)
		{
			delete GlobalInstance::matchPairHeros[i];
			GlobalInstance::matchPairHeros[i] = NULL;
		}
	}
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
				if (myhero->getState() != HS_DEAD && myhero->getName().compare(GlobalInstance::myOnChallengeHeros[m]->getName()) == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(myhero->getExp().getValue());
					GlobalInstance::myOnChallengeHeros[m]->setExp(dvint);

					GlobalInstance::myOnChallengeHeros[m]->setVocation(myhero->getVocation());
					GlobalInstance::myOnChallengeHeros[m]->setChangeCount(myhero->getChangeCount());

					for (int k = T_ARMOR; k <= T_NG; k++)
					{
						Equipable* eres = myhero->getEquipable(k);
						if (eres != NULL)
							GlobalInstance::myOnChallengeHeros[m]->setEquipable(eres, k);
					}
					GlobalInstance::myOnChallengeHeros[m]->setHp(GlobalInstance::myOnChallengeHeros[m]->getMaxHp());
				}
			}
		}
	}
}

void MatchMainLayer::onFinish(int code)
{
	if (httptag == 0)
		this->removeChildByName("waitingprogress");

	ErrorHintLayer* networkerrLayer = (ErrorHintLayer*)this->getChildByName("networkerrorlayer");

	if (code == 0)
	{
		if (httptag == 0)
		{
			if (networkerrLayer != NULL)
				networkerrLayer->removeFromParentAndCleanup(true);
			updateUI();
		}
		else if (httptag == 1)
		{
			MovingLabel::show(ResourceLang::map_lang["matchnetsaveok"]); 
		}
		else if (httptag == 2)
		{
			httptag = 3;
			HttpDataSwap::init(this)->getMatchPairData();
		}
		else if (httptag == 3)
		{
			//需要重新设置GlobalInstance::myOnChallengeHeros的数据
			bindHeroData();
			MatchVSLayer* layer = MatchVSLayer::create();
			this->addChild(layer, 100);
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
		}
		
		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	}
}