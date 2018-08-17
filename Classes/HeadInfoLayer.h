#pragma once
#ifndef __HeadInfoLayer_H__
#define __HeadInfoLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class HeadInfoLayer : public cocos2d::Layer
{
public:
	HeadInfoLayer();
	~HeadInfoLayer();

	static HeadInfoLayer* create();
	virtual void onExit();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
};

#endif
