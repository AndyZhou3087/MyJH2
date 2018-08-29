#pragma once
#ifndef _Task_LAYER_H_
#define _Task_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ResBase.h"
#include "GlobalInstance.h"
USING_NS_CC;

typedef enum
{
	TASK_0 = 0,
	TASK_1,
	TASK_2
}TASKTYPE;

class TaskLayer :public Layer
{
public:
	TaskLayer();
	~TaskLayer();

	virtual bool init();

	CREATE_FUNC(TaskLayer);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onPointClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	Node* m_csbnode;
	cocos2d::ui::ScrollView* scrollview;
	void loadData(int category);
	void updateContent(int category);
	void updateDaily(float dt);

	static bool larger_callback(TaskData a, TaskData b);
	static bool larger_branchcallback(TaskData a, TaskData b);
private:
	std::vector<cocos2d::ui::Button*> vec_categoryBtn;
	int lastCategoryindex;
	int langtype;
	cocos2d::ui::Widget* pnode;
	cocos2d::ui::Text* mypoint;
	cocos2d::ui::LoadingBar* probar;

public:
	void skipContent();
};
#endif

