/********************************************************************
*大地图界面
*********************************************************************/
#ifndef _MAINMAP_SCENE_H_
#define _MAINMAP_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
class MainMapScene :public Layer
{
public:
	MainMapScene();
	~MainMapScene();

	virtual bool init();

	CREATE_FUNC(MainMapScene);


private:

	/****************************
	点击地名
	*****************************/
	void onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);



private:
	int m_startClickX;
	int m_startClickY;
	bool m_isDraging;
};
#endif

