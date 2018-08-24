#pragma once
#pragma once
#ifndef __UNLOCKCHAPTER_LAYER_H__
#define __UNLOCKCHAPTER_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class UnlockChapterLayer : public cocos2d::Layer
{
public:
	UnlockChapterLayer();
	~UnlockChapterLayer();

	static UnlockChapterLayer* create(int unlockchapter = -1);
	bool init(int unlockchapter = -1);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:

};

#endif
