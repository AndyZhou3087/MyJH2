#ifndef __LOGINREWARD_LAYER_H__
#define __LOGINREWARD_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class LoginRewardLayer : public cocos2d::Layer
{
public:
	LoginRewardLayer();
	~LoginRewardLayer();

	static LoginRewardLayer* create();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void removeSelf();

private:

};

#endif
