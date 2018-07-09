#pragma once
#pragma once
#ifndef _TASKBRANCH_NODE_H_
#define _TASKBRANCH_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "TaskLayer.h"

USING_NS_CC;

class TaskBranchNode :public Node
{
public:
	TaskBranchNode();
	~TaskBranchNode();

	bool init(TaskBranchData* data, TaskLayer* layer);

	static TaskBranchNode* create(TaskBranchData* data, TaskLayer* layer);

private:
	cocos2d::ui::ImageView* redpoint;//提示红点
	cocos2d::ui::Text* namelbl;//名字
	cocos2d::ui::Widget* finish;//完成
	TaskBranchData* m_Data;
private:
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateData(float dt);
	TaskLayer* m_layer;
};
#endif

