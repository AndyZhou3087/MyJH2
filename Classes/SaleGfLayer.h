#ifndef __SaleGfLayer_H__
#define __SaleGfLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResBase.h"
USING_NS_CC;

class SaleGfLayer : public cocos2d::Layer
{
public:
	SaleGfLayer();
	~SaleGfLayer();

	static SaleGfLayer* create(ResBase* res);
	virtual void onExit();
	bool init(ResBase* res);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	ResBase* m_res;
};

#endif
