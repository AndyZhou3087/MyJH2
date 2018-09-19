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

	static SimpleResPopLayer* create(std::string resid, int forwhere = 0);
	bool init(std::string resid, int forwhere = 0);
};

#endif
