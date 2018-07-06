#ifndef __EQUIPDESCLAYER_LAYER_H__
#define __EQUIPDESCLAYER_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Equip.h"
USING_NS_CC;

typedef enum
{
	S_EQUIP_USE,
	S_EQUIP_DECOMPOSE,
	S_EQUIP_TAKEONGF,
	S_EQUIP_OTHER
}S_EQUIP_STATUS;
class EquipDescLayer : public cocos2d::Layer
{
public:
	EquipDescLayer();
	~EquipDescLayer();

	static EquipDescLayer* create(ResBase* res, int fromwhere);
	virtual void onExit();
	bool init(ResBase* res, int fromwhere);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int status;
	Equip* m_res;
};

#endif
