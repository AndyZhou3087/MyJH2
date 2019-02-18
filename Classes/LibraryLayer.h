#ifndef __LIBRARY_LAYER_H__
#define __LIBRARY_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class LibraryLayer : public cocos2d::Layer
{
public:
	LibraryLayer();
	~LibraryLayer();
	static LibraryLayer* create();
	virtual void onExit();
    bool init();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:

};

#endif
