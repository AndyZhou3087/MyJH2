#ifndef _FIGHTHERO_NODE_H_
#define _FIGHTHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class FightHeroNode :public Node
{
public:
	FightHeroNode();
	~FightHeroNode();

	virtual bool init();

	static FightHeroNode* create();

	void setData(Hero* herodata);
private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::LoadingBar* hp_bar;
	Hero* m_heroData;
private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

