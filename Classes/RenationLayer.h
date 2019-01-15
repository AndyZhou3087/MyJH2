#ifndef __RENATION_LAYER_H__
#define __RENATION_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class RenationLayer : public cocos2d::Layer
{
public:
	RenationLayer();
	~RenationLayer();

	static RenationLayer* create();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void createPage();
private:
	void updateNpcAddAttr(std::string npcid);
	cocos2d::ui::PageView* pageView;
	std::vector<std::string> vec_npc;
	cocos2d::ui::Text* npcattrlbl[6];
	cocos2d::ui::Text* npcname;
	Node* lastClickNpcItem;
	bool clickflag;
	Vec2 beginTouchPoint;
};

#endif
