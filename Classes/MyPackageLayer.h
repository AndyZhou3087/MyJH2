#ifndef _MYPACKAGE_LAYER_H_
#define _MYPACKAGE_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

class MyPackageLayer :public Layer
{
public:
	MyPackageLayer();
	~MyPackageLayer();

	virtual bool init();

	static MyPackageLayer* create();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onclick(Ref* pSender);

	void updateScrollviewContent();

private:
	cocos2d::ui::ScrollView* scrollview;
};
#endif

