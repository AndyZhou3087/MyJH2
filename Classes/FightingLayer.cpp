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

USING_NS_CC;

FightingLayer::FightingLayer()
{
	fightcount = 0;
}

FightingLayer::~FightingLayer()
{
	for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
	{
		m_enemyHeros[i] = NULL;
	}
	clearSkillsData();
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

	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)m_escapebtn->getChildByName("text");

	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("escapebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	for (unsigned int i = 0; i < enemyHeros.size(); i++)
	{
		if (enemyHeros[i] != NULL)
		{
			FightHeroNode * fightHeroNode = FightHeroNode::create();
			fightHeroNode->setPosition(145 + i % 3 * 215, 835 + i / 3 * 240);
			fightHeroNode->setData(m_enemyHeros[i], F_NPC, FS_FIGHTING);
			addChild(fightHeroNode, 1, 6 + i);
		}
	}

	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			FightHeroNode * fightHeroNode = FightHeroNode::create();
			fightHeroNode->setPosition(145 + i % 3 * 215, 435 - i / 3 * 240);

			fightHeroNode->setData(GlobalInstance::myCardHeros[i], F_HERO, FS_FIGHTING);
			addChild(fightHeroNode, 2, i);
		}
	}

	int r = GlobalInstance::getInstance()->createRandomNum(5);
	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_FIGHT_0 + r);

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
			if (fnode != NULL && fnode->isPlaySkillAnim)
			{
				return;
			}
		}
		for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
			if (fnode != NULL && fnode->isPlaySkillAnim)
			{
				return;
			}
		}
		for (int i = 0; i < 6; i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(i);
			if (fnode != NULL)
			{
				fnode->resumeTimeSchedule();
			}
		}
		for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
		{
			FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
			if (fnode != NULL)
			{
				fnode->resumeTimeSchedule();
			}
		}
	}
	else
	{
		pauseAtkSchedule();
		this->runAction(Sequence::create(DelayTime::create(0.7f), RemoveSelf::create(), NULL));
		if (g_MapBlockScene != NULL)
			g_MapBlockScene->showFightResult(ret);
	}
}

void FightingLayer::clearSkillsData()
{
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
					myheor->clearSkill(gf);
			}
		}
	}
}

