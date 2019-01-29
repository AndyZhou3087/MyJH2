#include "FightHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "Const.h"
#include "FighterAttrLayer.h"
#include "FightingLayer.h"
#include "LoadingBarProgressTimer.h"
#include "GlobalInstance.h"
#include "MapBlockScene.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "SoundManager.h"
#include "SkillStartLayer.h"
#include "AnimationEffect.h"
#include "Shake.h"

FightHeroNode::FightHeroNode()
{
	atkspeed = 0.0f;
	timedt = 0.0f;
	ispause = false;
	hurtup = 0.0f;
	atkspeedbns = 0.0f;
	dfbns = 0.0f;
	isPlaySkillAnim = false;
}


FightHeroNode::~FightHeroNode()
{

}

FightHeroNode* FightHeroNode::create()
{
	FightHeroNode *pRet = new(std::nothrow)FightHeroNode();
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

bool FightHeroNode::init()
{
	if (!Node::init())
		return false;

	csbnode = CSLoader::createNode(ResourcePath::makePath("fightHeroNode.csb"));
	this->addChild(csbnode);

	//头像框
	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	headbox->addTouchEventListener(CC_CALLBACK_2(FightHeroNode::onClick, this));

	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("head");

	statusimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("statusicon");
	statusimg->ignoreContentAdaptWithSize(true);
	statusimg->setVisible(false);
	statusimg->setLocalZOrder(2);

	critnumbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("critnumbg");
	critnumbg->setVisible(false);

	cardnamebox = (cocos2d::ui::Text*)csbnode->getChildByName("cardnamebox");
	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	hpbar_bg = (cocos2d::ui::Widget*)csbnode->getChildByName("herohpbarbg");

	//血量进度条
	hp_bar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("hpbar");

	atkspeed_bar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("atkspeedbar");

	atkspeed_barbg = (cocos2d::ui::Widget*)csbnode->getChildByName("heroatkspeedbarbg");

	atkspeed_bar->setVisible(false);
	atkspeed_barbg->setVisible(false);

	retbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("ret");
	rettext = (cocos2d::ui::ImageView*)retbox->getChildByName("text");
	retbox->setVisible(false);

	winexplbl = (cocos2d::ui::Text*)csbnode->getChildByName("winexp");
	winexplbl->setVisible(false);

	vocationbox = (cocos2d::ui::Widget*)csbnode->getChildByName("vocationbox");

	vocationicon = (cocos2d::ui::ImageView*)vocationbox->getChildByName("v");

	numfnt = (cocos2d::ui::TextBMFont*)csbnode->getChildByName("numfnt");
	numfnt->setVisible(false);

	skilltextbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("skilltextbox");
	skilltextbox->setVisible(false);

	skilltext = (cocos2d::ui::ImageView*)skilltextbox->getChildByName("skilltext");
	lvtext = (cocos2d::ui::Text*)csbnode->getChildByName("lvtext");
	lvtext->setVisible(false);

	return true;
}

void FightHeroNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_Data != NULL)
		{
			if (m_state == FS_READY)
			{
				FighterAttrLayer* layer = FighterAttrLayer::create(m_Data);
				this->getParent()->addChild(layer, 0);
				AnimationEffect::openAniEffect(layer);
				layer->setContentPos(Vec2(360, 1000));
			}
			//else if (m_state == FS_FIGHTING)
			//{
			//	if (m_datatype == F_HERO)
			//		layer->setContentPos(Vec2(360, 270));
			//	else if (m_datatype == F_NPC)
			//		layer->setContentPos(Vec2(360, 900));
			//}
		}
	}
}

void FightHeroNode::setData(Npc* data, FIGHTDATA_TYPE datatype, FIGHTNODE_STATE state)
{
	m_Data = data;
	m_datatype = datatype;
	m_state = state;

	if (data != NULL && ((datatype == F_NPC && data->getHp() > 0) || (datatype == F_HERO && ((Hero*)data)->getState() != HS_DEAD) || state == FS_SUCC || state == FS_FAIL))
	{
		std::string str;
		
		if (datatype == F_HERO)
		{
			int sex = ((Hero*)m_Data)->getSex();
			str = StringUtils::format("cardh_%d_%d.png", data->getVocation(), sex);
			str = ResourcePath::makeImagePath(str);
			headimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);

		}
		else if (datatype == F_NPC)
		{
			str = StringUtils::format("mapui/%s.png", GlobalInstance::map_Npcs[data->getId()].icon.c_str());
			headimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		int v = m_Data->getVocation();
		if (v >= 4)
		{
			str = StringUtils::format("ui/cardvocation%d.png", v);
			vocationicon->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);
			vocationbox->setVisible(true);
		}
		else
			vocationbox->setVisible(false);
		headimg->setVisible(true);

		str = StringUtils::format("cardherobox_%d.png", data->getPotential());
		headbox->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

		namelbl->setString(data->getName());
		namelbl->setVisible(true);
		float percent = data->getHp()*100/ data->getMaxHp();
		hp_bar->setPercent(percent);
		if (state == FS_FIGHTING)
		{
			atkspeed_bar->setVisible(true);
			atkspeed_barbg->setVisible(true);
			atkspeed_bar->setPercent(0);
			atkspeed = 1.0f / data->getAtkSpeed();
			this->scheduleUpdate();
		}
	}
	else
	{
		setBlankBox();
	}
}


Npc* FightHeroNode::getData()
{
	return m_Data;
}

