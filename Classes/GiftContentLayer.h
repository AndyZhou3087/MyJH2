#ifndef __GiftContentLayer_H__
#define __GiftContentLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class GiftContentLayer : public cocos2d::Layer
{
public:
	GiftContentLayer();
	~GiftContentLayer();

	static GiftContentLayer* create(ShopData* data, int tag);
	bool init(ShopData* data, int tag);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	ShopData* m_data;
	int m_tag;
};

#endif
