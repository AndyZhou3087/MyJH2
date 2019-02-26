#ifndef _EQUIPLIBRARY_LAYER_H_
#define _EQUIPLIBRARY_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"
USING_NS_CC;

typedef struct
{
	std::string strid;
	int qu;
	int type;
}SIMPLE_EQUIP;
class EquipLibraryLayer :public Layer
{
public:
	EquipLibraryLayer();
	~EquipLibraryLayer();

	virtual bool init();

	static EquipLibraryLayer* create();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onclick(Ref* pSender);

	void cataEquipData();
	void updateCata(int category);

	void addResUI(std::vector<SIMPLE_EQUIP> vec_res, Node* parent, int parentheight);

	static bool larger_EquipQu(SIMPLE_EQUIP a, SIMPLE_EQUIP b);
private:
	cocos2d::ui::ScrollView* scrollview;
	cocos2d::ui::ImageView* catetitle;
	cocos2d::ui::ImageView* equipbox;
	int lastCategoryindex;
	std::vector<cocos2d::ui::Button*> vec_categoryBtn;
	std::map<int, std::vector<SIMPLE_EQUIP>> map_resdatas;
	ResBase* res_equip;
};
#endif

