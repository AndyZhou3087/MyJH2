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
private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::LoadingBar* hp_bar;
	cocos2d::ui::LoadingBar* atkspeed_bar;
	cocos2d::ui::Widget* atkspeed_barbg;
	Npc* m_Data;
	int m_datatype;
private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

