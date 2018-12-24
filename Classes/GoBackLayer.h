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

	static GoBackLayer* create(int forwhere = 0);//默认0为回城
	virtual void onExit();
	bool init(int forwhere = 0);

	//引导
	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

	//消耗元宝回城
	void costCoinGoback();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int m_forwhere;
	cocos2d::ui::Button* actionbtn;
	cocos2d::ui::Button* cancelbtn;
	DynamicValueInt gobackcoin;
};

#endif
