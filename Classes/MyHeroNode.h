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

	bool init(Hero* herodata, int showtype = HS_OWNED, int forwhere = 0);//0-默认；1--客栈

	static MyHeroNode* create(Hero* herodata, int showtype = HS_OWNED, int forwhere = 0);
	void updateData();

	void changeBtnContent();
	void cacelTraining();
private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* vocationtextlbl;
	cocos2d::ui::Text* vocationlbl;
	cocos2d::ui::Text* lvtextlbl;
	cocos2d::ui::Text* lvlbl;
	cocos2d::ui::ImageView* actbtntxt;
	cocos2d::ui::ImageView* statetag;
	cocos2d::ui::ImageView* statetag_1;
	cocos2d::ui::Text* tagtext;
	cocos2d::ui::Text* tagtext_1;
	cocos2d::ui::Text* countdown;
	cocos2d::ui::Text* count;
	cocos2d::ui::ImageView* stars[5];
	cocos2d::ui::Widget* silver;
	cocos2d::ui::ImageView* actbtn;
	cocos2d::ui::Text* hpdesc;
	Hero* m_heroData;
	int m_showtype;
	int langtype;
	bool clickflag;
	Vec2 beginTouchPoint;
	Node* csbnode;
	cocos2d::ui::Widget* arrowglv;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void setStateTag(int state);
	void updateTime(float dt);

private:
	int lastVocation;
	int lastbreakcount;
};
#endif

