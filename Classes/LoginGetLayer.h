#ifndef __LOGIN_GET_LAYER_H__
#define __LOGIN_GET_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "ResBase.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class LoginGetLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	LoginGetLayer();
	~LoginGetLayer();

	static LoginGetLayer* create();
	bool init();
private:
	void onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void removeSelf(float dt);
	void onFinish(int code);
	void showRwd();

	void showRotation(Node* node);
private:
	std::vector<MSGAWDSDATA> vec_rewards;
	int getcount;
	Label *desclbl;
};

#endif
