#ifndef __BUYRES_LAYER_H__
#define __BUYRES_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class BuyResLayer : public cocos2d::Layer
{
public:
	BuyResLayer();
	~BuyResLayer();

	static BuyResLayer* create(std::vector<MSGAWDSDATA> vec_res);
	bool init(std::vector<MSGAWDSDATA> vec_res);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	DynamicValueInt needcoincount;
	std::vector<MSGAWDSDATA> m_vecres;
};

#endif
