#include "FightingLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "FightHeroNode.h"
#include "MapBlockScene.h"

USING_NS_CC;

FightingLayer::FightingLayer()
{
	fightcount = 0;
	whoskillindex = -1;
}

FightingLayer::~FightingLayer()
{
	/*for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
	{
		delete m_enemyHeros[i];
	}*/
}


FightingLayer* FightingLayer::create(std::vector<Npc*> enemyHeros)
{
	FightingLayer *pRet = new(std::nothrow)FightingLayer();
	if (pRet && pRet->init(enemyHeros))
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
bool FightingLayer::init(std::vector<Npc*> enemyHeros)
{
	if (!Layer::init())
	{
		return false;
	}

	m_enemyHeros = enemyHeros;
	//LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	//this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* bg = Sprite::create(ResourcePath::makeImagePath("fightingbg.jpg"));
	bg->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	this->addChild(bg);

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("fightLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//°´Å¥
	m_escapebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("escapebtn");
	m_escapebtn->addTouchEventListener(CC_CALLBACK_2(FightingLayer::onBtnClick, this));
	m_escapebtn->setVisible(false);

	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)m_escapebtn->getChildByName("text");

	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("escapebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(145 + i%3*215, 435 -  i/3*240);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i], F_HERO, FS_FIGHTING);
		addChild(fightHeroNode, 0, i);
	}

	for (unsigned int i = 0; i < enemyHeros.size(); i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(145 + i % 3 * 215, 835 + i / 3 * 240);
		fightHeroNode->setData(m_enemyHeros[i], F_NPC, FS_FIGHTING);
		addChild(fightHeroNode, 0, 6+i);
	}

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

void FightingLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void FightingLayer::onExit()
{
	Layer::onExit();
}

void FightingLayer::updateMapHero(int which)
{
	//GlobalInstance::myCardHeros[which] = NULL;
	g_MapBlockScene->updateHeroUI(which);
	fightcount++;

	if (fightcount > 10)
		m_escapebtn->setVisible(true);
}

void FightingLayer::pauseAtkSchedule()
{
	for (int i = 0; i < 6; i++)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
		if (fnode != NULL)
			fnode->pauseTimeSchedule();
	}
	for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
		if (fnode != NULL)
			fnode->pauseTimeSchedule();
	}
}
void FightingLayer::resumeAtkSchedule()
{
	int ret = checkFightResult();
	if (ret < 0)
	{
		for (int i = 0; i < 6; i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
			if (fnode != NULL)
				fnode->resumeTimeSchedule();
		}
		for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
			if (fnode != NULL)
				fnode->resumeTimeSchedule();
		}
	}
	else
	{
		pauseAtkSchedule();
		this->removeFromParentAndCleanup(true);
		if (g_MapBlockScene != NULL)
			g_MapBlockScene->showFightResult(ret);
	}
}

