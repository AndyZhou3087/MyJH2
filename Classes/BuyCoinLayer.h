#ifndef __BUYCOIN_LAYER_H__
#define __BUYCOIN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class BuyCoinLayer : public cocos2d::Layer
{
public:
	BuyCoinLayer();
	~BuyCoinLayer();

	static BuyCoinLayer* create(int coincout);
	bool init(int coincout);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:

};

#endif
