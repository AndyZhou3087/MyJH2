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

	virtual bool init(Equip* res_equip);

	static StrengthenLayer* create(Equip* res_equip);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	int langtype;
	Equip* m_equip;
	Node *csbnode;
};
#endif
