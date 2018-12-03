#include "FightingLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "FightHeroNode.h"
#include "MapBlockScene.h"
#include "SoundManager.h"
#include "NewGuideLayer.h"
#include "Shake.h"
#include "MatchMainLayer.h"

USING_NS_CC;

FightingLayer::FightingLayer()
{
	fightcount = 0;
}

FightingLayer::~FightingLayer()
{
	for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
	{
		if (m_enemyHeros[i] != NULL)
		{
			m_enemyHeros[i] = NULL;
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
			fnode->map_skillattricon.clear();
		}
	}
	clearSkillsData(0);
	clearSkillsData(1);
}


FightingLayer* FightingLayer::create(std::vector<Hero*> myHeros, std::vector<Npc*> enemyHeros, int bgtype)
{
	FightingLayer *pRet = new(std::nothrow)FightingLayer();
	if (pRet && pRet->init(myHeros, enemyHeros, bgtype))
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
bool FightingLayer::init(std::vector<Hero*> myHeros, std::vector<Npc*> enemyHeros, int bgtype)
{
	if (!Layer::init())
	{
		return false;
	}

	m_enemyHeros = enemyHeros;
	m_myHeros = myHeros;
	//LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	//this->addChild(color,0,"colorLayer");

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

	//cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)m_escapebtn->getChildByName("text");

	//actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("escapebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	for (unsigned int i = 0; i < enemyHeros.size(); i++)
	{
		if (enemyHeros[i] != NULL)
		{
			FightHeroNode * fightHeroNode = FightHeroNode::create();
			fightHeroNode->setPosition(145 + i % 3 * 215, 860 + i / 3 * 260);
			FIGHTDATA_TYPE datatype = F_NPC;
			if (enemyHeros[i]->getId().length() > 10)
				datatype = F_HERO;
			fightHeroNode->setData(m_enemyHeros[i], datatype, FS_FIGHTING);
			addChild(fightHeroNode, i, 6 + i);
		}
	}

	for (unsigned int i = 0; i < m_myHeros.size(); i++)
	{
		if (m_myHeros[i] != NULL)
		{
			FightHeroNode * fightHeroNode = FightHeroNode::create();
			fightHeroNode->setPosition(145 + i % 3 * 215, 460 - i / 3 * 260);
			m_myHeros[i]->setFightRound(0);
			fightHeroNode->setData(m_myHeros[i], F_HERO, FS_FIGHTING);
			addChild(fightHeroNode, 6 + i, i);
		}
	}

	checkNewGuide();

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

void FightingLayer::checkNewGuide()
{
	if (NewGuideLayer::checkifNewerGuide(FIRSTGUIDESTEP))
	{
		if (NewGuideLayer::checkifNewerGuide(6))
			showNewerGuide(6);
		else if (NewGuideLayer::checkifNewerGuide(7))
		{
			showNewerGuide(7);
		}
		else if (NewGuideLayer::checkifNewerGuide(8))
		{
			showNewerGuide(8);
		}
		else if (NewGuideLayer::checkifNewerGuide(9))
		{
			showNewerGuide(9);
		}
		else if (NewGuideLayer::checkifNewerGuide(10))
		{
			showNewerGuide(10);
		}
	}
}

void FightingLayer::showNewerGuide(int step)
{
	pauseAtkSchedule();
	std::vector<Node*> nodes;
	if (step == 6)
	{
		
	}
	else if (step == 7)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(0);
		nodes.push_back(fnode);
	}
	else if (step == 8)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(3);
		nodes.push_back(fnode);
	}
	else if (step == 9)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(1);
		nodes.push_back(fnode);
	}
	else if (step == 10)
	{
		FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(5);
		nodes.push_back(fnode);
	}
	if (g_MapBlockScene != NULL)
	{
		g_MapBlockScene->showNewerGuideNode(step, nodes);
	}
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
	if (g_MapBlockScene != NULL)
	{
		g_MapBlockScene->updateHeroUI(which);
		fightcount++;

		if (fightcount > 10)
			m_escapebtn->setVisible(true);
	}
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
			if (fnode != NULL && fnode->isPlaySkillAnim)
			{
				log("zhou < 6 1");
				return;
			}
		}
		for (unsigned int i = 0; i < 6; i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
			if (fnode != NULL && fnode->isPlaySkillAnim)
			{
				log("zhou >= 6 1");
				return;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
			if (fnode != NULL)
			{
				log("zhou < 6 2");
				fnode->resumeTimeSchedule();
			}
		}
		for (unsigned int i = 0; i < 6; i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
			if (fnode != NULL)
			{
				log("zhou >= 6 2");
				fnode->resumeTimeSchedule();
			}
		}
	}
	else
	{
		fightOver(ret);
	}
}

