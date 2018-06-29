#ifndef __MARKET_LAYER_H__
#define __MARKET_LAYER_H__

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
}MK_RES;

class MarketLayer : public cocos2d::Layer
{
public:
	MarketLayer();
	~MarketLayer();
	static MarketLayer* create(Building* buidingData);
    bool init(Building* buidingData);

	//更新scrollview
	void updateContent();

	void lvup();

	void buyRes(int iterindex, int count);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;

	void loadData();

	void saveStockRes();

	void updateUI(float dt);

	void resetStockRes();
private:
	std::vector<MK_RES> vec_Res;
	cocos2d::ui::Text* lvUIlbl;

	cocos2d::ui::LoadingBar* timebar;
	cocos2d::ui::Text* timelbl;

	cocos2d::ui::Text* mysilverlbl;
	cocos2d::ui::Text* mycoinlbl;
};

#endif
