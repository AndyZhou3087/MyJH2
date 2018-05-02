#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;
class MainScene : public cocos2d::Layer
{
public:
	MainScene();
	~MainScene();
    static cocos2d::Scene* createScene();
	CREATE_FUNC(MainScene);
	virtual void onExit();
    virtual bool init();

	void srollviewlistenEvent(Ref* ref, ui::ScrollView::EventType eventType);
	
	void onBuildingClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	cocos2d::ui::ScrollView* scroll_3;
	cocos2d::ui::ScrollView* scroll_2;
	cocos2d::ui::ScrollView* scroll_1;
private:


};

#endif
