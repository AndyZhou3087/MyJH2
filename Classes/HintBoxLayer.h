#pragma once
#pragma once
#ifndef __HitBox_LAYER_H__
#define __HitBox_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class HintBoxLayer : public cocos2d::Layer
{
public:
	HintBoxLayer();
	~HintBoxLayer();

	static HintBoxLayer* create(std::string str,int forwhere = 0);
	bool init(std::string str, int forwhere = 0);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
private:
	int m_forwhere;
	cocos2d::ui::Widget* okbtn;
};

#endif
