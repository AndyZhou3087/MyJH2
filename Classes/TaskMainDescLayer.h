#pragma once
#pragma once
#ifndef _TaskMainDesc_LAYER_H_
#define _TaskMainDesc_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"

USING_NS_CC;

class TaskMainDescLayer :public Layer
{
public:
	TaskMainDescLayer();
	~TaskMainDescLayer();

	virtual bool init(TaskMainData* data);

	static TaskMainDescLayer* create(TaskMainData* data);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
};
#endif