void FightingLayer::fightOver(int ret)
{
	pauseAtkSchedule();
	this->runAction(Sequence::create(DelayTime::create(0.7f), RemoveSelf::create(), NULL));
	if (g_MapBlockScene != NULL)
		g_MapBlockScene->showFightResult(ret);
	else
	{
		if (g_mainScene != NULL)
		{
			MatchMainLayer* matchlayer = (MatchMainLayer*)g_mainScene->getChildByName("8pkground");
			matchlayer->showFightResult(ret);
		}
	}
}

void FightingLayer::clearSkillsData(int type)
{
	if (type == 0)
	{
		for (unsigned int i = 0; i < m_myHeros.size(); i++)
		{
			Hero* myhero = m_myHeros[i];
			if (myhero != NULL)
			{
				int t[] = { T_WG , T_NG };

				for (int m = 0; m < 2; m++)
				{
					GongFa* gf = (GongFa*)myhero->getEquipable(t[m]);
					if (gf != NULL)
						myhero->clearSkill(gf);
				}
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getId().length() > 10)//hero角色，
			{
				Hero* myhero = (Hero*)m_enemyHeros[i];
				if (myhero != NULL)
				{
					int t[] = { T_WG , T_NG };

					for (int m = 0; m < 2; m++)
					{
						GongFa* gf = (GongFa*)myhero->getEquipable(t[m]);
						if (gf != NULL)
							myhero->clearSkill(gf);
					}
				}
			}
		}
	}
}

