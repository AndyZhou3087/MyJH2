#ifndef __HOMEHILL_LAYER_H__
#define __HOMEHILL_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
#include "HillResNode.h"
USING_NS_CC;

class HomeHillLayer : public cocos2d::Layer
{
public:
	HomeHillLayer();
	~HomeHillLayer();
	static HomeHillLayer* create(Building* buidingData);
	virtual void onExit();
    bool init(Building* buidingData);
	//更新scrollview
	void refreshResUi();
	//升级
	void lvup();

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateTime(float dt);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;
	cocos2d::ui::LoadingBar* m_timebar;
	cocos2d::ui::Text* m_timelbl;
	cocos2d::ui::Text* m_leftfarmercount;
	cocos2d::ui::Text* lvUIlbl;
	Node* csbnode;
	HillResNode* m_hillResNode;
};

#endif
