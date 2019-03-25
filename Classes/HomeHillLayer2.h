#ifndef __HOMEHILL2_LAYER_H__
#define __HOMEHILL2_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
USING_NS_CC;

class HomeHillLayer2 : public cocos2d::Layer
{
public:
	HomeHillLayer2();
	~HomeHillLayer2();
	static HomeHillLayer2* create(Building* buidingData);
	virtual void onExit();
    bool init(Building* buidingData);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onBuildClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateUI(float dt);
private:
	Building* m_buidingData;//建筑物数据
	cocos2d::ui::ScrollView* m_contentscroll;
	cocos2d::ui::Text* m_idlefarmercount;
	Node* csbnode;
	int m_startClickX;
	int m_startClickY;
	bool m_isDraging;
	Node* m_guideResNode;
	cocos2d::ui::Widget* actionbtn;
};

#endif
