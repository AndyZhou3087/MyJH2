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
	cocos2d::ui::Button* accbtn;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void accpTask();
	void getRewards();

private:
	TaskMainData* m_data;
	std::vector<std::vector<std::string>> rewards;
};
#endif