void FightHeroNode::update(float dt)
{
	if (ispause)
		return;

	timedt += dt;

	if (timedt >= getAtkSpeed() && this->isVisible())
	{
		timedt = 0.0f;
		FightingLayer* fighting = (FightingLayer*)this->getParent();
		fighting->pauseAtkSchedule();

		m_Data->setIsRndSkill(false);

		bool isSufferSkill3 = false;

		if (this->getTag() >= 6)
		{
			int stype = -1;
			for (unsigned int i = 0; i < 6; i++)
			{
				FightHeroNode* fnode = (FightHeroNode*)fighting->getChildByTag(i);
				if (fnode != NULL && fnode->getData() != NULL)
				{
					Hero* myhero = (Hero*)fnode->getData();
					if (myhero != NULL && myhero->getState() != HS_DEAD && myhero->getSkillingType() >= 0)//释放技能中
					{
						stype = myhero->getSkillingType();
						for (unsigned int m = 0; m < myhero->vec_whosufferskill.size(); m++)
						{
							if (myhero->vec_whosufferskill[m] == this->getTag() && stype == SKILL_3)//是否释放动弹不得
							{
								isSufferSkill3 = true;
								break;
							}
						}
						if (isSufferSkill3)
						{
							GongFa* gf = (GongFa*)myhero->checkSkillWg();
							gf->setSkillCount(gf->getSkillCount() - 1);
							if (gf->getSkillCount() <= 0)
							{
								myhero->clearSkill(gf);
								this->removeSufferSkillAnim(0);
							}
							nextRound(0);
							break;
						}
					}
				}
			}
		}
		else
		{
			int stype = -1;
			for (unsigned int i = 0; i < 6; i++)
			{
				FightHeroNode* fnode = (FightHeroNode*)fighting->getChildByTag(6 + i);
				if (fnode != NULL && fnode->getData() != NULL && fnode->getData()->getId().length() > 10)
				{
					Hero* myhero = (Hero*)fnode->getData();
					if (myhero != NULL && myhero->getState() != HS_DEAD && myhero->getSkillingType() >= 0)//释放技能中
					{
						stype = myhero->getSkillingType();
						for (unsigned int m = 0; m < myhero->vec_whosufferskill.size(); m++)
						{
							if (myhero->vec_whosufferskill[m] == this->getTag() && stype == SKILL_3)//是否释放动弹不得
							{
								isSufferSkill3 = true;
								break;
							}
						}
						if (isSufferSkill3)
						{
							GongFa* gf = (GongFa*)myhero->checkSkillWg();
							gf->setSkillCount(gf->getSkillCount() - 1);
							if (gf->getSkillCount() <= 0)
							{
								myhero->clearSkill(gf);
								this->removeSufferSkillAnim(0);
							}
							nextRound(0);
							break;
						}
					}
				}
			}
		}
		
		if (!isSufferSkill3)
		{
			GongFa* gf = m_Data->checkSkillWg();
			if (gf == NULL || (gf != NULL && (GlobalInstance::map_GF[gf->getId()].skill == SKILL_13 || GlobalInstance::map_GF[gf->getId()].skill == SKILL_15 || GlobalInstance::map_GF[gf->getId()].skill == SKILL_18 || GlobalInstance::map_GF[gf->getId()].skill == SKILL_20)))
			{
				showAtkOrHurtAnim(0);
				this->runAction(Sequence::create(ScaleTo::create(0.22f, 1.2f), ScaleTo::create(0.08f, 1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::atkAnimFinish, this)), DelayTime::create(0.6f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::nextRound, this, 0)), NULL));
			
			}
			else
			{
				if (this->isVisible())
					this->runAction(Sequence::create(ScaleTo::create(0.22f, 1.2f), ScaleTo::create(0.08f, 1.0f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::atkAnimFinish, this)), NULL));
				else
				{
					isPlaySkillAnim = false;
					nextRound(0);
				}
			}
		}
	}
	atkspeed_bar->setPercent(timedt * 100 / getAtkSpeed());
}

float FightHeroNode::getAtkSpeed()
{
	float s = 1.0f / atkspeed;
	s = (1 + atkspeedbns / 100)*s;
	s = 1.0f / s;
	return s;
}

void FightHeroNode::pauseTimeSchedule()
{
	if (m_Data != NULL)
	{
		ispause = true;
	}
}

void FightHeroNode::resumeTimeSchedule()
{
	if (m_Data != NULL)
	{
		ispause = false;
	}
}


