#ifndef __MATCHMAIN_LAYER_H__
#define __MATCHMAIN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CardHeroNode.h"
USING_NS_CC;

class MatchMainLayer : public cocos2d::Layer
{
public:
	MatchMainLayer();
	~MatchMainLayer();
	static MatchMainLayer* create();
	virtual void onExit();
    bool init();
	CardHeroNode* getMyCardHeroNode(int index);

private:
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	CardHeroNode* m_myCardHerosNode[6];

	int clickHero;
	Vec2 beginTouchPoint;
	cocos2d::ui::Text* matchlv;
	cocos2d::ui::Text* matchexp;
	cocos2d::ui::Text* needexp;
	cocos2d::ui::Text* nextlvtext;
};

#endif
