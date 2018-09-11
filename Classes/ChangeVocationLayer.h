#pragma once
#ifndef __ChangeVocation_LAYER_H__
#define __ChangeVocation_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"

USING_NS_CC;

class ChangeVocationLayer : public cocos2d::Layer
{
public:
	ChangeVocationLayer();
	~ChangeVocationLayer();
	/****************************************
	@para:data 需要传递的数据
	@para: forwhere 0--转职，1--突破
	*************************************/
	static ChangeVocationLayer* create(Hero* herodata,int forwhere = 0);
	virtual void onExit();
	bool init(Hero* herodata, int forwhere = 0);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	Hero* m_herodata;
	int m_forwhere;
	std::string needresid;

};

#endif