void FightHeroNode::hurt(float hp, int stat)//stat -1:不显示普攻动画
{
	if (m_Data != NULL && this->isVisible())
	{
		if (hp < m_Data->getDf())
			hp -= 0.1 * hp;
		else
			hp -= m_Data->getDf()*(1 + dfbns/100);

		if (hp > 0 || stat == 2)
		{
			hurtup = hp;

			std::string hurtstr = StringUtils::format("-%d", (int)hurtup);
			if (stat == 1)//暴击
			{
				numfnt->setFntFile("fonts/crithurtnum.fnt");
				int langtype = GlobalInstance::getInstance()->getLang();
				statusimg->loadTexture(ResourcePath::makeTextImgPath("crit_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
				numfnt->setScale(5);
				numfnt->runAction(Sequence::create(Show::create(), EaseRateAction::create(ScaleTo::create(0.15f, 0.6f), 5), EaseRateAction::create(ScaleTo::create(0.1f, 1),5), DelayTime::create(0.15f), Hide::create(), NULL));
				critnumbg->runAction(Sequence::create(Show::create(), EaseRateAction::create(ScaleTo::create(0.15f, 0.6f), 5), EaseRateAction::create(ScaleTo::create(0.1f, 1),5), DelayTime::create(0.15f), Hide::create(), NULL));
				showAtkOrHurtAnim(1);
			}
			else if (stat == 2)//闪避
			{
				hurtup = 0;
				int langtype = GlobalInstance::getInstance()->getLang();
				statusimg->loadTexture(ResourcePath::makeTextImgPath("dodge_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
			else//普通攻击
			{
				numfnt->setFntFile("fonts/normalhurtnum.fnt");
				statusimg->loadTexture("ui/blank.png", cocos2d::ui::Widget::TextureResType::PLIST);
				numfnt->runAction(Sequence::create(Show::create(), MoveBy::create(0.3f, Vec2(0, 10)), DelayTime::create(0.1f), Hide::create(), MoveBy::create(0.02f, Vec2(0, -10)), NULL));
				if (stat == 0)
					showAtkOrHurtAnim(1);
			}
			if (hurtup > 0)
			{
				numfnt->setString(hurtstr);
			}
			ActionInterval* ac1 = Spawn::create(Show::create(), FadeIn::create(0.1f), EaseSineIn::create(ScaleTo::create(0.15f, 1)), NULL);
			statusimg->runAction(Sequence::create(ac1, CallFunc::create(CC_CALLBACK_0(FightHeroNode::hpAnim, this)), DelayTime::create(0.2f), Hide::create(), NULL));
		
		}
		else
		{
			hurtAnimFinish();
		}
	}
} 

void FightHeroNode::atkAnimFinish()
{
	if (this->isVisible())
	{
		FightingLayer* fighting = (FightingLayer*)this->getParent();
		fighting->showAtk(this->getTag());
	}
	else
	{
		isPlaySkillAnim = false;
		nextRound(0);
	}
}

void FightHeroNode::hpAnim()
{
	float lefthp = m_Data->getHp() - hurtup;
	if (lefthp < 0)
		lefthp = 0;
	m_Data->setHp(lefthp);
	float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
	hp_bar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), CallFunc::create(CC_CALLBACK_0(FightHeroNode::hurtAnimFinish, this)), NULL));
}

void FightHeroNode::hurtAnimFinish()
{
	FightingLayer* fighting = (FightingLayer*)this->getParent();
	updateHp();

	if (m_datatype == F_HERO)
	{
		if (m_Data->getHp() <= 0)
		{
			removePlaySkillAnim(0);

			GongFa* gf = m_Data->checkSkillWg();
			if (gf != NULL && GlobalInstance::map_GF[gf->getId()].skill != SKILL_13)
				m_Data->clearSkill(gf);

			if (checkReviveSkill())
			{
				fighting->pauseAtkSchedule();
				isPlaySkillAnim = true;
				this->scheduleOnce(schedule_selector(FightHeroNode::delayShowReviveAnim), 0.6f);
				return;
			}
			else
			{
				pauseTimeSchedule();
				isPlaySkillAnim = false;
				((Hero*)m_Data)->setState(HS_DEAD);

				((Hero*)m_Data)->setPos(0);

				int v = ((Hero*)m_Data)->getVocation();

				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_DIE);
				//nextRound(0);
			}
		}

		fighting->updateMapHero(this->getTag());
	}
	else
	{
		if (m_Data->getHp() <= 0)
		{
			bool isskillfinish = false;
			for (int i = 0; i < 6; i++)
			{
				FightHeroNode* fnode = (FightHeroNode*)fighting->getChildByTag(i);
				if (fnode != NULL && fnode->getData() != NULL)
				{
					Hero* myhero = (Hero*)fnode->getData();
					if (myhero != NULL)
					{
						int count = 0;
						for (unsigned int n = 0; n < myhero->vec_whosufferskill.size(); n++)
						{
							if (myhero->vec_whosufferskill[n] == this->getTag())
							{
								isskillfinish = true;
								GongFa* gf = myhero->checkSkillWg();
								if(gf!= NULL)
									myhero->clearSkill(gf);
								return;
							}
						}
					}
				}
			}
		}
	}
}

void FightHeroNode::delayShowReviveAnim(float dt)
{
	this->setVisible(true);

	playSkill(SKILL_13, NULL);
	attackedSkill(SKILL_13, this->getTag());

}

bool FightHeroNode::checkReviveSkill()
{
	if (m_Data->getId().length() > 10)
	{
		Hero* hero = (Hero*)m_Data;
		GongFa* gf = (GongFa*)hero->getEquipable(T_NG);
		if (gf != NULL)
		{
			if (GlobalInstance::map_GF[gf->getId()].skill == SKILL_13 && GlobalInstance::map_GF[gf->getId()].vec_skillbns[hero->getVocation()] == 1)
			{
				if (gf->getSkillCount() <= 1)
				{
					gf->setSkillCount(2);
					return true;
				}
			}

		}
	}
	return false;
}

void FightHeroNode::reviveOnce(float hp)
{
	m_Data->setHp(hp);
	this->runAction(Sequence::create(Show::create(), FadeIn::create(0.3f), NULL));
	updateHp();

	FightingLayer* fighting = (FightingLayer*)this->getParent();
	fighting->updateMapHero(this->getTag());
}

void FightHeroNode::updateHp()
{
	if (m_Data->getHp() <= 0)
	{
		m_Data->setHp(0.0f);
		setBlankBox();
	}
	else if (m_Data->getHp() > m_Data->getMaxHp())
	{
		m_Data->setHp(m_Data->getMaxHp());
	}
	float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
	hp_bar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), NULL));
}

void FightHeroNode::setBlankBox()
{
	vocationbox->setVisible(false);
	if (m_state == FS_READY)
	{
		headbox->loadTexture(ResourcePath::makeImagePath("cardherobox_.png"), cocos2d::ui::Widget::TextureResType::LOCAL);
		headimg->setVisible(false);
		namelbl->setVisible(false);
		hp_bar->setVisible(false);
		atkspeed_bar->setVisible(false);
		atkspeed_barbg->setVisible(false);
	}
	else if (m_state == FS_FIGHTING)
	{
		showDeathAnim();
	}
	else if (m_state == FS_SUCC || m_state == FS_FAIL)
	{
		if (m_Data == NULL)
			this->setVisible(false);
	}
}