void FightingLayer::heroFight(int fightertag)
{
	int atkindex = -1;
	Hero* atkhero = NULL;
	int whoatk = 0;//0--我方攻击；1--对方攻击
	if (fightertag < 6)//我方攻击
	{
		atkindex = fightertag;
		atkhero = m_myHeros[atkindex];//攻击的英雄
	}
	else
	{
		atkindex = fightertag - 6;
		atkhero = (Hero*)m_enemyHeros[atkindex];
		whoatk = 1;
	}

	if (atkhero->getState() != HS_DEAD)//没有死亡可继续战斗
	{
		float atkhp = atkhero->getAtk();
		int atindex = -1;
		int astype = -1;

		std::vector<Npc*> vec_tmp;
		if (whoatk == 1)
		{	
			for (unsigned int i = 0; i < m_myHeros.size(); i++)
				vec_tmp.push_back(m_myHeros[i]);
		}
		else
		{
			for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
				vec_tmp.push_back(m_enemyHeros[i]);
		}
		for (unsigned int i = 0; i < vec_tmp.size(); i++)
		{
			if (vec_tmp[i] != NULL && vec_tmp[i]->getId().length() > 10)
			{
				Hero* myhero = (Hero*)vec_tmp[i];
				if (myhero != NULL && myhero->getState() != HS_DEAD && myhero->getSkillingType() >= 0)//释放技能中
				{
					if (astype == SKILL_4)
					{
						astype = myhero->getSkillingType();
						GongFa* gf = myhero->checkSkillWg();
						gf->setSkillCount(gf->getSkillCount() - 1);
						if (gf->getSkillCount() <= 0)
						{
							m_myHeros[i]->clearSkill(gf);
						}
						atindex = i;
					}
				}
			}
		}

		if (atindex < 0)
		{
			atindex = calcAttackNodeIndex(fightertag, whoatk);
		}

		if (atindex < 0)
			return;

		int atnodetag = atindex;

		if (whoatk == 0)
			atnodetag += 6;

		FightHeroNode* afnode = (FightHeroNode*)this->getChildByTag(atnodetag);
		
		if (astype < 0)
		{
			if (vec_tmp[atindex]->getId().length() > 10)
			{
				Hero* myhero = (Hero*)vec_tmp[atindex];
				GongFa* gf = myhero->checkSkillWg();
				if (gf != NULL)
				{
					astype = GlobalInstance::map_GF[gf->getId()].skill;
					if (astype == SKILL_15)
					{
						myhero->vec_whosufferskill.clear();
						myhero->vec_whosufferskill.push_back(fightertag);
						afnode->hurt(atkhp, -1);
						if (myhero->getHp() > 0)
							afnode->playSkill(astype, (FightHeroNode*)this->getChildByTag(fightertag));
						return;
					}
					else if (astype == SKILL_18)
					{
						myhero->vec_whosufferskill.clear();

						int index = atindex;
						if (whoatk == 0)
							index = atindex + 6;
						myhero->vec_whosufferskill.push_back(index);
						afnode->hurt(atkhp*(1 - GlobalInstance::map_GF[gf->getId()].skilleff1 / 100), -1);
						if (myhero->getHp() > 0)
							afnode->playSkill(astype, afnode);
						return;
					}
					else if (astype == SKILL_20)
					{
						myhero->vec_whosufferskill.clear();
						myhero->vec_whosufferskill.push_back(fightertag);
						afnode->hurt(atkhp, -1);
						if (myhero->getHp() > 0)
							afnode->playSkill(astype, (FightHeroNode*)this->getChildByTag(fightertag));
						return;
					}
				}
			}
		}
		else
		{
			afnode->runAction(Shake::create(0.2f, 5.0f));

			Npc* attackdata = NULL;
			if (whoatk == 0)
				attackdata = m_enemyHeros[atindex];
			else
				attackdata = m_myHeros[atindex];

			float dodge = attackdata->getDodge();

			int rdodge = GlobalInstance::getInstance()->createRandomNum(10000);

			if (rdodge < dodge * 100)
			{
				afnode->hurt(0, 2);
				atkhero->setFightRound(atkhero->getFightRound() + 1);
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DODGE);
				return;
			}
		}

		GongFa* gf = atkhero->checkSkillWg();//触发技能的功法
		int stype = -1;
		if (gf != NULL && GlobalInstance::map_GF[gf->getId()].skill != SKILL_13 && GlobalInstance::map_GF[gf->getId()].skill != SKILL_15 && GlobalInstance::map_GF[gf->getId()].skill != SKILL_18 && GlobalInstance::map_GF[gf->getId()].skill != SKILL_20)//没有触发技能
		{
			atkhero->vec_whosufferskill.clear();
			stype = GlobalInstance::map_GF[gf->getId()].skill;
			if (atkhero->getSkillingType() < 0)//谁发出的技能
				atkhero->setSkillingType(stype);
		}

		if (atkhero->getSkillingType() >= 0)//触发了技能
		{
			int attackindex = -1;

			if (stype == SKILL_5)//攻击多个目标
			{
				int scount = GlobalInstance::map_GF[gf->getId()].skilleff2;

				std::vector<int> vec(calcSkill5AttackNodeIndex(fightertag, whoatk, scount));
				for (unsigned int i = 0; i < vec.size(); i++)
				{
					if (whoatk == 0)
						atkhero->vec_whosufferskill.push_back(6 + vec[i]);
					else
						atkhero->vec_whosufferskill.push_back(vec[i]);
				}

			}
			else if (stype == SKILL_6)
			{
				int scount = GlobalInstance::map_GF[gf->getId()].skilleff2;
				std::vector<int> vec(calcSkill6AttackNodeIndex(fightertag, whoatk, scount));
				for (unsigned int i = 0; i < vec.size(); i++)
				{
					if (whoatk == 0)
						atkhero->vec_whosufferskill.push_back(6 + vec[i]);
					else
						atkhero->vec_whosufferskill.push_back(vec[i]);
				}
			}
			else if (stype == SKILL_7 || stype == SKILL_8)
			{
				vec_atkheronode.clear();

				if (fightertag < 6)
				{
					for (unsigned int i = 0; i < m_myHeros.size(); i++)
					{
						if (m_myHeros[i] != NULL && m_myHeros[i]->getState() != HS_DEAD)
						{
							FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
							vec_atkheronode.push_back(fnode);
						}
					}
				}
				else
				{
					for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
					{
						if (m_enemyHeros[i] != NULL && ((Hero*)m_enemyHeros[i])->getState() != HS_DEAD)
						{
							FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
							vec_atkheronode.push_back(fnode);
						}
					}

				}
				if (vec_atkheronode.size() > 0)
				{
					std::sort(vec_atkheronode.begin(), vec_atkheronode.end(), sortbyHp_lesscb);

					if (stype == SKILL_7)
					{
						atkhero->vec_whosufferskill.push_back(vec_atkheronode[0]->getTag());
					}
					else
					{
						int ssize = vec_atkheronode.size();

						int scount = GlobalInstance::map_GF[gf->getId()].skilleff2;
						int c = ssize < scount ? ssize : scount;
						for (int i = 0; i < c; i++)
						{
							atkhero->vec_whosufferskill.push_back(vec_atkheronode[i]->getTag());
						}
					}
				}
			}
			else if (stype == SKILL_9 || stype == SKILL_12)
			{
				atkhero->vec_whosufferskill.push_back(fightertag);

				GongFa* gf = atkhero->checkSkillWg();
				if (gf->getSkillCount() < GlobalInstance::map_GF[gf->getId()].skilleff2)
				{
					float atk = atkhero->getAtk();
					if (stype == SKILL_12)
						atk *= (1 + GlobalInstance::map_GF[gf->getId()].skilleff1 / 100);

					attackindex = calcAttackNodeIndex(fightertag, whoatk);

					if (whoatk == 0)
						attackindex += 6;

					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(attackindex);
					fnode->hurt(atk, 0);
				}
			}
			else if (stype == SKILL_17)
			{
				atkhero->vec_whosufferskill.push_back(fightertag);

				GongFa* gf = atkhero->checkSkillWg();
				if (gf->getSkillCount() < GlobalInstance::map_GF[gf->getId()].skilleff2)
				{
					float atk = atkhero->getAtk();
					atk *= (1 + GlobalInstance::map_GF[gf->getId()].skilleff1 / 100);
					
					attackindex = calcAttackNodeIndex(fightertag, whoatk);

					if (whoatk == 0)
						attackindex += 6;

					FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(attackindex);
					fnode->hurt(atk, 0);

				}
			}
			else if (stype == SKILL_2)
			{
				attackindex = calcSkill2AttackNodeIndex(fightertag, whoatk);
				if (whoatk == 0)
					attackindex += 6;
				atkhero->vec_whosufferskill.push_back(attackindex);
			}
			else//技能攻击单个目标
			{
				attackindex = calcAttackNodeIndex(fightertag, whoatk);

				if (whoatk == 0)
					attackindex += 6;
				atkhero->vec_whosufferskill.push_back(attackindex);
			}

			//不是增加自身属性
			if (stype >= 0 && stype != SKILL_7 && stype != SKILL_8 && stype != SKILL_9 && stype != SKILL_12 && stype != SKILL_17)
			{
				bool isdodge = false;
				for (unsigned int i = 0; i < atkhero->vec_whosufferskill.size(); i++)
				{
					FightHeroNode* attacknode = (FightHeroNode*)this->getChildByTag(atkhero->vec_whosufferskill[i]);
	
					Npc* attackdata = NULL;
					if (whoatk == 0)
						attackdata = m_enemyHeros[atkhero->vec_whosufferskill[i] - 6];
					else
						attackdata = m_myHeros[atkhero->vec_whosufferskill[i]];

					float dodge = attackdata->getDodge();

					int r = GlobalInstance::getInstance()->createRandomNum(10000);

					if (r < dodge * 100)
					{
						attackdata->setIsDodge(true);
						isdodge = true;
						attacknode->hurt(0, 2);
					}
					else
					{
						attackdata->setIsDodge(false);
					}
				}
				if (isdodge)
				{
					SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DODGE);
				}
				else
				{
					SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_ATK);
				}
			}

			//释放技能
			FightHeroNode* atkfnode = (FightHeroNode*)this->getChildByTag(fightertag);

			FightHeroNode* attackfnode = (FightHeroNode*)this->getChildByTag(atkhero->vec_whosufferskill[0]);

			atkfnode->playSkill(atkhero->getSkillingType(), attackfnode);//技能动画,1对1
		}
		else//没有触发技能
		{
			int attackindex = atindex;
			if (atindex < 0)
				attackindex = calcAttackNodeIndex(fightertag, whoatk);

			Npc* attackdata = NULL;
			if (whoatk == 0)
				attackdata = m_enemyHeros[attackindex];
			else
				attackdata = m_myHeros[attackindex];

			if (whoatk == 0)
				attackindex += 6;

			FightHeroNode* attackfnode = (FightHeroNode*)this->getChildByTag(attackindex);
			attackfnode->runAction(Shake::create(0.2f, 5.0f));

			float dodge = attackdata->getDodge();

			int rdodge = GlobalInstance::getInstance()->createRandomNum(10000);

			if (rdodge < dodge * 100)
			{
				attackfnode->hurt(0, 2);
				atkhero->setFightRound(atkhero->getFightRound() + 1);
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DODGE);
				return;
			}

			//正常攻击
			float atkhp = atkhero->getAtk();

			//计算暴击
			float crit = atkhero->getCrit();
			GongFa* gf = (GongFa*)atkhero->getEquipable(T_WG);
			if (gf != NULL)
				crit += gf->getCrit();

			int rcrit = GlobalInstance::getInstance()->createRandomNum(10000);
			int state = 0;
			if (rcrit < crit * 100)
			{
				state = 1;
				atkhp *= 2;

				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_CRIT);
			}
			else
			{
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_ATK);
			}
			atkhero->setFightRound(atkhero->getFightRound() + 1);
			attackfnode->hurt(atkhp, state);
		}
	}
}


