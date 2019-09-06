#ifndef _Strengthen_LAYER_H_
#define _Strengthen_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Equip.h"

USING_NS_CC;

class StrengthenLayer :public Layer
{
public:
	StrengthenLayer();

	~StrengthenLayer();

	virtual bool init(Equip* res_equip, int forwhere);

	static StrengthenLayer* create(Equip* res_equip, int forwhere);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onResClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateSuccRnd();
private:
	int langtype;
	Equip* m_equip;
	Node *csbnode;
	int m_forwhere;
	DynamicValueInt coincount;
	Node* boxeffectnode;
	cocos2d::ui::ImageView* resbox_qu;
	cocos2d::ui::ImageView* res4img;
	cocos2d::ui::Text* res4lbl;
	DynamicValueInt luckcount;
};
#endif

