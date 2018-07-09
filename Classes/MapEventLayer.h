#ifndef __MAPEVENT_LAYER_H__
#define __MAPEVENT_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class MapEventLayer : public cocos2d::Layer
{
public:
	MapEventLayer();
	~MapEventLayer();

	static MapEventLayer* create(int eventindex);
	virtual void onExit();
    bool init(int eventindex);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
};

#endif
