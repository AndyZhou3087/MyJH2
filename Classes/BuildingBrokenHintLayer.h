#ifndef __BUILDINGBROENHINT_LAYER_H__
#define __BUILDINGBROENHINT_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class BuildingBrokenHintLayer : public cocos2d::Layer
{
public:
	BuildingBrokenHintLayer();
	~BuildingBrokenHintLayer();

	static BuildingBrokenHintLayer* create();
	bool init();
private:
	void removeSelf();

private:

};

#endif
