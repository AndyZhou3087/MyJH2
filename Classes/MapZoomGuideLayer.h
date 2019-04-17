
#ifndef _MAPZOOMGUIDE_LAYER_H_
#define _MAPZOOMGUIDE_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

class MapZoomGuideLayer :public Layer
{
public:
	MapZoomGuideLayer();
	~MapZoomGuideLayer();
	static MapZoomGuideLayer* create();
	virtual bool init();
private:

private:

};
#endif

