#ifndef _MARKETRES_NODE_H_
#define _MARKETRES_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class MarketResNode :public Node
{
public:
	MarketResNode();
	~MarketResNode();

	bool init(std::string resid, int rescount);

	static MarketResNode* create(std::string resid, int rescount);

	void reset(int maxcount);
private:
	cocos2d::ui::Text* totalpricelbl;
	cocos2d::ui::ImageView* priceicon;
	cocos2d::ui::Text* rescountlbl;
	int buycount;
	int totalrescount;
	std::string m_resid;

	bool m_isLongPress;
	Node* m_longTouchNode;

	bool clickflag;
	Vec2 beginTouchPoint;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateData();
	bool checkResIsFull();

	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void addCount();
	void subCount();
};
#endif

