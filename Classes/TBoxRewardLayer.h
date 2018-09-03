#ifndef __TBOXREWARD_LAYER_H__
#define __TBOXREWARD_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class TBoxRewardLayer : public cocos2d::Layer
{
public:
	TBoxRewardLayer();
	~TBoxRewardLayer();

	static TBoxRewardLayer* create(std::string resid);
	bool init(std::string resid);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:

};

#endif
