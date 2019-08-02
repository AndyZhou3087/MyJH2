#ifndef _EXCHANGEPIECE_LAYER_H_
#define _EXCHANGEPIECE_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"
USING_NS_CC;

class ExchangePieceLayer :public Layer
{
public:
	ExchangePieceLayer();

	~ExchangePieceLayer();

	virtual bool init(std::string exchangeeid, int exchangecount);

	static ExchangePieceLayer* create(std::string exchangeeid, int exchangecount);

private:
	void onclick(Ref* pSender);
	cocos2d::ui::ScrollView* scrollview;
	void loadscrollview();

private:
	int m_count;
	std::string m_exchangerid;
};
#endif

