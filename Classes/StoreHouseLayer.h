#ifndef _STOREHOUSE_LAYER_H_
#define _STOREHOUSE_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ResBase.h"
USING_NS_CC;

typedef enum
{
	CATA_0 = 0,
	CATA_1,
	CATA_2,
	CATA_3
}CATATYPE;

class StoreHouseLayer :public Layer
{
public:
	StoreHouseLayer();
	~StoreHouseLayer();

	virtual bool init();

	CREATE_FUNC(StoreHouseLayer);
	void updateContent(int category);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	Node* m_csbnode;
	void onclick(Ref* pSender);
	cocos2d::ui::ScrollView* scrollview;
	void loadData();
	static bool less_callback(ResBase* a, ResBase* b);
private:
	std::vector<cocos2d::ui::Button*> vec_categoryBtn;
	std::map<int, std::vector<ResBase*>> map_cateRes;
	int lastCategoryindex;
	int langtype;
};
#endif

