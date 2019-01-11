#ifndef __M1_5_BOX_LAYER_H__
#define __M1_5_BOX_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class M1_5_BoxLayer : public cocos2d::Layer
{
public:
	M1_5_BoxLayer();
	~M1_5_BoxLayer();

	static M1_5_BoxLayer* create();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void removeSelf();

private:

};

#endif
