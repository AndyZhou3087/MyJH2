#ifndef __SMALLSTALL_LAYER_H__
#define __SMALLSTALL_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
USING_NS_CC;

typedef struct
{
	std::string resid;
	int maxcount;
	int stockcount;
}SS_RES;

typedef enum
{
	SSCATA_ALL = -1,
	SSCATA_0 = 0,
	SSCATA_1,
	SSCATA_2,
	SSCATA_3,
	SSCATA_4
}SMALLSTALLCATATYPE;

class SmallStallLayer : public cocos2d::Layer
{
public:
	SmallStallLayer();
	~SmallStallLayer();
	static SmallStallLayer* create();
    bool init();

	//更新scrollview
	void updateContent(int category);


	void buyRes(int iterindex, int count);

	void delayShowUI(float dt);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;

	void loadData();

	void saveStockRes();

	void updateLabel();
private:
	std::map<int, std::vector<SS_RES>> map_cateRes;

	cocos2d::ui::Text* mysilverlbl;
	cocos2d::ui::Text* mycoinlbl;
	cocos2d::ui::Widget* closebtn;
	int langtype;
	std::vector<cocos2d::ui::Button*> vec_categoryBtn;
	int lastCategoryindex;
};

#endif
