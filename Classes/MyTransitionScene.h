/********************************************************************
*过渡场景
*********************************************************************/
#ifndef _MYTRANSITION_SCENE_H_
#define _MYTRANSITION_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

typedef enum
{
	TO_MAP,
	TO_MAIN,
	TO_MAPBLOCK
}TRAN_TYPE;

class MyTransitionScene :public Layer
{
public:
	MyTransitionScene();
	~MyTransitionScene();

	virtual bool init(int type);

	virtual void onExit();

	static cocos2d::Scene* createScene(int type);

private:
	static MyTransitionScene* create(int type);

	void updateHorse(float dt);

	void loadfinish(int type);
private:
	Node* horse;
	cocos2d::ui::LoadingBar* loadingbar;
	bool ischanging;
};
#endif

