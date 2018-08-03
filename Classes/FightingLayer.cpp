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
	whosufferskillindex = -1;
}

FightingLayer::~FightingLayer()
{
	/*for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
	{
		delete m_enemyHeros[i];
	}*/
}


FightingLayer* FightingLayer::create(std::vector<Npc*> enemyHeros, int bgtype)
{
	FightingLayer *pRet = new(std::nothrow)FightingLayer();
	if (pRet && pRet->init(enemyHeros, bgtype))
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
bool FightingLayer::init(std::vector<Npc*> enemyHeros, int bgtype)
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

	std::string fbg = StringUtils::format("fightbg%d.jpg", bgtype);
	Node* bg = Sprite::create(ResourcePath::makeImagePath(fbg));
	bg->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	this->addChild(bg);

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("fightLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮
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

void FightingLayer::clearSkillsData()
{
	clearSkill();

	for (int i = 0; i < 6; i++)
	{
		Hero* myheor = GlobalInstance::myCardHeros[i];
		if (myheor != NULL)
		{
			int t[] = { T_WG , T_NG };

			for (int m = 0; m < 2; m++)
			{
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(t[m], myheor->getName());
				if (gf != NULL)
					gf->setSkillCount(0);
			}
		}
	}
}

void FightingLayer::showAtk(int fightertag)
{
	if (fightertag < 6)//自己英雄攻击
	{
		if (GlobalInstance::myCardHeros[fightertag]->getState() != HS_DEAD)//没有死亡可继续战斗
		{
			int stype = checkWgSkill(GlobalInstance::myCardHeros[fightertag], 0);
			if (stype >= 0)
			{
				if (whoskillindex < 0)//谁发出的技能
					whoskillindex = fightertag;
			}
			int enemyindex = -1;//打哪个敌人
			for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
			{
				if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
				{
					enemyindex = i;//敌人node索引
					if (whosufferskillindex < 0 && whoskillindex >= 0)
						whosufferskillindex = i;//谁遭到技能
					break;
				}
			}

			//计算闪避
			if (enemyindex >= 0)
			{
				FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(6 + enemyindex);
				float dodge = m_enemyHeros[enemyindex]->getDodge();

				int r = GlobalInstance::getInstance()->createRandomNum(10000);

				if (r < dodge * 100)
				{
					clearSkill();
					enemyfnode->hurt(0, 2);
					return;
				}

			}

			if (whoskillindex >= 0)//触发了技能
			{
				FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(whoskillindex);
				myfnode->playSkill(stype, m_enemyHeros[whosufferskillindex]);//施展技能

				FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(6 + whosufferskillindex);
				enemyfnode->attackedSkill(stype, GlobalInstance::myCardHeros[whoskillindex]);//遭受技能
			}
			else//正常攻击
			{
				FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(6 + enemyindex);
				float atkhp = GlobalInstance::myCardHeros[fightertag]->getAtk();

				//计算暴击
				float crit = GlobalInstance::myCardHeros[fightertag]->getCrit();
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_WG, GlobalInstance::myCardHeros[fightertag]->getName());
				if (gf != NULL)
					crit += gf->getCrit();

				int r = GlobalInstance::getInstance()->createRandomNum(10000);
				int state = 0;
				if (r < crit * 100)
				{
					state = 1;
					atkhp *= 2;
				}
				enemyfnode->hurt(atkhp, state);
			}

			//增加自身攻击速度%.2f，持续%d回合。
			if (stype == SKILL_9 && fightertag == whoskillindex)
			{
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
				if (gf != NULL)
				{
					gf->setSkillCount(gf->getSkillCount() - 1);
					if (gf->getSkillCount() <= 0)
					{
						FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(fightertag);
						myfnode->atkspeedbns = 0.0f;
						clearSkill();
					}
				}
			}
			//降低对方攻击速度%.2f，持续%d回合。
			else if (stype == SKILL_10 && enemyindex == whosufferskillindex)//
			{
				FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(6 + enemyindex);
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
				if (gf != NULL)
				{
					gf->setSkillCount(gf->getSkillCount() - 1);
					if (gf->getSkillCount() <= 0)
					{
						enemyfnode->atkspeedbns = 0.0f;
						clearSkill();
					}
					else
					{
						enemyfnode->atkspeedbns = -GlobalInstance::map_GF[gf->getId()].skilleff1;
					}
				}
			}
			//降低对方防御%.2f，持续%d回合。
			else if (stype == SKILL_11 && enemyindex == whosufferskillindex)
			{
				FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(6 + enemyindex);
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
				if (gf != NULL)
				{
					gf->setSkillCount(gf->getSkillCount() - 1);
					if (gf->getSkillCount() <= 0)
					{
						enemyfnode->dfbns = 0.0f;
						clearSkill();
					}
					else
					{
						enemyfnode->dfbns = -GlobalInstance::map_GF[gf->getId()].skilleff1;
					}
				}
			}
			else if (stype == SKILL_12 && fightertag == whoskillindex)
			{
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
				if (gf != NULL)
				{
					FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(fightertag);
					gf->setSkillCount(gf->getSkillCount() - 1);
					if (gf->getSkillCount() <= 0)
					{
						clearSkill();
					}
				}
			}
			else if (stype == SKILL_17 && fightertag == whoskillindex)
			{
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
				if (gf != NULL)
				{
					FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(fightertag);
					gf->setSkillCount(gf->getSkillCount() - 1);
					if (gf->getSkillCount() <= 0)
					{
						myfnode->dfbns = 0.0f;
						clearSkill();
					}
				}
			}
		}
	}
	else//NPC攻击
	{
		int myfindex = -1;
		for (unsigned int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
			{
				myfindex = i;
				break;
			}
		}


		if (whosufferskillindex >= 0)
		{
			if (checkWgSkill(GlobalInstance::myCardHeros[whoskillindex], 0) == SKILL_3)
			{
				GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_WG, GlobalInstance::myCardHeros[whoskillindex]->getName());
				if (gf != NULL)
				{
					gf->setSkillCount(gf->getSkillCount() - 1);
					if (gf->getSkillCount() <= 0)
						clearSkill();
					FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(myfindex);
					myfnode->hurt(0);
				}
			}
		}
		if (whoskillindex >= 0)
		{
			if (GlobalInstance::myCardHeros[whoskillindex] != NULL && GlobalInstance::myCardHeros[whoskillindex]->getState() != HS_DEAD)
			{
				if (checkWgSkill(GlobalInstance::myCardHeros[whoskillindex], 0) == SKILL_4)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(whoskillindex);
					if (fnode->isVisible() && m_enemyHeros[fightertag - 6]->getHp() > 0)
					{
						float atkhp = m_enemyHeros[fightertag - 6]->getAtk();
						fnode->hurt(atkhp);
						GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_WG, GlobalInstance::myCardHeros[whoskillindex]->getName());
						if (gf != NULL)
						{
							gf->setSkillCount(gf->getSkillCount() - 1);
							if (gf->getSkillCount() <= 0)
								clearSkill();
						}
					}
				}
				else if (checkWgSkill(GlobalInstance::myCardHeros[whoskillindex], 0) == SKILL_14)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(whoskillindex);
					if (fnode->isVisible() && m_enemyHeros[fightertag - 6]->getHp() > 0)
					{
						float atkhp = m_enemyHeros[fightertag - 6]->getAtk();

						GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
						if (gf != NULL)
						{
							gf->setSkillCount(gf->getSkillCount() - 1);
							if (gf->getSkillCount() <= 0)
								clearSkill();
						}
						fnode->hurt(atkhp*(1-GlobalInstance::map_GF[gf->getId()].skilleff1));
					}
				}
				else if (checkWgSkill(GlobalInstance::myCardHeros[whoskillindex], 0) == SKILL_15)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(whoskillindex);
					if (fnode->isVisible() && m_enemyHeros[fightertag - 6]->getHp() > 0)
					{
						float atkhp = m_enemyHeros[fightertag - 6]->getAtk();

						GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
						if (gf != NULL)
						{
							fnode->hurt(atkhp);

							FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(fightertag);
							enemyfnode->getData()->setHp(enemyfnode->getData()->getHp() - atkhp*GlobalInstance::map_GF[gf->getId()].skilleff1 / 100);
							enemyfnode->updateHp();
							clearSkill();
						}
					}
				}
				else if (checkWgSkill(GlobalInstance::myCardHeros[whoskillindex], 0) == SKILL_16)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(whoskillindex);
					if (fnode->isVisible() && m_enemyHeros[fightertag - 6]->getHp() > 0)
					{
						float atkhp = m_enemyHeros[fightertag - 6]->getAtk();

						GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
						if (gf != NULL)
						{
							gf->setSkillCount(gf->getSkillCount() - 1);
							if (gf->getSkillCount() <= 0)
							{
								fnode->dfbns = 0.0f;
								clearSkill();
							}
						}
						fnode->hurt(atkhp);
					}
				}
				else if (checkWgSkill(GlobalInstance::myCardHeros[whoskillindex], 0) == SKILL_18)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(whoskillindex);
					if (fnode->isVisible() && m_enemyHeros[fightertag - 6]->getHp() > 0)
					{
						float atkhp = m_enemyHeros[fightertag - 6]->getAtk();

						GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
						if (gf != NULL)
						{
							gf->setSkillCount(gf->getSkillCount() - 1);
							if (gf->getSkillCount() <= 0)
								clearSkill();
						}
						fnode->hurt(atkhp*(1 - GlobalInstance::map_GF[gf->getId()].skilleff1));
					}
				}
			}
		}
		else
		{
			FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(myfindex);
			float atkhp = m_enemyHeros[fightertag - 6]->getAtk();

			//计算暴击
			float crit = m_enemyHeros[fightertag - 6]->getCrit();

			int r = GlobalInstance::getInstance()->createRandomNum(10000);
			int state = 0;
			if (r < crit * 100)
			{
				state = 1;
				atkhp *= 2;
			}

			//计算闪避

			float dodge = GlobalInstance::myCardHeros[myfindex]->getDodge();

			GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[myfindex]->getName());
			if (gf != NULL)
				dodge += gf->getDodge();

			r = GlobalInstance::getInstance()->createRandomNum(10000);
			if (r < dodge * 100)
			{
				state = 2;
				atkhp = 0;
			}

			myfnode->hurt(atkhp, state);
		}
	}	
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
					fnode->hurt(GlobalInstance::myCardHeros[whoskillindex]->getAtk());
				}
			}
		}
	}
	else if (stype == SKILL_7 || stype == SKILL_8)
	{
		vec_myheronode.clear();
		for (unsigned int i = 0; i < 6; i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getHp() > 0)
			{
				FightHeroNode* mynode = (FightHeroNode*)this->getChildByTag(i);
				vec_myheronode.push_back(mynode);
			}
		}
		std::sort(vec_myheronode.begin(), vec_myheronode.end(), sortbyHp_lesscb);

		if (stype == SKILL_7)
		{
			if (vec_myheronode.size() >= 1)
				vec_myheronode[0]->recoveHp();
		}
		else
		{
			int ssize = vec_myheronode.size();
			ResBase* res = MyRes::getMyPutOnResByType(T_WG, GlobalInstance::myCardHeros[whoskillindex]->getName());
			if (res != NULL)
			{
				int scount = GlobalInstance::map_GF[res->getId()].skilleff1;
				int c = ssize < scount ? ssize : scount;

				for (int i = 0; i < c; i++)
				{
					vec_myheronode[i]->recoveHp();
				}
			}
		}
	}
	else if (stype == SKILL_19)
	{
		for (unsigned int m = 0; m < m_enemyHeros.size(); m++)
		{
			if (m_enemyHeros[m] != NULL && m_enemyHeros[m]->getHp() > 0)
			{
				FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + m); 
				for(int n=0;n<2;n++)
					fnode->hurt(GlobalInstance::myCardHeros[whoskillindex]->getAtk());
				break;
			}
		}
	}
	else if (stype == SKILL_20)
	{
		for (unsigned int m = 0; m < m_enemyHeros.size(); m++)
		{
			if (m_enemyHeros[m] != NULL && m_enemyHeros[m]->getHp() > 0)
			{
				ResBase* res = MyRes::getMyPutOnResByType(T_NG, GlobalInstance::myCardHeros[whoskillindex]->getName());
				if (res != NULL)
				{
					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + m);
					for (int n = 0; n < 3; n++)
						fnode->hurt(GlobalInstance::myCardHeros[whoskillindex]->getAtk() * GlobalInstance::map_GF[res->getId()].skilleff1);
				}
				break;
			}
		}
	}
}

