#pragma once
#ifndef __TrainSelect_LAYER_H__
#define __TrainSelect_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
#include "Hero.h"
USING_NS_CC;

class TrainSelectLayer : public cocos2d::Layer
{
public:
	TrainSelectLayer();
	~TrainSelectLayer();

	static TrainSelectLayer* create(Hero* herodata);
	virtual void onExit();
	bool init(Hero* herodata);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//更新UI,数值
	void updateUI();

	//资源足够升级
	bool checkResIsEnough();

	//操作
	void action();
private:
	cocos2d::ui::ImageView* selArr[3];//选中标签
	cocos2d::ui::Text* countlbl[3];//
	cocos2d::ui::Text* coincountlbl;
	Hero* m_data;//需要传递的数据
	std::vector<std::map<std::string, int>> vec_res;//需要的资源的数据
	DynamicValueInt coincount;
	int lastSelectIndex;
};

#endif
