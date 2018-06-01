#ifndef __OUTTOWN_LAYER_H__
#define __OUTTOWN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CardHeroNode.h"
USING_NS_CC;

class OutTownLayer : public cocos2d::Layer
{
public:
	OutTownLayer();
	~OutTownLayer();
	static OutTownLayer* create();
	virtual void onExit();
    bool init();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	bool isCarryOver();

	void updateCaryyCountLbl();
private:
	CardHeroNode* m_myCardHerosNode[6];
	cocos2d::ui::Text* carrylbl;
	int caryycount[3];
	cocos2d::ui::Text* caryycountlbl[3];
};

#endif
