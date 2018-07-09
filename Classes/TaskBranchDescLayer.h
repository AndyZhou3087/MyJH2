#pragma once
#pragma once
#ifndef _TaskBranchDesc_LAYER_H_
#define _TaskBranchDesc_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"

USING_NS_CC;

class TaskBranchDescLayer :public Layer
{
public:
	TaskBranchDescLayer();
	~TaskBranchDescLayer();

	virtual bool init(TaskBranchData* data);

	static TaskBranchDescLayer* create(TaskBranchData* data);

private:
	cocos2d::ui::Button* accbtn;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void accpTask();
	void getRewards();

private:
	TaskBranchData* m_data;
};
#endif

