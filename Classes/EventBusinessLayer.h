#ifndef _EVENTBUSINESS_LAYER_H_
#define _EVENTBUSINESS_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "MapBlock.h"
USING_NS_CC;

class EventBusinessLayer :public Layer
{
public:
	EventBusinessLayer();
	~EventBusinessLayer();

	virtual bool init();

	static EventBusinessLayer* create();

	void buyRes(FOURProperty res);

private:

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	std::vector<cocos2d::ui::ScrollView*> vec_scrollview;

	void updateScrollviewContent();

	void loadScrollviewData();

	void loadMyPackageRes();

	static bool sortBuyResByType(FOURProperty a, FOURProperty b);

	void delayShowUI(float dt);

	void updateRichLabel();

private:
	cocos2d::ui::Text* carrycountlbl;

	cocos2d::ui::Text* mycoinlbl;
	cocos2d::ui::Text* mysilverlbl;

	std::vector<FOURProperty> vec_buyres;
	std::vector<FOURProperty> vec_mypackagres;

	bool clickflag;
	Vec2 beginTouchPoint;
};
#endif

