#ifndef _TAKEON_LAYER_H_
#define _TAKEON_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Equip.h"
#include "Hero.h"
USING_NS_CC;

typedef struct
{
	std::string id;
	std::vector<std::string> vec_suit;
	std::vector<float> vec_bns;
}EquipSuit;

class TakeOnLayer :public Layer
{
public:
	TakeOnLayer();

	~TakeOnLayer();

	virtual bool init(Equip* res_equip, Hero* herodata);

	static TakeOnLayer* create(Equip* res_equip, Hero* herodata);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onclick(Ref* pSender);

	void parseSuitJson();
private:
	int langtype;
	Equip* m_equip;
	std::map<std::string, EquipSuit> map_suit;
	Hero* m_herodata;
};
#endif

