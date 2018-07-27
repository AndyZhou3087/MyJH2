#ifndef _HILLRES_NODE_H_
#define _HILLRES_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResCreator.h"
USING_NS_CC;

class HillResNode :public Node
{
public:
	HillResNode();
	~HillResNode();

	bool init(ResCreator* data);

	static HillResNode* create(ResCreator* data);

private:
	cocos2d::ui::ImageView* resimg;//资源图片
	cocos2d::ui::Text* namelbl;//名字
	cocos2d::ui::Text* maxcaptext;//最大容量
	cocos2d::ui::Text* maxcap;//最大容量
	cocos2d::ui::Text* outputtext;
	cocos2d::ui::Text* output;//产出，木有已拥有的数量
	cocos2d::ui::Text* farmercounttext;
	cocos2d::ui::Text* farmercount;//工人数量
	ResCreator* m_Data;

	bool m_isLongPress;
	Node* m_longTouchNode;

	bool clickflag;
	Vec2 beginTouchPoint;
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateData(float dt);

	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void addCount();
	void subCount();
};
#endif

