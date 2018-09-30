#ifndef __STORY_SCENE_H__
#define __STORY_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;
class StoryScene : public cocos2d::Layer
{
public:
	StoryScene();
	~StoryScene();
    static cocos2d::Scene* createScene();

	virtual void onExit();
    virtual bool init();

	/*************************
	显示下一个场景，
	***************************/
	void showNextScene(float dt);

private:
	CREATE_FUNC(StoryScene);

	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void delayShowText(float dt);

	void textShowOver();
private:

	std::vector<Label*> vec_labels;
	int showindex;
	int wordcount;
	Label* curshowlabel;
	bool isShowOver;
	Node* loadingclicktext;
	int storylines;
	cocos2d::ui::ImageView* bg;
};

#endif
