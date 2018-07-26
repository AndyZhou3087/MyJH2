#ifndef _CARDHERO_NODE_H_
#define _CARDHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class CardHeroNode :public Node
{
public:
	CardHeroNode();
	~CardHeroNode();

	virtual bool init();

	static CardHeroNode* create();

	void setData(Hero* herodata);
private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* vocationlbl;
	cocos2d::ui::Text* lvlbl;
	cocos2d::ui::Text* desclbl;
	cocos2d::ui::Widget* vocationbox;
	cocos2d::ui::ImageView* vocationicon;
private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

