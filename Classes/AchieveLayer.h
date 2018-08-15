#pragma once
#pragma once
#ifndef _AchieveLayer_H_
#define _AchieveLayer_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ResBase.h"
#include "GlobalInstance.h"
USING_NS_CC;

class AchieveLayer :public Layer
{
public:
	AchieveLayer();
	~AchieveLayer();

	virtual bool init();

	CREATE_FUNC(AchieveLayer);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	cocos2d::ui::ScrollView* scrollview;
};
#endif

