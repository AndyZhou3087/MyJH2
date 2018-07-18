#pragma once
#ifndef __HintBox_LAYER_H__
#define __HintBox_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResBase.h"
#include "Hero.h"
USING_NS_CC;

class TrainHintLayer : public cocos2d::Layer
{
public:
	TrainHintLayer();
	~TrainHintLayer();

	static TrainHintLayer* create(Hero* herodata);
	bool init(Hero* herodata);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Hero* m_herodata;
};

#endif
