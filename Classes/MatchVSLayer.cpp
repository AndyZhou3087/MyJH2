#include "MatchVSLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "Equip.h"
#include "GongFa.h"
#include "FightingLayer.h"

USING_NS_CC;

MatchVSLayer::MatchVSLayer()
{

}

MatchVSLayer::~MatchVSLayer()
{

}


MatchVSLayer* MatchVSLayer::create()
{
	MatchVSLayer *pRet = new(std::nothrow)MatchVSLayer();
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
bool MatchVSLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 250));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	csbnode = CSLoader::createNode(ResourcePath::makePath("matchVSLayer.csb"));
	this->addChild(csbnode);

	parsePairHeros();
	loadData();

	//auto action = CSLoader::createTimeline(ResourcePath::makePath("matchVSLayer.csb"));
	//effectnode->runAction(action);
	//action->gotoFrameAndPlay(0, false);

	this->scheduleOnce(schedule_selector(MatchVSLayer::delayShowFight), 1);
	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void MatchVSLayer::loadData()
{
	cocos2d::ui::ImageView* myheroboxs[6];
	cocos2d::ui::ImageView* myheroheads[6];

	cocos2d::ui::ImageView* pairheroboxs[6];
	cocos2d::ui::ImageView* pairheroheads[6];

	for (int i = 0; i < 6; i++)
	{
		std::string str = StringUtils::format("mhero%d", i);
		myheroboxs[i] = (cocos2d::ui::ImageView*)csbnode->getChildByName("matchbox0")->getChildByName(str);
		myheroheads[i] = (cocos2d::ui::ImageView*)myheroboxs[i]->getChildByName("headimg");

		Hero* myhero = GlobalInstance::myOnChallengeHeros[i];
		if (myhero != NULL)
		{
			str = StringUtils::format("ui/h_%d_%d.png", myhero->getVocation(), myhero->getSex());
			myheroheads[i]->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

			str = StringUtils::format("ui/herobox_%d.png", myhero->getPotential());
			myheroboxs[i]->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			myheroboxs[i]->setVisible(false);
		}

		str = StringUtils::format("phero%d", i);
		pairheroboxs[i] = (cocos2d::ui::ImageView*)csbnode->getChildByName("matchbox1")->getChildByName(str);
		pairheroheads[i] = (cocos2d::ui::ImageView*)pairheroboxs[i]->getChildByName("headimg");

		Hero* phero = GlobalInstance::matchPairHeros[i];
		if (phero != NULL)
		{
			str = StringUtils::format("ui/h_%d_%d.png", phero->getVocation(), phero->getSex());
			pairheroheads[i]->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

			str = StringUtils::format("ui/herobox_%d.png", phero->getPotential());
			pairheroboxs[i]->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			pairheroboxs[i]->setVisible(false);
		}
	}

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

	cocos2d::ui::Text* mynickname = (cocos2d::ui::Text*)csbnode->getChildByName("mynickname");
	mynickname->setString(GlobalInstance::getInstance()->getMyNickName());
	
	cocos2d::ui::Text* mymatchlv = (cocos2d::ui::Text*)csbnode->getChildByName("mymatchlv");
	std::string str = StringUtils::format("%s%s", ResourceLang::map_lang["matchlvtext_1"].c_str(), ResourceLang::map_lang[lvnamekey].c_str());
	mymatchlv->setString(str);
	cocos2d::ui::Text* pairnickname = (cocos2d::ui::Text*)csbnode->getChildByName("pairnickname");
	pairnickname->setString(GlobalInstance::myMatchInfo.pairnickname);

	cocos2d::ui::Text* pairmatchlv = (cocos2d::ui::Text*)csbnode->getChildByName("pairmatchlv");
	lv = 0;
	for (int i = 0; i < lvsize; i++)
	{
		if (GlobalInstance::myMatchInfo.pairscore < lvscores[i])
		{
			lv = i;
			break;
		}
	}
	lvnamekey = StringUtils::format("matchlvname_%d", lv);
	str = StringUtils::format("%s%s", ResourceLang::map_lang["matchlvtext_1"].c_str(), ResourceLang::map_lang[lvnamekey].c_str());
	pairmatchlv->setString(str);
}

