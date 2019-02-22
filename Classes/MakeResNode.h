#ifndef _MAKERES_NODE_H_
#define _MAKERES_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class MakeResNode :public Node
{
public:
	MakeResNode();
	~MakeResNode();

	bool init(std::string resid);

	static MakeResNode* create(std::string resid);

	void setEnable(bool val);

	int getResInSmithyLv();

	void updateMyOwnCountUI();

private:
	Node* csbnode;
	std::string m_resid;

	bool clickflag;
	Vec2 beginTouchPoint;
	cocos2d::ui::Text* hascount;
	cocos2d::ui::Widget* itembg;
	cocos2d::ui::Text* openlvtext;
private:
	void onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

