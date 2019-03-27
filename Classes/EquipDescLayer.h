#ifndef __EQUIPDESCLAYER_LAYER_H__
#define __EQUIPDESCLAYER_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Equipable.h"
USING_NS_CC;

typedef enum
{
	S_NONE = 0,
	S_EQUIP_SEL,
	S_EQUIP_DECOMPOSE,
	S_EQUIP_TAKEOFF,
	S_EQUIP_WGLV,
	S_EQUIP_STENGTH
}S_EQUIP_STATUS;
class EquipDescLayer : public cocos2d::Layer
{
public:
	EquipDescLayer();
	~EquipDescLayer();
	virtual void onEnterTransitionDidFinish();
	static EquipDescLayer* create(ResBase* res, int fromwhere);
	virtual void onExit();
	bool init(ResBase* res, int fromwhere);
	void updateAttr();

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int status;
	Equipable* m_res;
	cocos2d::ui::Text* attrlblArr[6];
	cocos2d::ui::Text* addtextArr[6];
	cocos2d::ui::Text* namelbl;
	Node* csbnode;
	cocos2d::ui::Widget* redpoint;
	cocos2d::ui::Widget* salepoint;
	cocos2d::ui::ImageView* resbox_qu;
	Node* boxeffect;
};

#endif
