#ifndef __WIN_LAYER_H__
#define __WIN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "MapBlock.h"
USING_NS_CC;

class WinLayer : public cocos2d::Layer
{
public:
	WinLayer();
	~WinLayer();

	static WinLayer* create(std::vector<FOURProperty> reward_res, int winexp);
	virtual void onExit();
	bool init(std::vector<FOURProperty> reward_res, int winexp);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
};

#endif
