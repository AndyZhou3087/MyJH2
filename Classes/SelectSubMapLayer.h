#ifndef __SELECTSUBMAP_LAYER_H__
#define __SELECTSUBMAP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class SelectSubMapLayer : public cocos2d::Layer
{
public:
	SelectSubMapLayer();
	~SelectSubMapLayer();
	static SelectSubMapLayer* create(std::string mapid);
    bool init(std::string mapid);
	
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onNodeClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
};

#endif
