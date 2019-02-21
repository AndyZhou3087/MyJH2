#ifndef __GambleBoxLayer_H__
#define __GambleBoxLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Const.h"

USING_NS_CC;
#if MAP_BET
class GambleBoxLayer : public cocos2d::Layer
{
public:
	GambleBoxLayer();
	~GambleBoxLayer();

	static GambleBoxLayer* create(int isWin, int diceid, int cost);
	bool init(int isWin, int diceid, int cost);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif
#endif
