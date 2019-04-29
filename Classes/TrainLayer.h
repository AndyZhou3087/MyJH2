#pragma once
#ifndef __TRAIN_LAYER_H__
#define __TRAIN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
USING_NS_CC;

class TrainLayer : public cocos2d::Layer
{
public:
	TrainLayer();
	~TrainLayer();
	static TrainLayer* create(Building* buidingData);
	bool init(Building* buidingData);

	//更新scrollview
	void updateContent();

	void lvup();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;

	void loadData();

	void updateRepairTime(float dt);
	void updateRepairUi();
private:
	int langtype;
	cocos2d::ui::Text* lvUIlbl;
	cocos2d::ui::Text* repairtimelbl;
	cocos2d::ui::Widget* repairbtn;
};

#endif
