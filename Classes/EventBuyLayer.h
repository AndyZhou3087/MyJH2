#ifndef _EVENTBUY_LAYER_H_
#define _EVENTBUY_LAYER_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "EventBusinessLayer.h"
USING_NS_CC;

class EventBuyLayer :public Layer
{
public:
	EventBuyLayer();
	~EventBuyLayer();

	bool init(FOURProperty fproper);

	static EventBuyLayer* create(FOURProperty fproper);

private:
	cocos2d::ui::Text* totalpricelbl;
	cocos2d::ui::ImageView* priceicon;
	cocos2d::ui::Text* rescountlbl;

	int buycount;
	FOURProperty m_fproper;
	bool m_isLongPress;
	Node* m_longTouchNode;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateData();

	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void addCount();
	void subCount();

};
#endif

