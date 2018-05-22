#ifndef __BUILDINGLVUP_LAYER_H__
#define __BUILDINGLVUP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
USING_NS_CC;

class BuildingLvUpLayer : public cocos2d::Layer
{
public:
	BuildingLvUpLayer();
	~BuildingLvUpLayer();
	static BuildingLvUpLayer* create(Building* building);
	virtual void onExit();
    bool init(Building* building);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	//更新UI,数值
	void updateUI(float dt);

private:
	cocos2d::ui::Text* namelbl[3];//资源名字ABEL
	cocos2d::ui::Text* countlbl[3];//资源数量LABEL
	Building* m_building;
};

#endif
