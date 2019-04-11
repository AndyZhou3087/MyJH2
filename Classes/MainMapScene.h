/********************************************************************
*大地图界面
*********************************************************************/
#ifndef _MAINMAP_SCENE_H_
#define _MAINMAP_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class MainMapScene :public Layer, public HTTPDataDelegateProtocol
{
public:
	MainMapScene();
	~MainMapScene();

	virtual bool init();

	virtual void onExit();
	virtual void onEnterTransitionDidFinish();
	static cocos2d::Scene* createScene();

	void showNewerGuide(int step, Node* node);
	void showNewerGuideNode(int step, std::vector<Node*> nodes);

private:
	CREATE_FUNC(MainMapScene);

	/****************************
	点击地名
	*****************************/
	void onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onBoxclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateTime(float dt);
	void onFinish(int code);

private:
	int m_startClickX;
	int m_startClickY;
	bool m_isDraging;
};
#endif

