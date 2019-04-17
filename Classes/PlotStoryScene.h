/********************************************************************
* 开场场景
*********************************************************************/
#ifndef _PLOTSTORY_SCENE_H_
#define _PLOTSTORY_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

class PlotStoryScene :public Layer
{
public:
	PlotStoryScene();
	~PlotStoryScene();

	virtual bool init(int type);
	static cocos2d::Scene* createScene(int type);//0-开场动画，1-结束动画
	virtual void onEnterTransitionDidFinish();
private:
	static PlotStoryScene* create(int type);
	void showClickText(float dt);
	void delayShowNextStory(float dt);
	void showNextScene(float dt);
	void show();
private:
	Node* m_csbnode;
	Label* m_wordlbl;
	int clickcount;
	bool isCanClick;
	int m_heroindex;
	cocostudio::timeline::ActionTimeline* m_action;
	int m_type;//0-开场动画，1-结束动画
};
#endif

