#ifndef _SELECTMAKEQUIP_LAYER_H_
#define _SELECTMAKEQUIP_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Equip.h"
USING_NS_CC;

class SelectMakeEquip :public Layer
{
public:
	SelectMakeEquip();

	~SelectMakeEquip();

	virtual bool init(std::string equipid);

	static SelectMakeEquip* create(std::string equipid);

private:
	void onclick(Ref* pSender);
	cocos2d::ui::ScrollView* scrollview;
	void loadData();
	void updateContent();

private:
	std::string m_equipid;
	std::vector<Equip*> vec_equips;
	int m_clickindex;
};
#endif

