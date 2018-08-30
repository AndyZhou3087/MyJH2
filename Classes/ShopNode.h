#ifndef _ShopNode_H_
#define _ShopNode_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
USING_NS_CC;

class ShopNode :public Node
{
public:
	ShopNode();
	~ShopNode();

	bool init(ShopData* shopdata);

	static ShopNode* create(ShopData* shopdata);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	ShopData* m_shopdata;
	bool clickflag;
	Vec2 beginTouchPoint;

};
#endif