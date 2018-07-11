#ifndef __SETINSTONE_LAYER_H__
#define __SETINSTONE_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResBase.h"
#include "Hero.h"
USING_NS_CC;

class SetInStoneLayer : public cocos2d::Layer
{
public:
	SetInStoneLayer();
	~SetInStoneLayer();

	static SetInStoneLayer* create(ResBase* res, int which, Hero* herodata);
	virtual void onExit();
	bool init(ResBase* res, int which, Hero* herodata);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	ResBase* m_res;
	Hero* m_herodata;
};

#endif
