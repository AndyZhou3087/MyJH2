#pragma once
#pragma once
#ifndef _Hospital_LAYER_H_
#define _Hospital_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Hero.h"

USING_NS_CC;

class HospitalLayer :public Layer
{
public:
	HospitalLayer();
	~HospitalLayer();

	virtual bool init();

	CREATE_FUNC(HospitalLayer);
	void updateContent();

private:
	Node* m_csbnode;
	cocos2d::ui::ScrollView* scrollview;
	std::vector<Hero*> vec_deadheros;
	cocos2d::ui::Text* cointext;
	cocos2d::ui::Text* silvertext;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

public:

};
#endif
