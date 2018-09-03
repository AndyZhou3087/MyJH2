#ifndef __HEROINTRO_LAYER_H__
#define __HEROINTRO_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class HeroIntroLayer : public cocos2d::Layer
{
public:
	HeroIntroLayer();
	~HeroIntroLayer();

	static HeroIntroLayer* create(std::string hid);
	bool init(std::string hid);
};

#endif