void FightHeroNode::setFightState(int winexp)
{
	int langtype = GlobalInstance::getInstance()->getLang();
	Hero* myhero = (Hero*)m_Data;

	int mylastlv = myhero->getLevel();

	int curlvexp = 0;
	int nextlvexp = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[myhero->getLevel()];

	if (myhero->getLevel() > 0)
		curlvexp = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[myhero->getLevel() - 1];

	int curpercent = (myhero->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);

	hp_bar->loadTexture("mapui/winexpbar.png", cocos2d::ui::Widget::TextureResType::PLIST);
	
	hp_bar->setPercent(curpercent);

	if (winexp > 0 && myhero->getState() != HS_DEAD)
	{
		std::string str;
		if ((myhero->getLevel() + 1) / 10 == myhero->getChangeCount() + 1)
		{
			str = ResourceLang::map_lang["changebreak"];
		}
		else
		{
			int maxlv = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp.size();
			DynamicValueInt dv;
			dv.setValue(myhero->getExp().getValue() + winexp);

			if (myhero->getExp().getValue() >= GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[maxlv - 1])
			{
				dv.setValue(GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[maxlv - 1]);
			}
			myhero->setExp(dv);

			str = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), winexp);
		}
		winexplbl->setString(str);
		winexplbl->setVisible(true);
		FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
		FiniteTimeAction* moveandout = Spawn::create(MoveBy::create(1.5f, Vec2(0, 10)), NULL);
		winexplbl->runAction(Sequence::create(DelayTime::create(0.5f), scales, moveandout, DelayTime::create(1.0f), FadeOut::create(1.0f), NULL));
	}

	curlvexp = 0;
	nextlvexp = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[myhero->getLevel()];

	if (myhero->getLevel() > 0)
		curlvexp = GlobalInstance::vec_herosAttr[myhero->getVocation()].vec_exp[myhero->getLevel() - 1];

	float percent = (myhero->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);
			
	if (myhero->getState() != HS_DEAD)
	{
		if (myhero->getLevel() > mylastlv)//升级
		{
			retbox->setVisible(true);

			rettext->loadTexture(ResourcePath::makeTextImgPath("winlvup_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

			FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
			FiniteTimeAction* moveandout = Spawn::create(EaseSineOut::create(MoveBy::create(1.2f, Vec2(0, 40))), NULL);
			retbox->runAction(Sequence::create(DelayTime::create(0.5f), scales, moveandout, DelayTime::create(1.0f), FadeOut::create(1.0f), NULL));
			myhero->setHp(myhero->getMaxHp());
			if (g_MapBlockScene != NULL)
				g_MapBlockScene->updateHeroUI(this->getTag());

			hp_bar->runAction(Sequence::create(DelayTime::create(0.5f), LoadingBarProgressTo::create(0.2f, 100), DelayTime::create(0.1f), LoadingBarProgressFromTo::create(0.2f, 0, percent), NULL));
		}
		else
		{
			hp_bar->runAction(Sequence::create(DelayTime::create(0.5f), LoadingBarProgressTo::create(0.2f, percent), NULL));
		}
	}
	else
	{
		myhero->setOnchallengepos(0);
		hp_bar->setPercent(percent);
		rettext->loadTexture(ResourcePath::makeTextImgPath("windeath_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		retbox->setVisible(true);
		retbox->setPositionY(retbox->getPositionY() + 40);
		retbox->setScale(3);

		retbox->runAction(Speed::create(ScaleTo::create(1.2f, 1.0f), 5));

		CommonFuncs::changeGray(headbox);
		CommonFuncs::changeGray(headimg);
		CommonFuncs::changeGray(hp_bar->getVirtualRenderer());
		CommonFuncs::changeGray(retbox);
		if (g_MapBlockScene != NULL)
			GlobalInstance::myCardHeros[this->getTag()] = NULL;
	}

	GlobalInstance::getInstance()->saveHero(myhero);

	std::string lvstr = StringUtils::format("Lv.%d", myhero->getLevel() + 1);
	lvtext->setString(lvstr);

	lvtext->runAction(Sequence::create(DelayTime::create(4.5f), Show::create(), FadeIn::create(0.5f), NULL));
}

void FightHeroNode::setMatchFightState()
{
	hp_bar->setVisible(false);
	hpbar_bg->setVisible(false);
	Hero* myhero = (Hero*)m_Data;
	if (myhero->getState() == HS_DEAD)
	{
		int langtype = GlobalInstance::getInstance()->getLang();
		rettext->loadTexture(ResourcePath::makeTextImgPath("windeath_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		retbox->setVisible(true);
		retbox->setPositionY(retbox->getPositionY() + 40);
		retbox->setScale(3);

		retbox->runAction(Speed::create(ScaleTo::create(1.2f, 1.0f), 5));

		CommonFuncs::changeGray(headbox);
		CommonFuncs::changeGray(headimg);
		CommonFuncs::changeGray(hp_bar->getVirtualRenderer());
		CommonFuncs::changeGray(retbox);
	}
	cardnamebox->setPositionY(-90);
	namelbl->setPositionY(cardnamebox->getPositionY());
}

void FightHeroNode::playSkill(int stype, FightHeroNode* whosufferNode)
{
	FightingLayer* fighting = (FightingLayer*)this->getParent();
	fighting->pauseAtkSchedule();

	GongFa* gf = m_Data->checkSkillWg();

	if (gf == NULL)
		return;

	if (m_Data->getId().length() > 10)//是否是自己的英雄
	{
		if (gf->getSkillCount() <= 0)
		{
			gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
		}

		if (gf->getSkillCount() == GlobalInstance::map_GF[gf->getId()].skilleff2)
		{
			isPlaySkillAnim = true;
		}

		if (stype == SKILL_5 || stype == SKILL_6 || stype == SKILL_8)
		{
			gf->setSkillCount(m_Data->vec_whosufferskill.size());
			isPlaySkillAnim = true;
		}

		float delay = 0.0f;
		if (stype == 1)
		{
			delay = 1.0f;
		}

		if (isPlaySkillAnim)
		{
			if (stype >= 9)
			{
				std::string textstr = StringUtils::format("bufpreskill%dtext", stype);
				skilltext->loadTexture(ResourcePath::makeTextImgPath(textstr, GlobalInstance::getInstance()->getLang()), cocos2d::ui::Widget::TextureResType::PLIST);
				skilltextbox->setScale(0);
				skilltextbox->setVisible(true);
				skilltextbox->runAction(Sequence::create(Spawn::create(EaseRateAction::create(ScaleTo::create(0.3f, 1.2f), 10), EaseRateAction::create(ScaleTo::create(0.2f, 1), 10), FadeIn::create(0.2f), NULL), DelayTime::create(0.3f), Hide::create(), NULL));
				delay += 0.6f;
			}
			else
			{
				if (this->getParent()->getChildByName("skillstart") == NULL)
				{
					if (this->getTag() < 6)
					{
						SkillStartLayer* layer = SkillStartLayer::create(m_Data->getVocation(), stype);
						this->getParent()->addChild(layer, 100, "skillstart");
						delay += 1.0f;
					}
				}
			}

			if (stype == SKILL_5)
			{
				for (unsigned int i = 0; i < m_Data->vec_whosufferskill.size(); i++)
					headbox->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(CC_CALLBACK_0(FightHeroNode::playMoreSkillEffectCB, this, stype, m_Data->vec_whosufferskill[i])), NULL));
			}
			else
			{
				if (stype != SKILL_7 && stype != SKILL_8 && stype != SKILL_9 && stype != SKILL_12 && stype != 13 && stype != SKILL_17 && stype != SKILL_18)
				{
					FightingLayer* fighting = (FightingLayer*)this->getParent();
					headbox->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(CC_CALLBACK_0(FightHeroNode::playSkillEffect, this, stype)), NULL));
				}
			}
			if (stype == SKILL_9)
			{
				Sprite* s = Sprite::createWithSpriteFrameName("mapui/attricon2up.png");
				s->setVisible(false);
				this->addChild(s);
				map_skillattricon[stype].push_back(s);

				Label *lbl = Label::createWithTTF(ResourceLang::map_lang["skillattratkspeedup"], FONT_NAME, 23);
				lbl->setColor(Color3B(36, 255, 82));
				lbl->setVisible(false);
				lbl->setPosition(Vec2(0, 110));
				lbl->enableOutline(Color4B(67, 105, 44, 255), 2);
				this->addChild(lbl, 20);
				map_skillattrlabel[stype].push_back(lbl);
			}
			else if (stype == SKILL_12)
			{
				Sprite* s = Sprite::createWithSpriteFrameName("mapui/attricon1up.png");
				this->addChild(s);
				s->setVisible(false);
				map_skillattricon[stype].push_back(s);

				Label *lbl = Label::createWithTTF(ResourceLang::map_lang["skillattratkup"], FONT_NAME, 23);
				lbl->setColor(Color3B(36, 255, 82));
				lbl->setVisible(false);
				lbl->setPosition(Vec2(0, 110));
				lbl->enableOutline(Color4B(67, 105, 44, 255), 2);
				this->addChild(lbl, 20);
				map_skillattrlabel[stype].push_back(lbl);

			}
			else if (stype == SKILL_17)
			{
				Sprite* s = Sprite::createWithSpriteFrameName("mapui/attricon1up.png");
				this->addChild(s);
				s->setVisible(false);
				map_skillattricon[stype].push_back(s);
				Sprite* s1 = Sprite::createWithSpriteFrameName("mapui/attricon0down.png");
				this->addChild(s1);
				s1->setVisible(false);
				map_skillattricon[stype].push_back(s1);

				Label *lbl = Label::createWithTTF(ResourceLang::map_lang["skillattratkup"], FONT_NAME, 23);
				lbl->setColor(Color3B(36, 255, 82));
				lbl->setVisible(false);
				lbl->setPosition(Vec2(0, 110));
				lbl->enableOutline(Color4B(67, 105, 44, 255), 2);
				this->addChild(lbl, 20);
				map_skillattrlabel[stype].push_back(lbl);

				Label *lbl1 = Label::createWithTTF(ResourceLang::map_lang["skillattrdfdwon"], FONT_NAME, 23);
				lbl1->setColor(Color3B(255, 61, 61));
				lbl1->setVisible(false);
				lbl1->setPosition(Vec2(0, 110));
				lbl1->enableOutline(Color4B(90, 6, 6, 255), 2);
				this->addChild(lbl1, 20);
				map_skillattrlabel[stype].push_back(lbl1);

			}
			else if (stype == SKILL_18)
			{
				Sprite* s = Sprite::createWithSpriteFrameName("mapui/attricon0up.png");
				this->addChild(s);
				s->setVisible(false);
				map_skillattricon[stype].push_back(s);

				Label *lbl = Label::createWithTTF(ResourceLang::map_lang["skillattrdfup"], FONT_NAME, 23);
				lbl->setColor(Color3B(36, 255, 82));
				lbl->setVisible(false);
				lbl->setPosition(Vec2(0, 110));
				lbl->enableOutline(Color4B(67, 105, 44, 255), 2);
				this->addChild(lbl, 20);
				map_skillattrlabel[stype].push_back(lbl);

			}
			if (map_skillattricon.size() > 0)
			{
				this->scheduleOnce(schedule_selector(FightHeroNode::showSkillAttrIcon), 0.1f);
			}
			log("zhou = %d", m_Data->vec_whosufferskill.size());
			for (unsigned int i = 0; i < m_Data->vec_whosufferskill.size(); i++)
			{
				FightHeroNode* fnode = (FightHeroNode*)this->getParent()->getChildByTag(m_Data->vec_whosufferskill[i]);
				fnode->attackedSkill(stype, this->getTag());

				if (!fnode->getData()->getIsDodge())
				{
					if (stype == SKILL_10)
					{
						Sprite* s = Sprite::createWithSpriteFrameName("mapui/attricon2down.png");
						fnode->addChild(s);
						s->setVisible(false);
						fnode->map_skillattricon[stype].push_back(s);

						Label *lbl = Label::createWithTTF(ResourceLang::map_lang["skillattratkspeeddwon"], FONT_NAME, 23);
						lbl->setColor(Color3B(255, 61, 61));
						lbl->setVisible(false);
						lbl->setPosition(Vec2(0, 110));
						lbl->enableOutline(Color4B(90, 6, 6, 255), 2);
						fnode->addChild(lbl, 20);
						fnode->map_skillattrlabel[stype].push_back(lbl);

					}
					else if (stype == SKILL_11)
					{
						Sprite* s = Sprite::createWithSpriteFrameName("mapui/attricon0down.png");
						fnode->addChild(s);
						s->setVisible(false);
						fnode->map_skillattricon[stype].push_back(s);

						Label *lbl = Label::createWithTTF(ResourceLang::map_lang["skillattrdfdwon"], FONT_NAME, 23);
						lbl->setColor(Color3B(255, 61, 61));
						lbl->setVisible(false);
						lbl->setPosition(Vec2(0, 110));
						lbl->enableOutline(Color4B(90, 6, 6, 255), 2);
						fnode->addChild(lbl, 20);
						fnode->map_skillattrlabel[stype].push_back(lbl);
					}
					if (fnode->map_skillattricon[stype].size() > 0)
					{
						fnode->scheduleOnce(schedule_selector(FightHeroNode::showSkillAttrIcon), 0.1f);
					}
				}
			}
		}
		else
		{
			if (stype != SKILL_4 && stype != SKILL_7 && stype != SKILL_8 && stype != SKILL_9 && stype != SKILL_12 && stype != 13 && stype != SKILL_17 && stype != SKILL_18)
				whosufferNode->attackShakeAnim();
			changeSkillValue(stype, whosufferNode);
		}
	}
}

void FightHeroNode::changeSkillValue(int stype, FightHeroNode* whosufferNode)
{
	isPlaySkillAnim = false;
	GongFa* gf = m_Data->checkSkillWg();

	float dt = 0.0f;

	if (gf != NULL)
	{
		float eff = GlobalInstance::map_GF[gf->getId()].skilleff1;

		if (!whosufferNode->getData()->getIsDodge())
		{
			if (stype == SKILL_1)//释放技能后吸收对方%.2f血量。
			{
				m_Data->setHp(m_Data->getHp() + eff*whosufferNode->getData()->getMaxHp() / 100);

				float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
				this->updateHp();

				//whosufferNode->getData()->setHp(whosufferNode->getData()->getHp() - eff*whosufferNode->getData()->getMaxHp() / 100);
				//whosufferNode->updateHp();
				whosufferNode->hurt(eff*whosufferNode->getData()->getMaxHp() / 100, -1);
			}
			else if (stype == SKILL_2)//释放技能后造成%d倍伤害。
			{
				//whosufferNode->getData()->setHp(whosufferNode->getData()->getHp() - eff*whosufferNode->getData()->getMaxHp() / 100);
				//whosufferNode->updateHp();
				whosufferNode->hurt(eff*m_Data->getAtk() / 100, -1);
			}
			else if (stype == SKILL_3)//被攻击目标%d回合内无法进行攻击。
			{
				if (gf->getSkillCount() <= GlobalInstance::map_GF[gf->getId()].skilleff2 - 1)
				{
					dt = 0.45f;
					whosufferNode->hurt(m_Data->getAtk(), 0);
				}
			}
			else if (stype == SKILL_4)//释放技能后所有敌人攻击你%d回合。
			{
				if (gf->getSkillCount() <= GlobalInstance::map_GF[gf->getId()].skilleff2 - 1)
				{
					dt = 0.45f;
					whosufferNode->hurt(m_Data->getAtk());
				}
			}
			else if (stype == SKILL_5 || stype == SKILL_6)//目标造成%.2f伤害，%d个目标。
			{
				//whosufferNode->getData()->setHp(whosufferNode->getData()->getHp() - );
				//whosufferNode->updateHp();
				whosufferNode->hurt(eff*m_Data->getAtk()/100, -1);
			}
			else if (stype == SKILL_7 || stype == SKILL_8)//回血
			{
				whosufferNode->getData()->setHp(whosufferNode->getData()->getHp() + eff*whosufferNode->getData()->getMaxHp() / 100);
				whosufferNode->updateHp();
			}
			else if (stype == SKILL_9)
			{
				if (gf->getSkillCount() >= GlobalInstance::map_GF[gf->getId()].skilleff2)
				{
					whosufferNode->atkspeedbns = GlobalInstance::map_GF[gf->getId()].skilleff1;
				}
				else
				{
					dt = 0.45f;
				}
			}
			else if (stype == SKILL_10)
			{
				if (gf->getSkillCount() >= GlobalInstance::map_GF[gf->getId()].skilleff2)
				{
					whosufferNode->atkspeedbns = -GlobalInstance::map_GF[gf->getId()].skilleff1;
				}
				else
				{
					whosufferNode->hurt(m_Data->getAtk());
					dt = 0.45f;
				}
			}
			else if (stype == SKILL_12)
			{
				if (gf->getSkillCount() >= GlobalInstance::map_GF[gf->getId()].skilleff2)
				{
				}
				else
				{
					dt = 0.45f;
				}
			}
			else if (stype == SKILL_17)
			{
				if (gf->getSkillCount() >= GlobalInstance::map_GF[gf->getId()].skilleff2)
				{

				}
				else
				{
					whosufferNode->dfbns = GlobalInstance::map_GF[gf->getId()].skilleff1;
					dt = 0.45f;
				}
			}

			else if (stype == SKILL_13)
			{
				dt = 0.2f;
				reviveOnce(m_Data->getMaxHp()*GlobalInstance::map_GF[gf->getId()].skilleff1 / 100);
			}
			else if (stype == SKILL_15)
			{
				whosufferNode->hurt(m_Data->getAtk() * eff / 100, -1);
			}

			else if (stype == SKILL_20)
			{
				for (int i = 0; i < 3; i++)
				{
					whosufferNode->runAction(Sequence::create(DelayTime::create(i*0.45f), CallFunc::create(CC_CALLBACK_0(FightHeroNode::hurt, whosufferNode, m_Data->getAtk() * eff / 100, -1)), NULL));
				}
				dt = 1.0f;
			}
		}
		else
		{
			whosufferNode->hurt(0, 2);
		}

		if (stype == SKILL_3 || stype == SKILL_4)
		{
			if (gf->getSkillCount() == GlobalInstance::map_GF[gf->getId()].skilleff2)
				gf->setSkillCount(gf->getSkillCount() - 1);
		}
		else if (stype == SKILL_13)
		{

		}
		else
		{
			gf->setSkillCount(gf->getSkillCount() - 1);
			if (gf->getSkillCount() <= 0)
			{
				m_Data->clearSkill(gf);
				whosufferNode->getData()->setIsDodge(false);
				if (stype == 9)
				{
					whosufferNode->atkspeedbns = 0;
				}
				else if (stype == 17)
				{
					whosufferNode->dfbns = 0.0f;
				}
				//清除技能属性图标
				if (stype == SKILL_9 || stype == SKILL_12 || stype == SKILL_17 || stype == SKILL_18)
				{
					this->refreshSkillAttrIcon(stype);
				}
				else if (stype == SKILL_10 || stype == SKILL_11)
				{
					whosufferNode->refreshSkillAttrIcon(stype);
				}
			}
		}
	}

	this->scheduleOnce(schedule_selector(FightHeroNode::nextRound), dt);

	//else if (stype == SKILL_3)//被攻击目标%d回合内无法进行攻击。
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_4)//释放技能后所有敌人攻击你%d回合。
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_9)//增加自身攻击速度%.2f。持续%d回合
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		atkspeedbns = GlobalInstance::map_GF[gf->getId()].skilleff1;
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_10)//降低对方攻击速度%.2f。持续%d回合
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_11)// 降低对方防御%.2f，持续%d回合。
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_12)//触发时提升自身攻击%.2f，持续%d回合。
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_13)//死亡后复活一次。复活后生命值为%.2f。
	//{
	//	isPlayAnim = true;
	//}
	//else if (stype == SKILL_14)//触发时减少%.2f伤害，持续%d回合。
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_15)
	//{
	//	isPlayAnim = true;
	//}
	//else if (stype == SKILL_16)
	//{
	//	dfbns = GlobalInstance::map_GF[gf->getId()].skilleff1;
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_17)
	//{
	//	dfbns = -GlobalInstance::map_GF[gf->getId()].skilleff1;
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_18)//
	//{
	//	if (gf->getSkillCount() <= 0)
	//	{
	//		gf->setSkillCount(GlobalInstance::map_GF[gf->getId()].skilleff2);
	//		isPlayAnim = true;
	//	}
	//}
	//else if (stype == SKILL_19)//
	//{
	//	isPlayAnim = true;
	//}
	//else if (stype == SKILL_20)//
	//{
	//	isPlayAnim = true;
	//}
	//else
	//{
	//	isPlayAnim = true;
	//}
}

