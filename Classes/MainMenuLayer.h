#ifndef __MAINMENU_LAYER_H__
#define __MAINMENU_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;
typedef enum
{
	HEADBOX,
	R001BTN,
	R002BTN,
	R003BTN,
	R004BTN,
	R005BTN,
	SETBTN,
	ACHBTN,
	RANKBTN,
	MAILBTN,
	SHOPBTN,
	SILVERBOX,
	COINBOX,
	ADDSILVERBTN,
	ADDCOINBTN
}MENUTYPE;
class MainMenuLayer : public cocos2d::Layer
{
public:
	MainMenuLayer();
	~MainMenuLayer();
	CREATE_FUNC(MainMenuLayer);
	virtual void onExit();
    virtual bool init();

private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateUI(float dt);
private:
	cocos2d::ui::Text* reslbl[5];
	cocos2d::ui::Text* silverlbl;
	cocos2d::ui::Text* coinlbl;
	cocos2d::ui::Widget* achredpoint;
	cocos2d::ui::Widget* mailredpoint;
};

#endif
