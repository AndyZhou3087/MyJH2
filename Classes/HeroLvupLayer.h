#pragma once
#ifndef __HeroLvup_LAYER_H__
#define __HeroLvup_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
#include "ResBase.h"
USING_NS_CC;

class HeroLvupLayer : public cocos2d::Layer
{
public:
	HeroLvupLayer();
	~HeroLvupLayer();
	static HeroLvupLayer* create(Hero* herodata);
	virtual void onExit();
	bool init(Hero* herodata);

private:

	/************************************
	点击按钮事件
	*************************************/
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	/************************************
	点击物品事件
	*************************************/
	void onGoodsClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updataAtrrUI(float dt);
private:

	//英雄数据
	Hero* m_heroData;
	//名字编辑控件
	cocos2d::ui::EditBox* m_editName;

	//血量label
	cocos2d::ui::Text* hplbl;

	//攻击label
	cocos2d::ui::Text* atkbl;

	//防御label
	cocos2d::ui::Text* dflbl;

	//攻击速度label
	cocos2d::ui::Text* atkspeedlbl;

	//暴击label
	cocos2d::ui::Text* critlbl;

	//闪避label
	cocos2d::ui::Text* dodgelbl;

	int langtype;

	cocos2d::ui::Text* goodarr[4];

	cocos2d::ui::LoadingBar* expbar;
	cocos2d::ui::Text* explbl;
	cocos2d::ui::Text* lvUIText;
};

#endif
