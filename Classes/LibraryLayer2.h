#ifndef __LIBRARY2_LAYER_H__
#define __LIBRARY2_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class LibraryLayer2 : public cocos2d::Layer
{
public:
	LibraryLayer2();
	~LibraryLayer2();
	static LibraryLayer2* create();
	virtual void onExit();
    bool init();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onBackClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	int m_startClickX;
	int m_startClickY;
	bool m_isDraging;
};

#endif
