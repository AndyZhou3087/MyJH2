#ifndef __SupperBossRuleLayer_H__
#define __SupperBossRuleLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class SimpleRuleLayer : public cocos2d::Layer
{
public:
	SimpleRuleLayer();
	~SimpleRuleLayer();

	static SimpleRuleLayer* create(std::string ruletext);
	bool init(std::string ruletext);
private:
	
};

#endif
