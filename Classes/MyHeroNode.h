#ifndef _MYHERO_NODE_H_
#define _MYHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class MyHeroNode :public Node
{
public:
	MyHeroNode();
	~MyHeroNode();

	bool init(Hero* herodata);

	static MyHeroNode* create(Hero* herodata);

private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* vocationtextlbl;
	cocos2d::ui::Text* vocationlbl;
	cocos2d::ui::Text* lvtextlbl;
	cocos2d::ui::Text* lvlbl;
	Hero* m_heroData;
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onbgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateData();
};
#endif