void MatchVSLayer::parsePairHeros()
{
	std::map<std::string, std::string>::iterator it;

	for (it = GlobalInstance::myMatchInfo.map_pairheros.begin(); it != GlobalInstance::myMatchInfo.map_pairheros.end(); it++)
	{
		std::string herodata = GlobalInstance::myMatchInfo.map_pairheros[it->first];
		int index = atoi(it->first.substr(it->first.length() - 1).c_str());
		if (herodata.length() > 0)
		{
			std::vector<std::string> vec_heros;
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(herodata, vec_heros, ";");
			CommonFuncs::split(vec_heros[0], vec_tmp, "-");

			std::string heroname = vec_tmp[0];

			Hero* hero = new Hero();
			hero->setName(heroname);
			DynamicValueInt dvint;
			dvint.setValue(atoi(vec_tmp[1].c_str()));
			hero->setExp(dvint);

			hero->setVocation(atoi(vec_tmp[2].c_str()));

			hero->setPotential(atoi(vec_tmp[3].c_str()));

			hero->setSex(atoi(vec_tmp[4].c_str()));
			hero->setRandAttr(atoi(vec_tmp[5].c_str()));
			hero->setChangeCount(atoi(vec_tmp[6].c_str()));
			hero->setState(HS_ONCHALLENGE);
			GlobalInstance::matchPairHeros[index] = hero;

			if (vec_heros[1].length() > 0)
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(vec_heros[1], vec_tmp, "#");
				for (unsigned int i = 0; i < vec_tmp.size(); i++)
				{
					std::vector<std::string> vec_one;
					CommonFuncs::split(vec_tmp[i], vec_one, "-");
					std::string rid = vec_one[0];

					int m = 0;
					for (; m < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); m++)
					{
						if (rid.compare(0, 1, RES_TYPES_CHAR[m]) == 0)
							break;
					}
					if (m >= T_ARMOR && m <= T_FASHION)
					{
						Equip* res = new Equip();
						res->setId(rid);
						res->setType(m);
						DynamicValueInt dv;
						dv.setValue(1);
						res->setCount(dv);
						res->setWhere(MYEQUIP);

						DynamicValueInt dv1;
						dv1.setValue(atoi(vec_one[1].c_str()));
						res->setQU(dv1);

						DynamicValueInt dv2;
						dv2.setValue(atoi(vec_one[2].c_str()));
						res->setLv(dv2);

						CommonFuncs::split(vec_one[3], res->vec_stones, ",");

						res->setWhos(hero->getName());

						hero->setEquipable(res, m);
					}
					else if (m >= T_WG && m <= T_NG)
					{

						GongFa* res = new GongFa();
						res->setId(rid);
						res->setType(m);
						DynamicValueInt dv;
						dv.setValue(1);
						res->setCount(dv);
						res->setWhere(MYEQUIP);

						DynamicValueInt dv1;
						dv1.setValue(atoi(vec_one[1].c_str()));
						res->setQU(dv1);

						DynamicValueInt dv2;
						dv2.setValue(atoi(vec_one[2].c_str()));
						res->setExp(dv2);

						res->setWhos(hero->getName());
						hero->setEquipable(res, m);
					}

				}
			}
		}
		else
		{
			GlobalInstance::matchPairHeros[index] = NULL;
		}
	}
}

void MatchVSLayer::delayShowFight(float dt)
{
	std::vector<Npc*> vec_pairs;
	for (int i = 0; i < 6; i++)
	{
		vec_pairs.push_back(GlobalInstance::matchPairHeros[i]);
		GlobalInstance::myCardHeros[i] = GlobalInstance::myOnChallengeHeros[i];
	}
	FightingLayer* layer = FightingLayer::create(vec_pairs, 1);
	this->getParent()->addChild(layer, 0, "FightingLayer");
	this->removeFromParentAndCleanup(true);
}