void FightHeroNode::attackedSkill(int stype, int myHeroPos)
{
	float dt1 = 1.0f;
	float dt2 = 1.0f;
	if (stype == 1)
	{
		dt1 = 0;
		dt2 = 0.8f;
	}
	else if (stype == 2)
	{
		dt1 = 0.3f;
		dt2 = 1.0f;
	}
	else if (stype == 3)
	{
		dt1 = 0.6f;
		dt2 = 1.0f;
	}
	else if (stype == 4)
	{
		dt1 = 0.0f;
		dt2 = 1.0f;
	}
	else if (stype == 5)
	{
		dt1 = 0.2f;
		dt2 = 1.0f;
	}
	else if (stype == 6)
	{
		dt1 = 0.8f;
		dt2 = 1.2f;
	}
	else if (stype == 7 || stype == 8)
	{
		dt1 = 0.0f;
		dt2 = 1.5f;
	}
	else if (stype == 9)
	{
		dt1 = -0.4f;
		dt2 = 0.8f;
	}
	else if (stype == 10)
	{
		dt1 = 0.8f;
		dt2 = 1.5f;
	}
	else if (stype == 12)
	{
		dt1 = -0.4f;
		dt2 = 1.0f;
	}
	else if (stype == 13)
	{
		dt1 = -0.4f;
		dt2 = 1.5f;
	}
	else if (stype == 15)
	{
		dt1 = 0.8f;
		dt2 = 1.2f;
	}
	else if (stype == 17)
	{
		dt1 = -0.4f;
		dt2 = 0.5f;
	}
	else if (stype == 18)
	{
		dt1 = -0.4f;
		dt2 = 1.2f;
	}
	else if (stype == 20)
	{
		dt1 = 0.0f;
		dt2 = 0.1f;
	}
	float skillstarttime = 1.0f;
	if (myHeroPos >= 6)//对手攻击，不会有技能说明动画
	{
		skillstarttime = 0.05f;
	}
	if (stype != 4)
	{
		if (!m_Data->getIsDodge())
			headimg->runAction(Sequence::create(DelayTime::create(dt1 + skillstarttime), CallFunc::create(CC_CALLBACK_0(FightHeroNode::attackedSkillEffect, this, stype, myHeroPos)), NULL));
	}

	if (stype != SKILL_4 && stype != SKILL_7 && stype != SKILL_8 && stype != SKILL_9 && stype != SKILL_12 && stype != 13 && stype != SKILL_17 && stype != SKILL_18)
	{
		float dtime = skillstarttime - 0.3f;
		if (dtime < 0)
			dtime = 0.05f;
		hpbar_bg->runAction(Sequence::create(DelayTime::create(dt2 + dtime), CallFunc::create(CC_CALLBACK_0(FightHeroNode::attackShakeAnim, this)), NULL));
	}

	namelbl->runAction(Sequence::create(DelayTime::create(dt2 + skillstarttime), CallFunc::create(CC_CALLBACK_0(FightHeroNode::attackedSkillCB, this, stype, myHeroPos)), NULL));
}

