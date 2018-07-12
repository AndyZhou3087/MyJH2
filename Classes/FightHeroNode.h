#ifndef _FIGHTHERO_NODE_H_
#define _FIGHTHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
#include "Npc.h"
USING_NS_CC;

class FightHeroNode :public Node
{
public:
	FightHeroNode();
	~FightHeroNode();

	virtual bool init();

	static FightHeroNode* create();

	void setData(Npc* data, int datatype = 0);


	void pauseTimeSchedule();

	void resumeTimeSchedule();

	void hurt(float hp);

	void updateHp();

	void setWinState(int winexp);
private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::LoadingBar* hp_bar;
	cocos2d::ui::LoadingBar* atkspeed_bar;
	cocos2d::ui::Widget* atkspeed_barbg;
	cocos2d::ui::ImageView* statusimg;
	cocos2d::ui::ImageView* retbox;
	cocos2d::ui::ImageView* rettext;
	Npc* m_Data;
	int m_datatype;
	float atkspeed;
	float timedt;
	bool isfighting;
	float hurtup;
	void update(float dt);
	void hpAnim();
	void setBlankBox();


private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void atkAnimFinish();
	void hurtAnimFinish();

};
#endif

