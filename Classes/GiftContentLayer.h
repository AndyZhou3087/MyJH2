#ifndef __GiftContentLayer_H__
#define __GiftContentLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class GiftContentLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	GiftContentLayer();
	~GiftContentLayer();

	static GiftContentLayer* create(ShopData* data, int tag, int type = 0);
	bool init(ShopData* data, int tag, int type = 0);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);

	void getVipReward();
	void onResclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	ShopData* m_data;
	int m_tag;
	cocos2d::ui::Text* lefttime;
	cocos2d::ui::Button* buybtn;
	cocos2d::ui::ImageView* buybtntext;
	cocos2d::ui::Text* price;
	int langtype;
	int m_type;
	bool isgetingvip;//是否正在领取月卡
};
#endif
