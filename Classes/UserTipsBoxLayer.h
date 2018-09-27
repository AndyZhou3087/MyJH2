
#ifndef _UserTipsBoxLayer_H_
#define _UserTipsBoxLayer_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UserTipsBoxLayer :public Layer
{
public:
	UserTipsBoxLayer();
	~UserTipsBoxLayer();

	virtual bool init();

	static UserTipsBoxLayer* create();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
};
#endif

