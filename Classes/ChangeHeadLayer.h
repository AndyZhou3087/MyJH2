#pragma once
#ifndef __ChangeHeadLayer_H__
#define __ChangeHeadLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class ChangeHeadLayer : public cocos2d::Layer
{
public:
	ChangeHeadLayer();
	~ChangeHeadLayer();

	static ChangeHeadLayer* create();
	virtual void onExit();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	Sprite* selectArr[8];
	int lastSelectIndex;
};

#endif
