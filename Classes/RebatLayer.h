#ifndef __REBATLAYER_H__
#define __REBATLAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class RebatLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	RebatLayer();
	~RebatLayer();

	static RebatLayer* create();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);

private:
	cocos2d::ui::ScrollView* scrollview;

	std::vector<DynamicValueInt> vec_prices;
};

#endif
