#pragma once
#ifndef _TASKMAIN_NODE_H_
#define _TASKMAIN_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "TaskLayer.h"

USING_NS_CC;

class TaskMainNode :public Node
{
public:
	TaskMainNode();
	~TaskMainNode();

	bool init(TaskMainData* data, TaskLayer* layer);

	static TaskMainNode* create(TaskMainData* data, TaskLayer* layer);

private:
	cocos2d::ui::ImageView* redpoint;//提示红点
	cocos2d::ui::Text* namelbl;//名字
	cocos2d::ui::Widget* finish;//完成
	TaskMainData* m_Data;
private:
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateData(float dt);
	TaskLayer* m_layer;
};
#endif

