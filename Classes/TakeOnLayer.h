#ifndef _TAKEON_LAYER_H_
#define _TAKEON_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Equip.h"
#include "Hero.h"
USING_NS_CC;

class TakeOnLayer :public Layer
{
public:
	TakeOnLayer();

	~TakeOnLayer();

	virtual bool init(Equip* res_equip, Hero* herodata);

	static TakeOnLayer* create(Equip* res_equip, Hero* herodata);

	void setInStone(ResBase* stoneres, int which);

	void setOutStone(std::string stoneid, int which);
	
	void chageStone(ResBase* stoneres, int which);

	void updateAttr();

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onEquipclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onStoneclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateUI();
	void updateRedpoint(float dt);
private:
	int langtype;
	Equip* m_equip;
	Hero* m_herodata;
	Equip* nohasEquip;
	cocos2d::ui::Widget* actionbtn;

	std::vector<cocos2d::ui::Text*> vec_suitDesc;

	Node *csbnode;

	cocos2d::ui::Text* name;
	cocos2d::ui::Widget* redpoint;
	cocos2d::ui::Widget* takeoffpoint;
};
#endif

