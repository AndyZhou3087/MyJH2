#ifndef __FIGHTING_LAYER_H__
#define __FIGHTING_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
#include "FightHeroNode.h"
USING_NS_CC;

class FightingLayer : public cocos2d::Layer
{
public:
	FightingLayer();
	~FightingLayer();

	static FightingLayer* create(std::vector<Hero*> myHeros, std::vector<Npc*> enemyHeros, int bgtype);
	virtual void onExit();
	bool init(std::vector<Hero*> myHeros, std::vector<Npc*> enemyHeros, int bgtype);

	void showAtk(int fightertag);

	void heroFight(int fightertag);

	void pauseAtkSchedule();
	void resumeAtkSchedule();

	void updateMapHero(int which);

	//1--胜利，0-失败,-1战斗中
	int checkFightResult();

	static bool sortbyHp_lesscb(FightHeroNode* a, FightHeroNode* b);

	void clearSkillsData(int type);//0-我的英雄，1--对手

	void checkNewGuide();
	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	int calcAttackNodeIndex(int fighterindex, int type);

	int calcSkill2AttackNodeIndex(int fighterindex, int type);

	std::vector<int> calcSkill5AttackNodeIndex(int fighterindex, int type, int count);

	std::vector<int> calcSkill6AttackNodeIndex(int fighterindex, int type, int count);

	void fightOver(int ret);

private:
	std::vector<Npc*> m_enemyHeros;
	std::vector<Hero*> m_myHeros;
	cocos2d::ui::Widget* m_escapebtn;
	int fightcount;

	std::vector<FightHeroNode*> vec_atkheronode;
	std::vector< FightHeroNode*> vec_pairheronode;

	std::vector<FightHeroNode*> vec_myfront3node;
	std::vector<FightHeroNode*> vec_myback3node;

	std::vector<FightHeroNode*> vec_enemyfront3node;
	std::vector<FightHeroNode*> vec_enemyback3node;
};

#endif
