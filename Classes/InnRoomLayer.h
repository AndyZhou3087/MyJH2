#ifndef __INNROOM_LAYER_H__
#define __INNROOM_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
USING_NS_CC;

class InnRoomLayer : public cocos2d::Layer
{
public:
	InnRoomLayer();
	~InnRoomLayer();
	static InnRoomLayer* create(Building* buidingData);
	virtual void onExit();
    bool init(Building* buidingData);
	void fireHero(int index);
	void refreshMyHerosUi();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);


private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;
};

#endif
