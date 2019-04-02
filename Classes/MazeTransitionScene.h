/********************************************************************
*迷宫过渡场景
*********************************************************************/
#ifndef _MAZETRANSITION_SCENE_H_
#define _MAZETRANSITION_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

typedef enum
{
	TO_ENTER,
	TO_OUT,
	TO_EXIT
}MAZE_TYPE;

class MazeTransitionScene :public Layer
{
public:
	MazeTransitionScene();
	~MazeTransitionScene();

	virtual bool init(int mazechapter, int type);
	virtual void onEnterTransitionDidFinish();
	static Scene* createScene(int mazechapter, int type);

	virtual void onExit();

private:
	static MazeTransitionScene* create(int mazechapter, int type);

	void loadfinish(int mazechapter, int type);

	void resetMazeMap(std::string mapid);
private:
	int m_mazechapter;
	int m_type;
};
#endif

