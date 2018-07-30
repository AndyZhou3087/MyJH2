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
	�����ť�¼�
	*************************************/
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	/************************************
	�����Ʒ�¼�
	*************************************/
	void onGoodsClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updataAtrrUI(float dt);
private:

	GongFa* m_res;

	//Ѫ��label
	cocos2d::ui::Text* hplbl;

	//����label
	cocos2d::ui::Text* atkbl;

	//����label
	cocos2d::ui::Text* dflbl;

	//�����ٶ�label
	cocos2d::ui::Text* atkspeedlbl;

	//����label
	cocos2d::ui::Text* critlbl;

	//����label
	cocos2d::ui::Text* dodgelbl;

	//Ѫ��label
	cocos2d::ui::Text* hplbl2;

	//����label
	cocos2d::ui::Text* atkbl2;

	//����label
	cocos2d::ui::Text* dflbl2;

	//�����ٶ�label
	cocos2d::ui::Text* atkspeedlbl2;

	//����label
	cocos2d::ui::Text* critlbl2;

	//����label
	cocos2d::ui::Text* dodgelbl2;

	int langtype;

	cocos2d::ui::Text* goodarr[4];

	cocos2d::ui::LoadingBar* expbar;
	cocos2d::ui::Text* explbl;
	cocos2d::ui::Text* lvtext;
};

#endif
