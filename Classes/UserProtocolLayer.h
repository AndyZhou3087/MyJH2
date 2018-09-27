
#ifndef _UserProtocolLayer_H_
#define _UserProtocolLayer_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UserProtocolLayer :public Layer
{
public:
	UserProtocolLayer();
	~UserProtocolLayer();

	virtual bool init();

	static UserProtocolLayer* create();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
};
extern UserProtocolLayer* userProlayer;
#endif

