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

typedef enum
{
	MKCATA_ALL = -1,
	MKCATA_0 = 0,
	MKCATA_1,
	MKCATA_2,
	MKCATA_3,
	MKCATA_4
}MARKETCATATYPE;

class MarketLayer : public cocos2d::Layer
{
public:
	MarketLayer();
	~MarketLayer();
	static MarketLayer* create(Building* buidingData);
    bool init(Building* buidingData);

	//更新scrollview
	void updateContent(int category);

	void lvup();

	void buyRes(int iterindex, int count);

	void resetStockRes();

	void delayShowUI(float dt);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;

	void loadData();

	void saveStockRes();

	void updateUI(float dt);

	void loadTimeMarket();

	int getResInMarketLv(std::string resid);
private:
	std::map<int, std::vector<MK_RES>> map_cateRes;
	cocos2d::ui::Text* lvUIlbl;

	cocos2d::ui::LoadingBar* timebar;
	cocos2d::ui::Text* timelbl;

	cocos2d::ui::Text* mysilverlbl;
	cocos2d::ui::Text* mycoinlbl;
	cocos2d::ui::Widget* closebtn;
	int langtype;
	std::vector<cocos2d::ui::Button*> vec_categoryBtn;
	int lastCategoryindex;
};

#endif
