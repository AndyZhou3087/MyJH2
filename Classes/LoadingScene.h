#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "HttpDataSwap.h"

USING_NS_CC;
class LoadingScene : public cocos2d::Layer, public HTTPDataDelegateProtocol
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
	void delayLoadLocalData(float dt);

	void delayLoadServerData(float dt);

	void onFinish(int errcode);

private:
	CREATE_FUNC(LoadingScene);

private:
	bool isGetPlayerId;
};

#endif
