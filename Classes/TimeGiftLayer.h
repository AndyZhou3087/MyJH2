#ifndef __TIMEGIFTLAYER_H__
#define __TIMEGIFTLAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class TimeGiftLayer : public cocos2d::Layer
{
public:
	TimeGiftLayer();
	~TimeGiftLayer();

	static TimeGiftLayer* create(ShopData* data);
	bool init(ShopData* data);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	ShopData* m_data;
	cocos2d::ui::Text* lefttime;

	void updatetime(float dt);
};

#endif
