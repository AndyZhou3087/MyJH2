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
	CardHeroNode* getMyCardHeroNode(int index);

	void updateHeroCarry();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	bool isCarryOver();

	void updateCaryyCountLbl();
	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void addRes(Node* clicknode);
	void subRes(Node* clicknode);
	void onGoBuyText(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	CardHeroNode* m_myCardHerosNode[6];
	cocos2d::ui::Text* carrylbl;
	int caryycount[3];
	cocos2d::ui::Text* caryycountlbl[3];

	bool m_isLongPress;
	Node* m_longTouchNode;
};

#endif