void FightHeroNode::attackedSkillCB(int stype, int myHeroPos)
{
	FightHeroNode* fnode = (FightHeroNode*)this->getParent()->getChildByTag(myHeroPos);
	fnode->changeSkillValue(stype, this);
}

void FightHeroNode::recoveHp(float hp)
{
	float eff = hp;
	m_Data->setHp(m_Data->getHp() + eff);
	if (m_Data->getHp() + eff > m_Data->getMaxHp())
		m_Data->setHp(m_Data->getMaxHp());

	float percent = m_Data->getHp() * 100 / m_Data->getMaxHp();
	hp_bar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), NULL));

	std::string hpstr = StringUtils::format("+%d", (int)eff);
	numfnt->setString(hpstr);
	numfnt->setFntFile("fonts/addhpnum.fnt");
	numfnt->runAction(Sequence::create(Show::create(), DelayTime::create(0.35f), Hide::create(), NULL));
}

void FightHeroNode::nextRound(float dt)
{
	FightingLayer* fighting = (FightingLayer*)this->getParent();
	fighting->resumeAtkSchedule();
}

void FightHeroNode::attackShakeAnim()
{
	this->runAction(Shake::create(0.2f, 5.0f));
}

void FightHeroNode::playSkillEffect(int stype)
{
	this->setLocalZOrder(12);

	std::string effectname = StringUtils::format("effect/skill_%d_0.csb", stype);
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	csbnode->addChild(effectnode, 3, "playskillani");

	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);

	this->scheduleOnce(schedule_selector(FightHeroNode::removePlaySkillAnim), action->getDuration() / 60.0f);
}

