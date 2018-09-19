#pragma once
#ifndef _TASKMAIN_NODE_H_
#define _TASKMAIN_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "TaskLayer.h"

USING_NS_CC;

class TaskNode :public Node
{
public:
	TaskNode();
	~TaskNode();

	bool init(TaskData* data, int type = 0);//默认0主线，1支线

	static TaskNode* create(TaskData* data, int type = 0);

private:
	cocos2d::ui::Widget* redpoint;//提示红点
	cocos2d::ui::Widget* redtip;
	cocos2d::ui::Text* namelbl;//名字
	cocos2d::ui::Widget* finish;//完成
	TaskData* m_Data;
private:
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateData(float dt);
	int m_type;
	bool clickflag;
	Vec2 beginTouchPoint;
};
#endif

