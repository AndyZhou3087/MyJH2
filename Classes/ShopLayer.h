#ifndef __ShopLayer_H__
#define __ShopLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "HttpDataSwap.h"

USING_NS_CC;

typedef enum
{
	PAY_FAIL = -1,
	PAY_SUCC = 0
}PYARET;

class ShopLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	ShopLayer();
	~ShopLayer();

	static ShopLayer* create();
	static void beginPay(int index);
	static void setMessage(PYARET ret);
	virtual void onExit();
	bool init();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onQQClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	static void showVipReward(ShopData* data, int tag);

	void updateCoinLable(float dt);

private:
	cocos2d::ui::ScrollView* scrollView;
	static int payindex;
	static bool isPaying;
	cocos2d::ui::Text* coinlbl;
};

#endif
