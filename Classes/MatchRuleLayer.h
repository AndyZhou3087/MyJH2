#ifndef __MatchRuleLayer_H__
#define __MatchRuleLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class MatchRuleLayer : public cocos2d::Layer
{
public:
	MatchRuleLayer();
	~MatchRuleLayer();

	static MatchRuleLayer* create();
	bool init();
private:
	
};

#endif
