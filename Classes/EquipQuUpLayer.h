#ifndef _EQUIPQUUP_LAYER_H_
#define _EQUIPQUUP_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Equip.h"

USING_NS_CC;

class EquipQuUpLayer :public Layer
{
public:
	EquipQuUpLayer();

	~EquipQuUpLayer();

	virtual bool init(Equip* res_equip);

	static EquipQuUpLayer* create(Equip* res_equip);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onResClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	int langtype;
	Equip* m_equip;
	Node *csbnode;
	Node* boxeffectnode;
	cocos2d::ui::ImageView* resbox_qu;
	std::string needjresid;
};
#endif

