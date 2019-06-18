
#ifndef __TRAINFINISH_LAYER_H__
#define __TRAINFINISH_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class TrainFinishLayer : public cocos2d::Layer
{
public:
	TrainFinishLayer();
	~TrainFinishLayer();
	static TrainFinishLayer* create();
	bool init();


private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateContent();
private:
	cocos2d::ui::ScrollView* m_contentscroll;

};

#endif
