#ifndef __GoBackLayer_LAYER_H__
#define __GoBackLayer_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class GoBackLayer : public cocos2d::Layer
{
public:
	GoBackLayer();
	~GoBackLayer();

	static GoBackLayer* create(int forwhere = 0);//默认0为回城
	virtual void onExit();
	bool init(int forwhere = 0);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int m_forwhere;
};

#endif
