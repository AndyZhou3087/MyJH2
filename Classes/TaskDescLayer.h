#pragma once
#pragma once
#ifndef _TaskMainDesc_LAYER_H_
#define _TaskMainDesc_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"

USING_NS_CC;

class TaskDescLayer :public Layer
{
public:
	TaskDescLayer();
	~TaskDescLayer();

	virtual bool init(TaskData* data, int type = 0);//默认0主线，1支线

	static TaskDescLayer* create(TaskData* data, int type = 0);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

private:
	cocos2d::ui::Button* accbtn;
	cocos2d::ui::Button* closebtn;
	cocos2d::ui::Button* getbtn;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void accpTask();
	void getRewards();

private:
	TaskData* m_data;
	int m_type;
	std::vector<std::vector<std::string>> rewards;
};
#endif

