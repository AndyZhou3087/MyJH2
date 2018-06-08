/********************************************************************
*地图界面
*********************************************************************/
#ifndef _MAPBLOCK_SCENE_H_
#define _MAPBLOCK_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

typedef enum
{
	KEY_UP = 1000,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
}MAP_KEYTYPE;

typedef enum
{
	BTN_MAP = 2000,
	BTN_GOCITY,
	BTN_PACKAGE
}BTNTYPE;

class MapBlockScene :public Layer
{
public:
	MapBlockScene();
	~MapBlockScene();

	virtual bool init();

	static cocos2d::Scene* createScene();

private:
	CREATE_FUNC(MapBlockScene);

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateLabel();
private:
	cocos2d::ui::Text* carrycountlbl;
	cocos2d::ui::Text* foodcountlbl;
	cocos2d::ui::Text* solivercountlbl;
};
#endif

