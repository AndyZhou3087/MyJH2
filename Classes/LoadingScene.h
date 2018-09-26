#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "HttpDataSwap.h"
#include "UserProtocolLayer.h"

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

	void delayGetServerData(float dt);

	void onFinish(int errcode);

	void setUserProtocol(int ar);
	void enterNewScene();

private:
	CREATE_FUNC(LoadingScene);

	void showPointAnim(float dt);

	void loadingSkillEffectOver(cocos2d::Texture2D* texture);
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	bool isGetPlayerId;
	Node* point[3];
	int curEffectPlistNum;
	int protocal;
	bool loadSuccess;
};

#endif
