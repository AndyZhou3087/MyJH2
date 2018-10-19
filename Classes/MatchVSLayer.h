#ifndef __MATCHVS_LAYER_H__
#define __MATCHVS_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class MatchVSLayer : public cocos2d::Layer
{
public:
	MatchVSLayer();
	~MatchVSLayer();

	static MatchVSLayer* create();
	bool init();
private:

	void loadData();
	void parsePairHeros();

	void delayShowFight(float dt);
private:
	Node* csbnode;
};

#endif
