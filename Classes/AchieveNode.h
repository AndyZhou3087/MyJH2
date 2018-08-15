#pragma once
#pragma once
#pragma once
#ifndef _AchieveNode_H_
#define _AchieveNode_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class AchieveNode :public Node
{
public:
	AchieveNode();
	~AchieveNode();

	bool init(AchieveData* data);

	static AchieveNode* create(AchieveData* data);

private:
	AchieveData* m_Data;

	cocos2d::ui::Widget* finish;
	cocos2d::ui::LoadingBar* taskprobar;
	cocos2d::ui::Button* getbtn;
	cocos2d::ui::Text* taskprotext;

	bool clickflag;
	Vec2 beginTouchPoint;
private:
	void onbtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateData(float dt);
};
#endif

