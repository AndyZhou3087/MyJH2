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

	void parseCfgFiles();

	void delayGetServerData(float dt);

	void onFinish(int errcode);

	void enterNewScene();

	void resetLoadData(float dt);

	void punishmentAction(float dt);

private:
	CREATE_FUNC(LoadingScene);

	void showPointAnim(float dt);

	void loadingSkillEffectOver(cocos2d::Texture2D* texture);
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void checkBoxCallback(cocos2d::Ref* pSender, cocos2d::ui::CheckBox::EventType);

	void loadData();

	void loadLocalData();

	void addHeroId();

	void showTips(float dt);

	void cfgFileEncryp();

	void optimizeSaveData();
private:
	bool isGetPlayerId;
	Node *m_loadingbar;
	Node* point[3];
	Node* m_loadingclicktext;
	cocos2d::ui::ImageView* m_loadingbg;
	Node* m_loadingtext;
	Label* m_wordlbl;
	cocos2d::ui::ImageView* m_userpro;
	int curEffectPlistNum;
	cocos2d::ui::Text* tipslbl;
};
extern LoadingScene* g_loadingScene;
#endif
