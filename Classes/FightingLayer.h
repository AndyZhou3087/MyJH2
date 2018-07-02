#ifndef __FIGHTING_LAYER_H__
#define __FIGHTING_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "MapBlock.h"
USING_NS_CC;

class FightingLayer : public cocos2d::Layer
{
public:
	FightingLayer();
	~FightingLayer();

	static FightingLayer* create(MapBlock* mapblock);
	virtual void onExit();
	bool init(MapBlock* mapblock);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	MapBlock* m_mapblock;
};

#endif
