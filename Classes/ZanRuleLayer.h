#ifndef __ZanRuleLayer_H__
#define __ZanRuleLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class ZanRuleLayer : public cocos2d::Layer
{
public:
	ZanRuleLayer();
	~ZanRuleLayer();

	static ZanRuleLayer* create();
	bool init();
private:
	
};

#endif
