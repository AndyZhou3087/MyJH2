#ifndef __MAZEDESC_LAYER_H__
#define __MAZEDESC_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class MazeDescLayer : public cocos2d::Layer
{
public:
	MazeDescLayer();
	~MazeDescLayer();

	static MazeDescLayer* create();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onclick(Ref* pSender);
private:

};

#endif
