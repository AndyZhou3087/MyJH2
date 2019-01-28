#ifndef __SIMPLERESPOP_LAYER_H__
#define __SIMPLERESPOP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class SimpleResPopLayer : public cocos2d::Layer
{
public:
	SimpleResPopLayer();
	~SimpleResPopLayer();

	static SimpleResPopLayer* create(std::string resid, int forwhere = 0, int needcount = 0);
	bool init(std::string resid, int forwhere, int needcount);
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	std::string m_resid;
	int m_needcount;
};

#endif
