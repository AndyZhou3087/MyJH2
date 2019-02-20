#ifndef __GoBackLayer_LAYER_H__
#define __GoBackLayer_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "DynamicValue.h"

USING_NS_CC;

class GoBackLayer : public cocos2d::Layer
{
public:
	GoBackLayer();
	~GoBackLayer();

	static GoBackLayer* create();
	virtual void onExit();
	bool init();

	//引导
	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

	//消耗元宝回城
	void costCoinGoback(int gotype);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	cocos2d::ui::Button* gocitybtn;
	cocos2d::ui::Button* gomapbtn;
	DynamicValueInt gobackcoin;
};

#endif
