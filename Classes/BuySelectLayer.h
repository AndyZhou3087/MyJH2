#ifndef __BUYSELECT_LAYER_H__
#define __BUYSELECT_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class BuySelectLayer : public cocos2d::Layer
{
public:
	BuySelectLayer();
	~BuySelectLayer();

	static BuySelectLayer* create(std::vector<MSGAWDSDATA> vec_res, int putwhere = MYSTORAGE);
	bool init(std::vector<MSGAWDSDATA> vec_res, int putwhere = MYSTORAGE);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	DynamicValueInt needcoincount;
	std::vector<MSGAWDSDATA> m_vecres;
	Node* csbnode;
	void updateSelect(int index);
	int m_putwhere;
	Node* select;
};

#endif
