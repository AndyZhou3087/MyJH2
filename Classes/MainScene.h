#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "HttpDataSwap.h"

USING_NS_CC;
class MainScene : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	MainScene();
	~MainScene();
    static cocos2d::Scene* createScene();

	virtual void onExit();
    virtual bool init();

	virtual void onEnterTransitionDidFinish();

	/*********************************
	监听滑动事件
	**********************************/
	void srollviewlistenEvent(Ref* ref, ui::ScrollView::EventType eventType);
	
	/*********************************
	点击建筑物
	**********************************/
	void onBuildingClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	/*********************************
	更新任务图标
	**********************************/
	void updateTaskIcon();

	//新手引导
	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
	void showNewerGuideNode(int step, std::vector<Node*> nodes);

	//放开滑动功能
	void setScrollGliding();

	void showInnRoomNewHeroAnim();

private:
	cocos2d::ui::ScrollView* scroll_3;//最外层滑动层，包括训练场-仓库-铁匠铺-医馆-出城
	cocos2d::ui::ScrollView* scroll_2;//中间层滑动层，包括后山-客栈-市场
	cocos2d::ui::ScrollView* scroll_1;//最内层滑动层，包括议事厅-竞技场
private:

	CREATE_FUNC(MainScene);

	void onFinish(int code);
	
	void updateTime(float dt);

	void delayGetServerTime(float dt);

	int m_startClickX;
	int m_startClickY;
	bool m_isDraging;
	int costFoodsT;
	std::vector<Node*> vec_taskicon;
	bool isPlayNewHeroAnim;
	Node* maincityhintbox;
};
extern MainScene* g_mainScene;
#endif
