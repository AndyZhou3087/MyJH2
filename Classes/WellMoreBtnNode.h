#ifndef _WELLMOREBTN_NODE_H_
#define _WELLMOREBTN_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;
typedef enum
{
	REBATEBTN,
	WELLCARDBTN
}WELLSUB_MENUTYPE;
class WellMoreBtnNode :public Node
{
public:
	WellMoreBtnNode();
	~WellMoreBtnNode();

	bool init();

	static WellMoreBtnNode* create();

private:

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
};
#endif

