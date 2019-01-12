#ifndef __LOGINREWARD_V2_LAYER_H__
#define __LOGINREWARD_V2_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class LoginRewardV2Layer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	LoginRewardV2Layer();
	~LoginRewardV2Layer();

	static LoginRewardV2Layer* create();
	bool init();

	void getLoginData();

	void changeLogin();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void removeSelf();
	void onFinish(int code);
private:

	cocos2d::ui::Text* wordtext;
	Node* csbnode;
	bool isRetry;
};

#endif
