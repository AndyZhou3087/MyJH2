#ifndef __INNROOM_LAYER_H__
#define __INNROOM_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
#include "MyHeroNode.h"
USING_NS_CC;

class InnRoomLayer : public cocos2d::Layer
{
public:
	InnRoomLayer();
	~InnRoomLayer();
	static InnRoomLayer* create(Building* buidingData);
	virtual void onExit();
    bool init(Building* buidingData);
	//更新scrollview
	void refreshMyHerosUi();

	void lvup();

	void fireHero(int index);

	MyHeroNode* getMyHeroNode(int index);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateRedPoint(float dt);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;
	cocos2d::ui::Text* lvUIlbl;
	cocos2d::ui::Widget* newHeroPoint;
	Node* csbnode;
};

#endif
