#pragma once
#ifndef __TrainSelect_LAYER_H__
#define __TrainSelect_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
#include "Hero.h"
#include "MyHeroNode.h"
USING_NS_CC;

class TrainSelectLayer : public cocos2d::Layer
{
public:
	TrainSelectLayer();
	~TrainSelectLayer();

	static TrainSelectLayer* create(Hero* herodata, MyHeroNode* myNode);
	virtual void onExit();
	bool init(Hero* herodata, MyHeroNode* myNode);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//更新UI,数值
	void updateUI();

	//资源足够升级
	bool checkResIsEnough(int index);

	//操作
	void action();

	void updatelbl(float dt);
private:
	cocos2d::ui::ImageView* selArr[3];//选中标签
	cocos2d::ui::Text* countlbl[3];//
	cocos2d::ui::Text* coincountlbl;
	cocos2d::ui::Text* myfcoutlbl;
	Hero* m_data;//需要传递的数据
	std::vector<std::map<std::string, int>> vec_res;//需要的资源的数据
	DynamicValueInt coincount;
	int lastSelectIndex;
	MyHeroNode* myheroNode;
};

#endif
