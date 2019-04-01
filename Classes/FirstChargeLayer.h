#ifndef __FIRSTCHARGE_LAYER_H__
#define __FIRSTCHARGE_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class FirstChargeLayer : public cocos2d::Layer
{
public:
	FirstChargeLayer();
	~FirstChargeLayer();

	static FirstChargeLayer* create(ShopData* data);
	bool init(ShopData* data);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void removeSelf();

private:
	std::vector<MSGAWDSDATA> vec_rewards;
};

#endif
