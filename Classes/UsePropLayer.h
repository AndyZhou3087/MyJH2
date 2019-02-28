#ifndef __USEPROP_LAYER_H__
#define __USEPROP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class UsePropLayer : public cocos2d::Layer
{
public:
	UsePropLayer();
	~UsePropLayer();

	static UsePropLayer* create(std::string strid, int rcount);
	bool init(std::string strid, int rcount);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	DynamicValueInt needcoincount;
	std::string m_resid;
};

#endif
