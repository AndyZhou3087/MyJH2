#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;
class LoadingScene : public cocos2d::Layer
{
public:
	LoadingScene();
	~LoadingScene();
    static cocos2d::Scene* createScene();

	virtual void onExit();
    virtual bool init();

	/*************************
	显示下一个场景，
	***************************/
	void showNextScene(float dt);

	/*************************
	界面出来后加载数据，进度条，
	***************************/
	void delayLoadData(float dt);

private:
	CREATE_FUNC(LoadingScene);
};

#endif
