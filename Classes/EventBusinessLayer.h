#ifndef _EVENTBUSINESS_LAYER_H_
#define _EVENTBUSINESS_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"
USING_NS_CC;

class EventBusinessLayer :public Layer
{
public:
	EventBusinessLayer();
	~EventBusinessLayer();

	virtual bool init();

	static EventBusinessLayer* create();

	void buyRes(MSGAWDSDATA res);

private:

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	std::vector<cocos2d::ui::ScrollView*> vec_scrollview;

	void updateScrollviewContent();

	void loadScrollviewData();

	void loadMyPackageRes();

	static bool sortBuyResByType(MSGAWDSDATA a, MSGAWDSDATA b);

	void delayShowUI(float dt);

	void updateRichLabel(float dt);

private:
	cocos2d::ui::Text* carrycountlbl;

	cocos2d::ui::Text* mycoinlbl;
	cocos2d::ui::Text* mysilverlbl;

	std::vector<MSGAWDSDATA> vec_buyres;
	std::vector<MSGAWDSDATA> vec_mypackagres;

	bool clickflag;
	Vec2 beginTouchPoint;
};
#endif

