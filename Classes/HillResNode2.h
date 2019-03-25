#ifndef _HILLRES_NODE2_H_
#define _HILLRES_NODE2_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResCreator.h"
USING_NS_CC;

class HillResNode2 :public Node
{
public:
	HillResNode2();
	~HillResNode2();

	bool init(int rescreatorindex);

	static HillResNode2* create(int rescreatorindex);

private:
	cocos2d::ui::ImageView* maxcaptext;//最大容量
	cocos2d::ui::Text* maxcap;//最大容量
	cocos2d::ui::ImageView* outputtext;
	cocos2d::ui::Text* output;//产出，木有已拥有的数量

	bool m_isLongPress;
	Node* m_longTouchNode;

	bool clickflag;
	Vec2 beginTouchPoint;

	ResCreator* m_resCreator;
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateData(float dt);

	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void addCount();
	void subCount();
};
#endif