void FightingLayer::showAtk(int fightertag)
{
	int ret = checkFightResult();
	if (ret >= 0)
	{
		fightOver(ret);
		return;
	}

	if (fightertag < 6)//自己英雄攻击
	{
		heroFight(fightertag);
	}
	else//NPC攻击
	{
		if (m_enemyHeros[fightertag - 6]->getId().length() > 10)
		{
			heroFight(fightertag);
		}
		else
		{
			float atkhp = m_enemyHeros[fightertag - 6]->getAtk();
			int myfindex = -1;
			int stype = -1;
			for (unsigned int i = 0; i < m_myHeros.size(); i++)
			{
				Hero* myhero = m_myHeros[i];
				if (myhero != NULL && myhero->getState() != HS_DEAD && myhero->getSkillingType() >= 0)//释放技能中
				{
					stype = myhero->getSkillingType();
					if (stype == SKILL_4)
					{
						GongFa* gf = m_myHeros[i]->checkSkillWg();
						gf->setSkillCount(gf->getSkillCount() - 1);
						if (gf->getSkillCount() <= 0)
						{
							m_myHeros[i]->clearSkill(gf);
						}
						myfindex = i;
					}
				}
			}

			if (myfindex < 0)
			{
				myfindex = calcAttackNodeIndex(fightertag, 1);
			}

			FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(myfindex);

			if (stype < 0)
			{
				Hero* myhero = m_myHeros[myfindex];
				GongFa* gf = myhero->checkSkillWg();
				if (gf != NULL)
				{
					stype = GlobalInstance::map_GF[gf->getId()].skill;
					if (stype == SKILL_15)
					{
						m_myHeros[myfindex]->vec_whosufferskill.clear();
						m_myHeros[myfindex]->vec_whosufferskill.push_back(fightertag);
						myfnode->hurt(atkhp, -1);
						if (m_myHeros[myfindex]->getHp() > 0)
							myfnode->playSkill(stype, (FightHeroNode*)this->getChildByTag(fightertag));
						return;
					}
					else if (stype == SKILL_18)
					{
						m_myHeros[myfindex]->vec_whosufferskill.clear();
						m_myHeros[myfindex]->vec_whosufferskill.push_back(myfindex);
						myfnode->hurt(atkhp*(1 - GlobalInstance::map_GF[gf->getId()].skilleff1 / 100), -1);
						if (m_myHeros[myfindex]->getHp() > 0)
							myfnode->playSkill(stype, myfnode);
						return;
					}
					else if (stype == SKILL_20)
					{
						m_myHeros[myfindex]->vec_whosufferskill.clear();
						m_myHeros[myfindex]->vec_whosufferskill.push_back(fightertag);
						myfnode->hurt(atkhp, -1);
						if (m_myHeros[myfindex]->getHp() > 0)
							myfnode->playSkill(stype, (FightHeroNode*)this->getChildByTag(fightertag));
						return;
					}
				}
			}
			myfnode->runAction(Shake::create(0.2f, 5.0f));

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

			float dodge = m_myHeros[myfindex]->getDodge();

			GongFa* gf = (GongFa*)m_myHeros[myfindex]->getEquipable(T_NG);
			if (gf != NULL)
				dodge += gf->getDodge();

			r = GlobalInstance::getInstance()->createRandomNum(10000);
			if (r < dodge * 100)
			{
				state = 2;
				atkhp = 0;
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DODGE);
			}
			else
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_HURT);
			myfnode->hurt(atkhp, state);
		}
	}
}

