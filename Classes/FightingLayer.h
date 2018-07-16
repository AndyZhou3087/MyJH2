#ifndef __FIGHTING_LAYER_H__
#define __FIGHTING_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class FightingLayer : public cocos2d::Layer
{
public:
	FightingLayer();
	~FightingLayer();

	static FightingLayer* create(std::vector<Npc*> enemyHeros);
	virtual void onExit();
	bool init(std::vector<Npc*> enemyHeros);

	void showAtk(Npc* ndata);
	void pauseAtkSchedule();
	void resumeAtkSchedule();

	void updateMapHero(int which);

	//1--ʤ����0-ʧ��,-1ս����
	int checkFightResult();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	std::vector<Npc*> m_enemyHeros;
};

#endif