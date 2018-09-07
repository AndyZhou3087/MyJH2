#ifndef _SELECTEQUIP_LAYER_H_
#define _SELECTEQUIP_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ResBase.h"
#include "Hero.h"
USING_NS_CC;

class SelectEquipLayer :public Layer
{
public:
	SelectEquipLayer();

	~SelectEquipLayer();

	virtual bool init(int restype, Hero* herodata);

	static SelectEquipLayer* create(int restype, Hero* herodata);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onclick(Ref* pSender);
	cocos2d::ui::ScrollView* scrollview;
	void loadData();
	void updateContent();
private:
	int m_restype;
	std::vector<ResBase*> vec_res;
	Hero* m_herodata;
};
#endif

