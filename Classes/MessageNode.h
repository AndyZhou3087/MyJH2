#ifndef _MESSASGE_NODE_H_
#define _MESSASGE_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
USING_NS_CC;

typedef enum
{
	MSG_NOTICE = 0,
	MSG_PRIZE,
	MSG_ADDGOODS
}MESSAGETYPE;

class MessageNode :public Node
{
public:
	MessageNode();
	~MessageNode();

	virtual bool init(int index);

	static MessageNode* create(int index);

	void updataUI();
private:
	bool clickflag;
	Vec2 beginTouchPoint;
	int lastStatus;
	cocos2d::ui::Widget* redpoint;
	cocos2d::ui::Widget* icon;
private:
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void refreshStatus(int type, int status);
};
#endif

