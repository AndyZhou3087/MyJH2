#ifndef _SELECTEQUIP_LAYER_H_
#define _SELECTEQUIP_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ResBase.h"
USING_NS_CC;

class SelectEquipLayer :public Layer
{
public:
	SelectEquipLayer();

	~SelectEquipLayer();

	virtual bool init(int restype);

	static SelectEquipLayer* create(int restype);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onclick(Ref* pSender);
	cocos2d::ui::ScrollView* scrollview;
	void loadData();
	void updateContent();
private:
	int m_restype;
	std::vector<ResBase*> vec_res;
};
#endif

