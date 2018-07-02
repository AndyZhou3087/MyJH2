#pragma once
#ifndef __MARKETREFRESHLAYER_LAYER_H__
#define __MARKETREFRESHLAYER_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResBase.h"
USING_NS_CC;


class MarkerRefreshLayer : public cocos2d::Layer
{
public:
	MarkerRefreshLayer();
	~MarkerRefreshLayer();

	static MarkerRefreshLayer* create();
	virtual void onExit();
	bool init();
private:
	void onreBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void closebtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};

#endif
