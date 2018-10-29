#ifndef __MatchRewardLayer_H__
#define __MatchRewardLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class MatchRewardLayer : public cocos2d::Layer
{
public:
	MatchRewardLayer();
	~MatchRewardLayer();

	static MatchRewardLayer* create();
	bool init();
private:

};

#endif
