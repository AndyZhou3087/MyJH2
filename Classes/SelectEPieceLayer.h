#ifndef _SELECTEPIECE_LAYER_H_
#define _SELECTEPIECE_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"
USING_NS_CC;

class SelectEPieceLayer :public Layer
{
public:
	SelectEPieceLayer();

	~SelectEPieceLayer();

	virtual bool init(Node* changeNode, int count = 1, ShopData* sdata = NULL);

	static SelectEPieceLayer* create(Node* changeNode, int count = 1, ShopData * sdata = NULL);

private:
	void onclick(Ref* pSender);
	cocos2d::ui::ScrollView* scrollview;
	void loadscrollview();

private:
	int m_count;
	Node* m_changeNode;
	ShopData* m_shopdata;
};
#endif

