#ifndef __FIGHTINGRESULT_LAYER_H__
#define __FIGHTINGRESULT_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "MapBlock.h"
USING_NS_CC;

class FightingResultLayer : public cocos2d::Layer
{
public:
	FightingResultLayer();
	~FightingResultLayer();

	static FightingResultLayer* create(std::vector<FOURProperty> reward_res, int winexp, int onstate = 0);//0--正常战斗；1--经济场战斗
	virtual void onExit();
	bool init(std::vector<FOURProperty> reward_res, int winexp, int onstate = 0);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	std::vector<FOURProperty> m_rewards;
	int m_onstate;
};

#endif