void FightHeroNode::playMoreSkillEffectCB(int stype, int enemyindex)
{
	this->setLocalZOrder(12);
	std::string effectname = StringUtils::format("effect/skill_%d_0.csb", stype);

	if (enemyindex < 6)
		effectname = StringUtils::format("effect/skill_%d_0_f.csb", stype);
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	csbnode->addChild(effectnode, 3, "playskillani");

	float movex = 0;
	float movey = 0;
	float tanz = 0;
	if (enemyindex >= 6)
	{
		float offsety = 450;
		float angle = 0;
		Vec2 mypos = this->getPosition();
		Vec2 enemypos = this->getParent()->getChildByTag(enemyindex)->getPosition();
		float tanx = enemypos.x - mypos.x;
		float tany = enemypos.y - mypos.y;
		tanz = sqrt(tanx*tanx + tany * tany);
		float tan_yx = std::fabs(tany) / std::fabs(tanx);
		angle = 90 - atan(tan_yx) * 180 / M_PI;

		if (tanx > 0)
		{
			effectnode->setRotation(angle);
			movex = tanx - offsety * sin(angle * M_PI / 180);
		}
		else
		{
			effectnode->setRotation(-angle);
			movex = -fabs(tanx) + offsety * sin(angle * M_PI / 180);
		}
		movey = tany - offsety * cos(angle * M_PI / 180);
	}
	else
	{
		float offsety = 450;
		float angle = 0;
		Vec2 mypos = this->getPosition();
		Vec2 pos = this->getParent()->getChildByTag(enemyindex)->getPosition();
		float tanx = pos.x - mypos.x;
		float tany = pos.y - mypos.y;
		tanz = sqrt(tanx*tanx + tany * tany);
		float tan_yx = std::fabs(tany) / std::fabs(tanx);
		angle = 90 - atan(tan_yx) * 180 / M_PI;
		
		if (tanx > 0)
		{
			effectnode->setRotation(-angle);
			movex = tanx - offsety * sin(angle * M_PI / 180);
		}
		else
		{
			effectnode->setRotation(angle);
			movex = -fabs(tanx) + offsety * sin(angle * M_PI / 180);
		}

		movey = -fabs(tany) + offsety * cos(angle * M_PI / 180);
	}
	//if (tanz > 420)
		effectnode->runAction(MoveTo::create(1.0f, Vec2(movex, movey)));
	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
	this->scheduleOnce(schedule_selector(FightHeroNode::removePlaySkillAnim), action->getDuration() / 60.0f);
	this->scheduleOnce(schedule_selector(FightHeroNode::resetZorder), action->getDuration() / 60.0f);
}

