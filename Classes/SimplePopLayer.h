#ifndef __SIMPLEPOP_LAYER_H__
#define __SIMPLEPOP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class SimplePopLayer : public cocos2d::Layer
{
public:
	SimplePopLayer();
	~SimplePopLayer();

	static SimplePopLayer* create(std::string content);
	bool init(std::string content);
};

#endif
