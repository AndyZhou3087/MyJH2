#ifndef _BUYSINGLERES_LAYER_H_
#define _BUYSINGLERES_LAYER_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class BuySingleResLayer :public Layer
{
public:
	BuySingleResLayer();
	~BuySingleResLayer();

	bool init(MSGAWDSDATA fproper);

	static BuySingleResLayer* create(MSGAWDSDATA fproper);

private:
	cocos2d::ui::Text* totalpricelbl;
	cocos2d::ui::ImageView* priceicon;
	cocos2d::ui::Text* rescountlbl;

	int buycount;
	MSGAWDSDATA m_fproper;
	bool m_isLongPress;
	Node* m_longTouchNode;
	cocos2d::ui::Text* attrlblArr[6];
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateData();

	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void addCount();
	void subCount();
	void getAttr(std::string gfid, int type);
};
#endif

