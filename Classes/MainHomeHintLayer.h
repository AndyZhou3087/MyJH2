#ifndef __MAINHOMEHINT_LAYER_H__
#define __MAINHOMEHINT_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class MainHomeHintLayer : public cocos2d::Layer
{
public:
	MainHomeHintLayer();
	~MainHomeHintLayer();

	static MainHomeHintLayer* create(int typ);
	bool init(int type);
private:
	void removeSelf();

private:

};

#endif