void FightingLayer::showAtk(int fightertag)
{
	if (fightertag < 6)//自己英雄攻击
	{
		int enemyindex = -1;//打哪个敌人
		Hero* myhero = GlobalInstance::myCardHeros[fightertag];//攻击的英雄
		if (myhero->getState() != HS_DEAD)//没有死亡可继续战斗
		{
			GongFa* gf = myhero->checkSkillWg();//触发技能的功法
			int stype = -1;
			if (gf != NULL && GlobalInstance::map_GF[gf->getId()].skill != SKILL_13 && GlobalInstance::map_GF[gf->getId()].skill != SKILL_15 && GlobalInstance::map_GF[gf->getId()].skill != SKILL_18 && GlobalInstance::map_GF[gf->getId()].skill != SKILL_20)//没有触发技能
			{
				myhero->vec_whosufferskill.clear();
				stype = GlobalInstance::map_GF[gf->getId()].skill;
				if (myhero->getSkillingType() < 0)//谁发出的技能
					myhero->setSkillingType(stype);
			}

			if (myhero->getSkillingType() >= 0)//触发了技能
			{
				if (stype == SKILL_5 || stype == SKILL_6)//攻击多个目标
				{
					int scount = GlobalInstance::map_GF[gf->getId()].skilleff2;//攻击的几个目标
					int ssize = m_enemyHeros.size();
					int count = 0;
					for (int m = 0; m < ssize; m++)
					{
						if (m_enemyHeros[m] != NULL && m_enemyHeros[m]->getHp() > 0)
						{
							myhero->vec_whosufferskill.push_back(6 + m);
							count++;
							if (count == scount)
								break;
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
					if (vec_myheronode.size() > 0)
					{
						std::sort(vec_myheronode.begin(), vec_myheronode.end(), sortbyHp_lesscb);

						if (stype == SKILL_7)
						{
							myhero->vec_whosufferskill.push_back(vec_myheronode[0]->getTag());
						}
						else
						{
							int ssize = vec_myheronode.size();

							int scount = GlobalInstance::map_GF[gf->getId()].skilleff2;
							int c = ssize < scount ? ssize : scount;
							for (int i = 0; i < c; i++)
							{
								myhero->vec_whosufferskill.push_back(vec_myheronode[i]->getTag());
							}
						}
					}
				}
				else if (stype == SKILL_9 || stype == SKILL_12)
				{
					myhero->vec_whosufferskill.push_back(fightertag);
	
					GongFa* gf = myhero->checkSkillWg();
					if (gf->getSkillCount() < GlobalInstance::map_GF[gf->getId()].skilleff2)
					{
						float myatk = myhero->getAtk();
						if (stype == SKILL_12)
							myatk *= (1 + GlobalInstance::map_GF[gf->getId()].skilleff1/100);
						for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
						{
							if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
							{
								FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
								fnode->hurt(myatk, 0);
								break;
							}
						}
					}
				}
				else if (stype == SKILL_17)
				{
					myhero->vec_whosufferskill.push_back(fightertag);

					GongFa* gf = myhero->checkSkillWg();
					if (gf->getSkillCount() < GlobalInstance::map_GF[gf->getId()].skilleff2)
					{
						float myatk = myhero->getAtk();
						myatk *= (1 + GlobalInstance::map_GF[gf->getId()].skilleff1 / 100);
						for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
						{
							if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
							{
								FightHeroNode* fnode = (FightHeroNode*)this->getChildByTag(6 + i);
								fnode->hurt(myatk, 0);
								break;
							}
						}
					}
				}
				else//攻击单个目标
				{
					for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
					{
						if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
						{
							myhero->vec_whosufferskill.push_back(6 + i);
							break;
						}
					}
				}
			}
			else//没有触发技能
			{
				for (unsigned int i = 0; i < m_enemyHeros.size(); i++)
				{
					if (m_enemyHeros[i] != NULL && m_enemyHeros[i]->getHp() > 0)
					{
						enemyindex = i;//敌人node索引
						break;
					}
				}
			}
			//计算闪避
			if (enemyindex >= 0)//没有触发技能
			{
				FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(6 + enemyindex);
				float dodge = m_enemyHeros[enemyindex]->getDodge();

				int r = GlobalInstance::getInstance()->createRandomNum(10000);

				if (r < dodge * 100)
				{
					enemyfnode->hurt(0, 2);
					SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DODGE);
					return;
				}
			}
			else//触发技能
			{
				if (myhero->getSkillingType() >= 0 && myhero->getSkillingType() != SKILL_7 && myhero->getSkillingType() != SKILL_8 && myhero->getSkillingType() != SKILL_9 && myhero->getSkillingType() != SKILL_12 && myhero->getSkillingType() != SKILL_17)
				{
					bool isdodge = false;
					for (unsigned int i = 0; i < myhero->vec_whosufferskill.size(); i++)
					{
						FightHeroNode* enemyfnode = (FightHeroNode*)this->getChildByTag(myhero->vec_whosufferskill[i]);
						float dodge = m_enemyHeros[myhero->vec_whosufferskill[i] - 6]->getDodge();

						int r = GlobalInstance::getInstance()->createRandomNum(10000);

						if (r < dodge * 100)
						{
							m_enemyHeros[myhero->vec_whosufferskill[i] - 6]->setIsDodge(true);
							isdodge = true;
							enemyfnode->hurt(0, 2);
						}
						else
						{
							m_enemyHeros[myhero->vec_whosufferskill[i] - 6]->setIsDodge(false);
						}
					}
					if (isdodge)
					{
						SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DODGE);
					}
				}
			}

			if (myhero->getSkillingType() >= 0)//触发了技能
			{
				FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(fightertag);

				FightHeroNode* enemynode = (FightHeroNode*)this->getChildByTag(myhero->vec_whosufferskill[0]);

				myfnode->playSkill(myhero->getSkillingType(), enemynode);//技能动画,1对1
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

					SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_CRIT);
				}
				else
				{
					SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_ATK);
				}
				enemyfnode->hurt(atkhp, state);
			}
		}
	}
	else//NPC攻击
	{
		float atkhp = m_enemyHeros[fightertag - 6]->getAtk();
		int myfindex = -1;
		int stype = -1;
		int whoSkill = -1;
		for (unsigned int i = 0; i < 6; i++)
		{
			Hero* myhero = GlobalInstance::myCardHeros[i];
			if (myhero != NULL && myhero->getState() != HS_DEAD && myhero->getSkillingType() >= 0)//释放技能中
			{
				stype = myhero->getSkillingType();
				whoSkill = i;
				break;
			}
		}
		bool isSufferSkill = false;
		//技能是否释放此NPC
		if (stype >= 0 && whoSkill >= 0)
		{
			if (stype == SKILL_4)
			{
				GongFa* gf = GlobalInstance::myCardHeros[whoSkill]->checkSkillWg();
				gf->setSkillCount(gf->getSkillCount() - 1);
				if (gf->getSkillCount() <= 0)
				{
					GlobalInstance::myCardHeros[whoSkill]->clearSkill(gf);
				}
				myfindex = whoSkill;
			}
		}

		if (myfindex < 0)
		{
			for (unsigned int i = 0; i < 6; i++)
			{
				if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
				{
					myfindex = i;
					break;
				}
			}
		}

		if (stype < 0)
		{
			Hero* myhero = GlobalInstance::myCardHeros[myfindex];
			GongFa* gf = myhero->checkSkillWg();
			if (gf != NULL)//
			{
				if (GlobalInstance::map_GF[gf->getId()].skill == SKILL_15)
				{
					stype = SKILL_15;
					GlobalInstance::myCardHeros[myfindex]->vec_whosufferskill.clear();
					GlobalInstance::myCardHeros[myfindex]->vec_whosufferskill.push_back(fightertag);
					FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(myfindex);
					myfnode->hurt(atkhp, -1);
					if (GlobalInstance::myCardHeros[myfindex]->getHp() > 0)
						myfnode->playSkill(stype, (FightHeroNode*)this->getChildByTag(fightertag));
					return;
				}
				else if (GlobalInstance::map_GF[gf->getId()].skill == SKILL_18)
				{
					stype = SKILL_18;
					GlobalInstance::myCardHeros[myfindex]->vec_whosufferskill.clear();
					GlobalInstance::myCardHeros[myfindex]->vec_whosufferskill.push_back(myfindex);
					FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(myfindex);
					myfnode->hurt(atkhp*(1 - GlobalInstance::map_GF[gf->getId()].skilleff1/100), -1);
					if (GlobalInstance::myCardHeros[myfindex]->getHp() > 0)
						myfnode->playSkill(stype, myfnode);
					return;
				}
				else if (GlobalInstance::map_GF[gf->getId()].skill == SKILL_20)
				{
					stype = SKILL_20;
					GlobalInstance::myCardHeros[myfindex]->vec_whosufferskill.clear();
					GlobalInstance::myCardHeros[myfindex]->vec_whosufferskill.push_back(fightertag);
					FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(myfindex);
					myfnode->hurt(atkhp, -1);
					if (GlobalInstance::myCardHeros[myfindex]->getHp() > 0)
						myfnode->playSkill(stype, (FightHeroNode*)this->getChildByTag(fightertag));
					return;
				}
			}

		}

		FightHeroNode* myfnode = (FightHeroNode*)this->getChildByTag(myfindex);


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
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DODGE);
		}
		else
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_HURT);
		myfnode->hurt(atkhp, state);
	}
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