int FightingLayer::checkFightResult()
{
	int index = 0;
	int esize = m_enemyHeros.size();
	for (int i = 0; i < esize; i++)
	{
		if (m_enemyHeros[i] == NULL)
		{
			index++;
		}
		else
		{
			if (m_enemyHeros[i]->getId().length() <= 10)//地图中的NPC
			{
				if (m_enemyHeros[i]->getHp() <= 0)//地图中的NPC
					index++;
			}
			else
			{
				if (((Hero*)m_enemyHeros[i])->getState() == HS_DEAD)
					index++;
			}
		}
	}
	if (index == esize)
		return 1;

	index = 0;
	for (unsigned int i = 0; i < m_myHeros.size(); i++)
	{
		if (m_myHeros[i] == NULL || m_myHeros[i]->getState() == HS_DEAD)
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
	if (a->getData()->getHp() / a->getData()->getMaxHp() < b->getData()->getHp() / b->getData()->getMaxHp())
		return true;
	else
		return false;
}

int FightingLayer::calcAttackNodeIndex(int fighterindex, int type)
{
	if (type == 0)
	{
		vec_enemyfront3node.clear();
		vec_enemyback3node.clear();
		int enemycount = m_enemyHeros.size();
		for (int i = 0; i < enemycount; i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
			{
				if (i < 3)
					vec_enemyfront3node.push_back((FightHeroNode*)this->getChildByTag(6 + i));
				else
					vec_enemyback3node.push_back((FightHeroNode*)this->getChildByTag(6 + i));
			}
		}

		int findex = fighterindex;

		if (findex >= 3)
			findex -= 3;

		FightHeroNode* enemynode = (FightHeroNode*)this->getChildByTag(6 + findex);
		if (enemynode != NULL && findex < enemycount && m_enemyHeros[findex] != NULL && m_enemyHeros[findex]->getHp() > 0)//自身位置正前方前排
			return findex;

		enemynode = (FightHeroNode*)this->getChildByTag(6 + findex + 3);
		if (enemynode != NULL && findex + 3 < enemycount && m_enemyHeros[findex + 3] != NULL && m_enemyHeros[findex + 3]->getHp() > 0)//正前方后排位置
			return findex + 3;

		if (vec_enemyfront3node.size() > 0)
		{
			std::sort(vec_enemyfront3node.begin(), vec_enemyfront3node.end(), sortbyHp_lesscb);
			return vec_enemyfront3node[0]->getTag() - 6;
		}
		else
		{
			if (vec_enemyback3node.size() > 0)
			{
				std::sort(vec_enemyback3node.begin(), vec_enemyback3node.end(), sortbyHp_lesscb);
				return vec_enemyback3node[0]->getTag() - 6;
			}
		}
	}
	else
	{
		vec_myfront3node.clear();
		vec_myback3node.clear();

		for (unsigned int i = 0; i < m_myHeros.size(); i++)
		{
			if (m_myHeros[i] != NULL && m_myHeros[i]->getState() != HS_DEAD)
			{
				if (i < 3)
					vec_myfront3node.push_back((FightHeroNode*)this->getChildByTag(i));
				else
					vec_myback3node.push_back((FightHeroNode*)this->getChildByTag(i));
			}
		}

		int findex = fighterindex - 6;

		if (findex >= 3)
			findex -= 3;

		FightHeroNode* mynode = (FightHeroNode*)this->getChildByTag(findex);
		if (mynode != NULL && m_myHeros[findex] != NULL && m_myHeros[findex]->getState() != HS_DEAD)//自身位置正前方
			return findex;

		mynode = (FightHeroNode*)this->getChildByTag(findex + 3);
		if (mynode != NULL && m_myHeros[findex + 3] != NULL && m_myHeros[findex + 3]->getState() != HS_DEAD)//正前方后排位置
			return findex + 3;

		if (vec_myfront3node.size() > 0)
		{
			std::sort(vec_myfront3node.begin(), vec_myfront3node.end(), sortbyHp_lesscb);
			return vec_myfront3node[0]->getTag();
		}
		else
		{
			if (vec_myback3node.size() > 0)
			{
				std::sort(vec_myback3node.begin(), vec_myback3node.end(), sortbyHp_lesscb);
				return vec_myback3node[0]->getTag();
			}
		}
	}
	return -1;
}

int FightingLayer::calcSkill2AttackNodeIndex(int fighterindex, int type)
{
	if (type == 0)
	{
		int enemycount = m_enemyHeros.size();

		int findex = fighterindex;

		if (findex < 3)
			findex += 3;

		FightHeroNode *enemynode = (FightHeroNode*)this->getChildByTag(6 + findex);
		if (enemynode != NULL && findex < enemycount && m_enemyHeros[findex] != NULL && m_enemyHeros[findex]->getHp() > 0)//正前方后排位置
			return findex;

		enemynode = (FightHeroNode*)this->getChildByTag(6 + findex - 3);
		if (enemynode != NULL && findex - 3 < enemycount && m_enemyHeros[findex - 3] != NULL && m_enemyHeros[findex - 3]->getHp() > 0)//自身位置正前方
			return findex - 3;

		std::vector<FightHeroNode*> vec_tmp;
		for (int i = 0; i < enemycount; i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
			{
				vec_tmp.push_back((FightHeroNode*)this->getChildByTag(6 + i));
			}
		}
		if (vec_tmp.size() > 0)
		{
			int r = GlobalInstance::getInstance()->createRandomNum(vec_tmp.size());
			return vec_tmp[r]->getTag() - 6;
		}

	}
	else
	{
		int findex = fighterindex - 6;

		if (findex < 3)
		{
			findex += 3;
		}

		FightHeroNode* mynode = (FightHeroNode*)this->getChildByTag(findex);
		if (mynode != NULL && m_myHeros[findex] != NULL && m_myHeros[findex]->getState() != HS_DEAD)//自身位置正前方
			return findex;
		mynode = (FightHeroNode*)this->getChildByTag(findex - 3);
		if (mynode != NULL && m_myHeros[findex - 3] != NULL && m_myHeros[findex - 3]->getState() != HS_DEAD)//正前方后排位置
			return findex - 3;

		std::vector<FightHeroNode*> vec_tmp;
		for (unsigned int i = 0; i < m_myHeros.size(); i++)
		{
			if (m_myHeros[i] != NULL && m_myHeros[i]->getState() != HS_DEAD)
			{
				vec_tmp.push_back((FightHeroNode*)this->getChildByTag(i));
			}
		}

		if (vec_tmp.size() > 0)
		{
			int r = GlobalInstance::getInstance()->createRandomNum(vec_tmp.size());
			return vec_tmp[r]->getTag();
		}
	}
	return -1;
}

std::vector<int> FightingLayer::calcSkill5AttackNodeIndex(int fighterindex, int type, int count)
{
	std::vector<int> vec_indexs;
	if (type == 0)
	{
		vec_enemyfront3node.clear();
		vec_enemyback3node.clear();
		int enemycount = m_enemyHeros.size();

		int findex = fighterindex;

		if (findex < 3)
			findex += 3;

		std::vector<FightHeroNode*> vec_tmp;

		FightHeroNode *enemynode = (FightHeroNode*)this->getChildByTag(6 + findex);
		if (enemynode != NULL && findex < enemycount && m_enemyHeros[findex] != NULL && m_enemyHeros[findex]->getHp() > 0)//正前方后排位置
		{
			vec_tmp.push_back(enemynode);
		}

		for (int i = 3; i < enemycount; i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0 && i != findex)
			{
				vec_enemyback3node.push_back((FightHeroNode*)this->getChildByTag(6 + i));
			}
		}

		if (vec_enemyback3node.size() > 0)
		{
			std::random_shuffle(vec_enemyback3node.begin(), vec_enemyback3node.end());
		}

		vec_tmp.insert(vec_tmp.end(), vec_enemyback3node.begin(), vec_enemyback3node.end());

		enemynode = (FightHeroNode*)this->getChildByTag(6 + findex - 3);
		if (enemynode != NULL && findex - 3 < enemycount && m_enemyHeros[findex - 3] != NULL && m_enemyHeros[findex - 3]->getHp() > 0)//自身位置正前方
		{
			vec_tmp.push_back(enemynode);
		}

		for (int i = 0; i < enemycount; i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
			{
				if (i < 3 && i != findex - 3)
					vec_enemyfront3node.push_back((FightHeroNode*)this->getChildByTag(6 + i));
			}
		}
		if (vec_enemyfront3node.size() > 0)
		{
			std::random_shuffle(vec_enemyfront3node.begin(), vec_enemyfront3node.end());
		}

		vec_tmp.insert(vec_tmp.end(), vec_enemyfront3node.begin(), vec_enemyfront3node.end());

		int tmpsize = vec_tmp.size();
		int tcount = tmpsize > count ? count : tmpsize;

		for (int i = 0; i < tcount; i++)
		{
			vec_indexs.push_back(vec_tmp[i]->getTag() - 6);
		}
	}
	else
	{
		vec_myfront3node.clear();
		vec_myback3node.clear();
		int findex = fighterindex - 6;

		if (findex < 3)
		{
			findex += 3;
		}

		std::vector<FightHeroNode*> vec_tmp;

		FightHeroNode* mynode = (FightHeroNode*)this->getChildByTag(findex);
		if (mynode != NULL && m_myHeros[findex] != NULL && m_myHeros[findex]->getState() != HS_DEAD)//正前方后排位置
			vec_tmp.push_back(mynode);

		for (int i = 3; i < 6; i++)
		{
			if (m_myHeros[i] != NULL && m_myHeros[i]->getState() != HS_DEAD && i != findex)
				vec_myback3node.push_back((FightHeroNode*)this->getChildByTag(i));
		}

		if (vec_myback3node.size() > 0)
		{
			std::random_shuffle(vec_myback3node.begin(), vec_myback3node.end());
		}

		vec_tmp.insert(vec_tmp.end(), vec_myback3node.begin(), vec_myback3node.end());

		mynode = (FightHeroNode*)this->getChildByTag(findex - 3);
		if (mynode != NULL && m_myHeros[findex - 3] != NULL && m_myHeros[findex - 3]->getHp() > 0)//自身位置正前方
		{
			vec_tmp.push_back(mynode);
		}

		for (int i = 0; i < 3; i++)
		{
			if (m_myHeros[i] != NULL && m_myHeros[i]->getState() != HS_DEAD && i != findex - 3)
			{
				vec_myfront3node.push_back((FightHeroNode*)this->getChildByTag(i));
			}
		}
		if (vec_myfront3node.size() > 0)
		{
			std::random_shuffle(vec_myfront3node.begin(), vec_myfront3node.end());
		}

		vec_tmp.insert(vec_tmp.end(), vec_myfront3node.begin(), vec_myfront3node.end());

		int tmpsize = vec_tmp.size();
		int tcount = tmpsize > count ? count : tmpsize;

		for (int i = 0; i < tcount; i++)
		{
			vec_indexs.push_back(vec_tmp[i]->getTag());
		}
	}
	return vec_indexs;
}

