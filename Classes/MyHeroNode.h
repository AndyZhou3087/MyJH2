#ifndef _MYHERO_NODE_H_
#define _MYHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class MyHeroNode :public Node
{
public:
	MyHeroNode();
	~MyHeroNode();

	virtual bool init();

	CREATE_FUNC(MyHeroNode);

private:

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
};
#endif

