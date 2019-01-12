#ifndef _MOREBTN_NODE_H_
#define _MOREBTN_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;
typedef enum
{
	SETBTN,
	LOGINBTN
}SUB_MENUTYPE;
class MoreBtnNode :public Node
{
public:
	MoreBtnNode();
	~MoreBtnNode();

	bool init();

	static MoreBtnNode* create();

private:

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
};
#endif

