#ifndef __SMITHY_LAYER_H__
#define __SMITHY_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
#include "Equip.h"
USING_NS_CC;

typedef enum
{
	SMCATA_0 = 0,
	SMCATA_1,
	SMCATA_2,
	SMCATA_3,
	SMCATA_4
}SMCATATYPE;

class SmithyLayer : public cocos2d::Layer
{
public:
	SmithyLayer();
	~SmithyLayer();
	static SmithyLayer* create(Building* buidingData);
    bool init(Building* buidingData);

	//更新scrollview
	void updateContent(int category);

	void lvup();

	std::vector<int> makeResRnd(int originEquip_qu);

	void makeRes(std::string resid, int originEquip_qu, int originEquip_lv);

	void delayShowNewerGuide(float dt);

	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onArmsCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;
	cocos2d::ui::ScrollView* m_armcontentscroll;//武器滚动

	void loadData();

	void updateArmContent(int armcategory);

	void loadArmCataUi();

	void updateRepairTime(float dt);
	void updateRepairUi();

private:
	std::vector<cocos2d::ui::Button*> vec_categoryBtn;
	std::map<int, std::vector<std::string>> map_cateRes;
	std::map<int, std::vector<std::string>> map_catearms;
	int lastCategoryindex;
	int langtype;
	cocos2d::ui::Text* lvUIlbl;
	cocos2d::ui::Text* hintlbl;
	cocos2d::ui::Widget* closebtn;
	cocos2d::ui::Widget* armcataclick[8];
	Node* armcatacontent[8];

	bool clickflag;
	Vec2 beginTouchPoint;
	cocos2d::ui::Text* repairtimelbl;
	cocos2d::ui::Widget* repairbtn;
	Node* buildinglvbox;
	int brokenlesslv;
	bool isrepairrefresh;
	LayerColor* repaircolorlayer;
	cocos2d::Vec2 repairpos;
};

#endif
