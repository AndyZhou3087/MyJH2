#ifndef __WellGiftLayer_H__
#define __WellGiftLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class WellGiftLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	WellGiftLayer();
	~WellGiftLayer();

	static WellGiftLayer* create();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);

	void getVipReward();
	void onResclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void selectVip(ShopData* data);
private:
	cocos2d::ui::Text* desc;
	cocos2d::ui::ImageView* box[6];
	int langtype;
	bool isgetingvip;//是否正在领取月卡
};

#endif
