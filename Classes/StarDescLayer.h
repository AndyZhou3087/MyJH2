#ifndef __STARDESC_LAYER_H__
#define __STARDESC_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "HttpDataSwap.h"
USING_NS_CC;

class StarDescLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	StarDescLayer();
	~StarDescLayer();

	static StarDescLayer* create(std::string mapid);
	bool init(std::string mapid);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onresClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
	static bool sortbyfinishstat(S_StarData a, S_StarData b);
	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

	void showMopUpRwd();
	void showSupperBossRwd();

	void mopUpAction();

	void updatelabel(float dt);

	bool checkMopupHeroPower();

	void onFinish(int errcode);
private:
	std::string m_mapid;
	cocos2d::ui::Widget* actionbtn;
	Node* mopupnode;
	cocos2d::ui::Text* mopupleftcountlbl;
	cocos2d::ui::Text* j003leftcountlbl;
	cocos2d::ui::Text* packagefoodcountlbl;
	cocos2d::ui::Text* supperbossleftcountlbl;
	cocos2d::ui::Text* supperbosstimelbl;
};

#endif
