#ifndef _RANDHERO_NODE_H_
#define _RANDHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class RandHeroNode :public Node
{
public:
	RandHeroNode();
	~RandHeroNode();

	virtual bool init();

	static RandHeroNode* create();

	void markRecruited();
	void setData(Hero* herodata);
public:
private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* vocationtextlbl;
	cocos2d::ui::Text* vocationlbl;
	cocos2d::ui::Text* potentialtextlbl;
	cocos2d::ui::Text* potentiallbl;
	cocos2d::ui::Widget* isrecruitedWidget;

private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

