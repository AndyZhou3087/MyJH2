#pragma once
#pragma once
#ifndef _TaskDaily_NODE_H_
#define _TaskDaily_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class TaskDailyNode :public Node
{
public:
	TaskDailyNode();
	~TaskDailyNode();

	bool init(DailyTaskData* data);

	static TaskDailyNode* create(DailyTaskData* data);

private:
	DailyTaskData* m_Data;

	cocos2d::ui::Widget* finish;
	cocos2d::ui::Button* actionbtn;
	cocos2d::ui::LoadingBar* taskprobar;
	cocos2d::ui::Button* getbtn;
	cocos2d::ui::Text* taskprotext;
private:
	void onbtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void ongetClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateData(float dt);
};
#endif