std::vector<int> FightingLayer::calcSkill6AttackNodeIndex(int fighterindex, int type, int count)
{
	std::vector<int> vec_indexs;
	if (type == 0)
	{
		vec_enemyfront3node.clear();
		vec_enemyback3node.clear();
		int enemycount = m_enemyHeros.size();

		int findex = fighterindex;

		if (findex >= 3)
			findex -= 3;

		std::vector<FightHeroNode*> vec_tmp;
		FightHeroNode* enemynode = (FightHeroNode*)this->getChildByTag(6 + findex);
		if (enemynode != NULL && findex < enemycount && m_enemyHeros[findex] != NULL && m_enemyHeros[findex]->getHp() > 0)//自身位置正前方前排
			vec_tmp.push_back(enemynode);

		for (int i = 0; i < enemycount; i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
			{
				if (i < 3 && i != findex)
					vec_enemyfront3node.push_back((FightHeroNode*)this->getChildByTag(6 + i));
			}
		}
		if (vec_enemyfront3node.size() > 0)
		{
			std::random_shuffle(vec_enemyfront3node.begin(), vec_enemyfront3node.end());
		}

		vec_tmp.insert(vec_tmp.end(), vec_enemyfront3node.begin(), vec_enemyfront3node.end());

		enemynode = (FightHeroNode*)this->getChildByTag(6 + 3 + findex);
		if (enemynode != NULL && findex + 3 < enemycount && m_enemyHeros[findex + 3] != NULL && m_enemyHeros[findex + 3]->getHp() > 0)//正前方后排位置
		{
			vec_tmp.push_back(enemynode);
		}

		for (int i = 3; i < enemycount; i++)
		{
			if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0 && i != findex + 3)
			{
				vec_enemyback3node.push_back((FightHeroNode*)this->getChildByTag(6 + i));
			}
		}

		if (vec_enemyback3node.size() > 0)
		{
			std::random_shuffle(vec_enemyback3node.begin(), vec_enemyback3node.end());
		}
		vec_tmp.insert(vec_tmp.end(), vec_enemyback3node.begin(), vec_enemyback3node.end());

		int tmpsize = vec_tmp.size();
		int tcount = tmpsize > count ? count : tmpsize;

		for (int i = 0; i < tcount; i++)
		{
			vec_indexs.push_back(vec_tmp[i]->getTag() - 6);
		}
	}
	else
	{
		vec_myfront3node.clear();
		vec_myback3node.clear();

		int findex = fighterindex - 6;

		if (findex >= 3)
			findex -= 3;

		std::vector<FightHeroNode*> vec_tmp;

		FightHeroNode* mynode = (FightHeroNode*)this->getChildByTag(findex);
		if (mynode != NULL && m_myHeros[findex] != NULL && m_myHeros[findex]->getState() != HS_DEAD)//自身位置正前方
			vec_tmp.push_back(mynode);

		for (int i = 0; i < 3; i++)
		{
			if (m_myHeros[i] != NULL && m_myHeros[i]->getState() != HS_DEAD && i != findex)
			{
				vec_myfront3node.push_back((FightHeroNode*)this->getChildByTag(i));
			}
		}
		if (vec_myfront3node.size() > 0)
		{
			std::random_shuffle(vec_myfront3node.begin(), vec_myfront3node.end());
		}

		vec_tmp.insert(vec_tmp.end(), vec_myfront3node.begin(), vec_myfront3node.end());

		mynode = (FightHeroNode*)this->getChildByTag(findex + 3);
		if (mynode != NULL && m_myHeros[findex + 3] != NULL && m_myHeros[findex + 3]->getState() != HS_DEAD)//正前方后排位置
			vec_tmp.push_back(mynode);

		for (int i = 3; i < 6; i++)
		{
			if (m_myHeros[i] != NULL && m_myHeros[i]->getState() != HS_DEAD && i != findex + 3)
				vec_myback3node.push_back((FightHeroNode*)this->getChildByTag(i));
		}

		if (vec_myback3node.size() > 0)
		{
			std::random_shuffle(vec_myback3node.begin(), vec_myback3node.end());
		}
		vec_tmp.insert(vec_tmp.end(), vec_myback3node.begin(), vec_myback3node.end());

		int tmpsize = vec_tmp.size();
		int tcount = tmpsize > count ? count : tmpsize;

		for (int i = 0; i < tcount; i++)
		{
			vec_indexs.push_back(vec_tmp[i]->getTag());
		}
	}
	return vec_indexs;
}
