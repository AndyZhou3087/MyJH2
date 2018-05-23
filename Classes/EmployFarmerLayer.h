#ifndef __EMPLOYFARMER_LAYER_H__
#define __EMPLOYFARMER_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "DynamicValue.h"
USING_NS_CC;

class EmployFarmerLayer : public cocos2d::Layer
{
public:
	EmployFarmerLayer();
	~EmployFarmerLayer();
	static EmployFarmerLayer* create();
	virtual void onExit();
    bool init();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//更新UI,数值
	void updateUI(float dt);

private:
	cocos2d::ui::Text* needlbl;//
	cocos2d::ui::Text* lacklbl;//
	cocos2d::ui::Text* countlbl;
	DynamicValueInt dint;//需要的食物数量
};

#endif