void FightingLayer::showAtk(int fightertag)
{
	if (fightertag < 6)//×Ô¼ºÓ¢ÐÛ¹¥»÷
	{
		for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
			{
				FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
				if (fnode->isVisible() && GlobalInstance::myCardHeros[fightertag]->getHp() > 0)
				{
					int stype = 5;//checkWgSkill(ndata, 0);
					if (stype >= 0)
					{
						if (whoskillindex < 0)
							whoskillindex = fightertag;
						FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(fightertag);
						myfnode->playSkill(stype, m_enemyHeros[i]);
						fnode->attackedSkill(stype, GlobalInstance::myCardHeros[fightertag]);

					}
					else
					{
						float atkhp = GlobalInstance::myCardHeros[fightertag]->getAtk();
						fnode->hurt(atkhp);
					}
					break;
				}
			}
		}
	}
	else//NPC¹¥»÷
	{

		if (whoskillindex >= 0)
		{
			if (GlobalInstance::myCardHeros[whoskillindex] != NULL && GlobalInstance::myCardHeros[whoskillindex]->getHp() > 0)
			{
				FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(whoskillindex);
				if (fnode->isVisible() && m_enemyHeros[fightertag - 6]->getHp() > 0)
				{
					float atkhp = m_enemyHeros[fightertag - 6]->getAtk();
					if (checkWgSkill(GlobalInstance::myCardHeros[whoskillindex], 0) == SKILL_4)
					{
						fnode->hurt(atkhp);
						ResBase* res = MyRes::getMyPutOnResByType(T_WG, GlobalInstance::myCardHeros[whoskillindex]->getName());
						if (res != NULL)
						{
							GlobalInstance::map_GF[res->getId()].roundcount--;
							if (GlobalInstance::map_GF[res->getId()].roundcount <= 0)
								clearSkill();
						}
					}

				}
			}
		}
		else
		{
			for (unsigned int i = 0; i < 6; i++)
			{
				if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getHp() > 0)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
					if (fnode->isVisible() && m_enemyHeros[fightertag - 6]->getHp() > 0)
					{
						float atkhp = m_enemyHeros[fightertag - 6]->getAtk();
						if (checkWgSkill(GlobalInstance::myCardHeros[i], 0) == SKILL_3)
						{
							atkhp = 0;
							ResBase* res = MyRes::getMyPutOnResByType(T_WG, GlobalInstance::myCardHeros[i]->getName());
							if (res != NULL)
							{
								GlobalInstance::map_GF[res->getId()].roundcount--;
								if (GlobalInstance::map_GF[res->getId()].roundcount <= 0)
									clearSkill();
							}
						}
						else
							fnode->hurt(atkhp);

						break;
					}
				}
			}
		}
	}	
}

void FightingLayer::clearSkill()
{
	whoskillindex = -1;
}

void FightingLayer::skillAction(int stype)
{
	if (stype == SKILL_5 || stype == SKILL_6)
	{
		ResBase* res = MyRes::getMyPutOnResByType(T_WG, GlobalInstance::myCardHeros[whoskillindex]->getName());
		if (res != NULL)
		{
			int scount = GlobalInstance::map_GF[res->getId()].skilleff1;
			int ssize = m_enemyHeros.size();
			int fcount = scount > ssize ? ssize : scount;

			for (int m = 0; m < fcount; m++)
			{
				if (m_enemyHeros[m] != NULL && m_enemyHeros[m]->getHp() > 0)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + m);
					if (fnode->isVisible())
					{
						fnode->hurt(GlobalInstance::myCardHeros[whoskillindex]->getAtk());
					}
				}
			}
		}
	}
}

int FightingLayer::checkWgSkill(Npc* data, int npctype)
{
	if (npctype == 0)//×Ô¼ºÓ¢ÐÛ
	{
		Hero* myhero = (Hero*)data;
		ResBase* res = MyRes::getMyPutOnResByType(T_WG, myhero->getName());
		if (res != NULL)
		{
			if (GlobalInstance::map_GF[res->getId()].vec_skillbns[myhero->getVocation()] == 1)
			{
				if (GlobalInstance::map_GF[res->getId()].roundcount > 0)
					return GlobalInstance::map_GF[res->getId()].skill;

				int r = GlobalInstance::getInstance()->createRandomNum(100);
				if (r < GlobalInstance::map_GF[res->getId()].skillrnd)
				{
					return GlobalInstance::map_GF[res->getId()].skill;
				}
			}
		}
	}
	else//NPC
	{

	}
	return NULL;
}

int FightingLayer::checkFightResult()
{
	int index = 0;
	int esize = m_enemyHeros.size();
	for (int i = 0; i < esize; i++)
	{
		if (m_enemyHeros[i] == NULL || m_enemyHeros[i]->getHp() <= 0)
		{
			index++;
		}
	}
	if (index == esize)
		return 1;

	index = 0;
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] == NULL || GlobalInstance::myCardHeros[i]->getState() == HS_DEAD)
		{
			index++;
		}
	}
	if (index == 6)
		return 0;

	return -1;
}