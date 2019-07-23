#ifndef __SupperBossRuleLayer_H__
#define __SupperBossRuleLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class SupperBossRuleLayer : public cocos2d::Layer
{
public:
	SupperBossRuleLayer();
	~SupperBossRuleLayer();

	static SupperBossRuleLayer* create();
	bool init();
private:
	
};

#endif
