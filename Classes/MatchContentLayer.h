#ifndef __MatchContentLayer_H__
#define __MatchContentLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class MatchContentLayer : public cocos2d::Layer
{
public:
	MatchContentLayer();
	~MatchContentLayer();

	static MatchContentLayer* create(MyRankData herodata);
	bool init(MyRankData herodata);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int mynormalstageformaion;
};

#endif
