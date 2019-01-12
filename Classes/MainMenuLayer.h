#ifndef __MAINMENU_LAYER_H__
#define __MAINMENU_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "HttpDataSwap.h"
#include "MoreBtnNode.h"

USING_NS_CC;
//主界面按钮
typedef enum
{
	HEADBOX,//头像按钮
	R001BTN,//食物按钮
	R002BTN,//石头按钮
	R003BTN,//玄铁按钮
	R004BTN,//铁矿按钮
	R005BTN,//木材按钮
	MOREBTN,//更多按钮
	ACHBTN,//成就按钮
	RANKBTN,//排行榜按钮
	MAILBTN,//邮箱按钮
	SHOPBTN,//商场按钮
	SILVERBOX,//银子框按钮
	COINBOX,//金币狂按钮
	ADDSILVERBTN,//银子“+”按钮
	ADDCOINBTN,//金币“+”按钮
	VIP1BTN,//月卡1
	VIP2BTN,//月卡2
	TIMEGIFTBTN,//限时礼包
	TIMEGIFTBTN_0,
	TIMEGIFTBTN_1,
	TIMEGIFTBTN_2
}MENUTYPE;
class MainMenuLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	MainMenuLayer();
	~MainMenuLayer();
	CREATE_FUNC(MainMenuLayer);
	virtual void onExit();
    virtual bool init();

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

private:
	//点击按钮
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//更新UI,数值
	void updateUI(float dt);

	void delayGetServerData(float dt);

	void onFinish(int code);

public:
	void changeHead();

private:
	cocos2d::ui::Text* reslbl[5];//5个资源数量LABEL
	cocos2d::ui::Text* mysilverlbl;//银子数量LABEL
	cocos2d::ui::Text* mycoinlbl;//金币数量LABEL
	cocos2d::ui::Widget* achredpoint;//成就小红点
	cocos2d::ui::Widget* mailredpoint;//邮件小红点
	cocos2d::ui::Widget* timegiftbtn;
	cocos2d::ui::Text* timegiftlefttime;//限时礼包剩下时间

	cocos2d::ui::ImageView* head;//头像
	Node* csbnode;
	cocos2d::ui::Text* vipstrArr[2];
	cocos2d::ui::Widget* tgiftbtn[3];
	cocos2d::ui::Text* tgiftname[3];
	bool isGetVipData;
	MoreBtnNode* morebtnnode;
};
extern MainMenuLayer* g_MainMenuLayer;
#endif
