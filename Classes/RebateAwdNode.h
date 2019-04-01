#ifndef _REBATEAWD_NODE_H_
#define _REBATEAWD_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class RebateAwdNode :public Node
{
public:
	RebateAwdNode();
	~RebateAwdNode();

	bool init(std::string awdstr);

	static RebateAwdNode* create(std::string awdstr);

private:
	bool clickflag;
	Vec2 beginTouchPoint;
private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

