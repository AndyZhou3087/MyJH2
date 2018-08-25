#ifndef __NEWPOP_LAYER_H__
#define __NEWPOP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class NewPopLayer : public cocos2d::Layer
{
public:
	NewPopLayer();
	~NewPopLayer();

	static NewPopLayer* create(int para = -1);
	bool init(int para = -1);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:

};

#endif
