#ifndef _REBATEAWD_NODE_H_
#define _REBATEAWD_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
USING_NS_CC;

class RebateAwdNode :public Node
{
public:
	RebateAwdNode();
	~RebateAwdNode();

	bool init(std::string awdstr);

	static RebateAwdNode* create(std::string awdstr);

	void setSelect();
private:
	bool clickflag;
	Vec2 beginTouchPoint;
	Node* selectnode;
	cocos2d::ui::Button* actionbtn;
	cocos2d::ui::ImageView* getbtntext;
	int langtype;
	std::vector<MSGAWDSDATA> vec_resdata;
private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