int FightingLayer::checkWgSkill(Npc* data, int npctype)
{
	if (npctype == 0)//自己英雄的技能
	{
		Hero* myhero = (Hero*)data;
		int t[] = { T_WG ,T_NG };

		for (int i = 0; i < 2; i++)
		{
			GongFa* gf = (GongFa*)MyRes::getMyPutOnResByType(t[i], myhero->getName());

			if (gf != NULL)
			{
				if (GlobalInstance::map_GF[gf->getId()].vec_skillbns[myhero->getVocation()] == 1)
				{
					if (gf->getSkillCount() > 0)
						return GlobalInstance::map_GF[gf->getId()].skill;

					int r = GlobalInstance::getInstance()->createRandomNum(100);
					if (r < GlobalInstance::map_GF[gf->getId()].skillrnd)
					{
						return GlobalInstance::map_GF[gf->getId()].skill;
					}
				}
			}
		}
	}
	else//NPC触发的技能
	{

	}
	return -1;
}

void FightingLayer::clearSkill()
{
	whoskillindex = -1;
	whosufferskillindex = -1;
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

bool FightingLayer::sortbyHp_lesscb(FightHeroNode* a, FightHeroNode* b)
{
	if (a->getData()->getHp() < b->getData()->getHp())
		return true;
	else
		return false;
}