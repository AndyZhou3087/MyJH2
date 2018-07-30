#pragma once
#pragma once
#ifndef __WgLv_LAYER_H__
#define __WgLv_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResBase.h"
#include "GongFa.h"
USING_NS_CC;

class WgLvLayer : public cocos2d::Layer
{
public:
	WgLvLayer();
	~WgLvLayer();
	static WgLvLayer* create(ResBase* res);
	virtual void onExit();
	bool init(ResBase* res);

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

	GongFa* m_res;

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

	//血量label
	cocos2d::ui::Text* hplbl2;

	//攻击label
	cocos2d::ui::Text* atkbl2;

	//防御label
	cocos2d::ui::Text* dflbl2;

	//攻击速度label
	cocos2d::ui::Text* atkspeedlbl2;

	//暴击label
	cocos2d::ui::Text* critlbl2;

	//闪避label
	cocos2d::ui::Text* dodgelbl2;

	int langtype;

	cocos2d::ui::Text* goodarr[4];

	cocos2d::ui::LoadingBar* expbar;
	cocos2d::ui::Text* explbl;
	cocos2d::ui::Text* lvtext;
};

#endif