void FightHeroNode::attackedSkillEffect(int stype, int myHeroPos)
{
	std::string effectname = StringUtils::format("effect/skill_%d_1.csb", stype);
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	csbnode->addChild(effectnode, 3, "sufferskillani");

	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, stype==3);

	if (stype != 3)
		this->scheduleOnce(schedule_selector(FightHeroNode::removeSufferSkillAnim), action->getDuration()/60.0f);
	

	if (stype == 1)
	{
		float movex = 0;
		float movey = 0;
		float offsety = 450;
		float angle = 0;
		float tanz = 0;
		if (myHeroPos < 6)
		{
			Vec2 pos = this->getPosition();
			Vec2 mypos = this->getParent()->getChildByTag(myHeroPos)->getPosition();
			float tanx = pos.x - mypos.x;
			float tany = pos.y - mypos.y;
			float tanz = sqrt(tanx*tanx + tany * tany);
			float tan_yx = std::fabs(tany) / std::fabs(tanx);
			angle = 90 - atan(tan_yx) * 180 / M_PI;

			if (tanx > 0)
			{
				effectnode->setRotation(angle);
				movex = -tanx + offsety * sin(angle * M_PI / 180);
			}
			else
			{
				effectnode->setRotation(-angle);
				movex = -tanx - offsety * sin(angle * M_PI / 180);
			}
			movey = -tany + offsety * cos(angle * M_PI / 180);
		}
		else
		{
			Vec2 pos = this->getPosition();
			Vec2 mypos = this->getParent()->getChildByTag(myHeroPos)->getPosition();
			float tanx = pos.x - mypos.x;
			float tany = pos.y - mypos.y;
			tanz = sqrt(tanx*tanx + tany * tany);
			float tan_yx = std::fabs(tany) / std::fabs(tanx);
			angle = 90 - atan(tan_yx) * 180 / M_PI;

			for (int i = 0; i < effectnode->getChildrenCount(); i++)
			{
				Sprite* s = (Sprite*)effectnode->getChildren().at(i);
				s->setFlippedY(true);
				s->setPositionY(s->getPositionY() + 500);
			}
			if (tanx > 0)
			{
				effectnode->setRotation(-angle);
				movex = -tanx + offsety * sin(angle * M_PI / 180);
			}
			else
			{
				effectnode->setRotation(angle);
				movex = -tanx - offsety * sin(angle * M_PI / 180);
			}
			movey = fabs(tany) - offsety * cos(angle * M_PI / 180);
		}

		//if (tanz > 420)
			effectnode->runAction(MoveTo::create(1.0f, Vec2(movex, movey)));
	}
	if (stype != SKILL_5)
		this->scheduleOnce(schedule_selector(FightHeroNode::resetZorder), action->getDuration() / 60.0f);
}

void FightHeroNode::resetZorder(float dt)
{
	for (int i = 0; i < 12; i++)
	{
		FightHeroNode* myheronode = (FightHeroNode*)this->getParent()->getChildByTag(i);
		if (myheronode != NULL)
			myheronode->setLocalZOrder(i);
	}
}

void FightHeroNode::showAtkOrHurtAnim(int type)
{
	std::string effectname;
	if (type == 0)
	{
		effectname ="effect/atkanim.csb";
	}
	else
	{
		effectname = "effect/hurtanim.csb";
	}
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
	csbnode->addChild(effectnode, 3, "atkhurt");
	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
	this->scheduleOnce(schedule_selector(FightHeroNode::removeAtkOrHurtAnim), 0.33f);
}

void FightHeroNode::showDeathAnim()
{
	std::string effectname = "effect/carddeath.csb";
	auto effectnode = CSLoader::createNode(effectname);
	effectnode->setPosition(Vec2(this->getPositionX(), this->getPositionY() + 20));
	this->getParent()->addChild(effectnode, 20, "death");
	auto action = CSLoader::createTimeline(effectname);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
	this->scheduleOnce(schedule_selector(FightHeroNode::HideMe), action->getDuration() / 60.0 - 0.5f);
	this->scheduleOnce(schedule_selector(FightHeroNode::removeDeathAnim), action->getDuration()/60.0f);
}

void FightHeroNode::removeDeathAnim(float dt)
{
	this->getParent()->removeChildByName("death");
}

void FightHeroNode::HideMe(float dt)
{
	this->setVisible(false);
}

void FightHeroNode::removeAtkOrHurtAnim(float dt)
{
	csbnode->removeChildByName("atkhurt");
}

void FightHeroNode::removePlaySkillAnim(float dt)
{
	csbnode->removeChildByName("playskillani");
}

void FightHeroNode::removeSufferSkillAnim(float dt)
{
	csbnode->removeChildByName("sufferskillani");
}

void FightHeroNode::showSkillAttrIcon(float dt)
{
	int i = 0;
	std::map<int, std::vector<Sprite*>>::iterator it;
	for (it = map_skillattricon.begin(); it != map_skillattricon.end(); it++)
	{
		for (unsigned int m = 0; m < map_skillattricon[it->first].size(); m++)
		{
			map_skillattricon[it->first][m]->setPosition(-50 + 40 * i, -120);
			map_skillattricon[it->first][m]->setVisible(true);
			i++;
		}
	}

	std::map<int, std::vector<Label*>>::iterator lit;
	for (lit = map_skillattrlabel.begin(); lit != map_skillattrlabel.end();)
	{
		int i = 0;
		for (unsigned int m = 0; m < map_skillattrlabel[lit->first].size(); m++)
		{
			map_skillattrlabel[lit->first][m]->runAction(Sequence::create(DelayTime::create(i*2.0f), Show::create(), FadeOut::create(2.0f), RemoveSelf::create(), NULL));
			i++;
		}
		lit = map_skillattrlabel.erase(lit);
	}
}

void FightHeroNode::refreshSkillAttrIcon(int stype)
{
	for (unsigned int m = 0; m < map_skillattricon[stype].size(); m++)
	{
		map_skillattricon[stype][m]->runAction(Sequence::create(FadeOut::create(0.2f), FadeIn::create(0.2f), FadeOut::create(0.1f), RemoveSelf::create(), NULL));
	}
	if (map_skillattricon.find(stype) != map_skillattricon.end())
		map_skillattricon.erase(stype);
	this->scheduleOnce(schedule_selector(FightHeroNode::showSkillAttrIcon), 0.5f);
}