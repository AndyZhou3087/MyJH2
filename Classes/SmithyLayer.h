#ifndef __SMITHY_LAYER_H__
#define __SMITHY_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
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

	void makeRes(std::string resid);

	void delayShowNewerGuide(float dt);

	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;

	void loadData();

private:
	std::vector<cocos2d::ui::Button*> vec_categoryBtn;
	std::map<int, std::vector<std::string>> map_cateRes;
	int lastCategoryindex;
	int langtype;
	cocos2d::ui::Text* lvUIlbl;
	cocos2d::ui::Text* hintlbl;
	cocos2d::ui::Widget* closebtn;

	bool clickflag;
	Vec2 beginTouchPoint;
};

#endif
