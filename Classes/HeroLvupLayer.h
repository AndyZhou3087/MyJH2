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
	�����ť�¼�
	*************************************/
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	/************************************
	�����Ʒ�¼�
	*************************************/
	void onGoodsClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updataAtrrUI(float dt);
private:

	//Ӣ������
	Hero* m_heroData;
	//���ֱ༭�ؼ�
	cocos2d::ui::EditBox* m_editName;

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

	int langtype;

	cocos2d::ui::Text* goodarr[4];

	cocos2d::ui::LoadingBar* expbar;
	cocos2d::ui::Text* explbl;
	cocos2d::ui::Text* lvUIText;
};

#endif
