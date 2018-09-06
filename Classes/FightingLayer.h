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

	static FightingLayer* create(std::vector<Npc*> enemyHeros, int bgtype);
	virtual void onExit();
	bool init(std::vector<Npc*> enemyHeros, int bgtype);

	void showAtk(int fightertag);

	void pauseAtkSchedule();
	void resumeAtkSchedule();

	void updateMapHero(int which);

	//1--Ê¤Àû£¬0-Ê§°Ü,-1Õ½¶·ÖÐ
	int checkFightResult();

	static bool sortbyHp_lesscb(FightHeroNode* a, FightHeroNode* b);

	void clearSkillsData();

	void checkNewGuide();
	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	std::vector<Npc*> m_enemyHeros;
	cocos2d::ui::Widget* m_escapebtn;
	int fightcount;

	std::vector<FightHeroNode*> vec_myheronode;
};

#endif
