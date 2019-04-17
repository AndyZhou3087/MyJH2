#ifndef __STARDESC_LAYER_H__
#define __STARDESC_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
USING_NS_CC;

class StarDescLayer : public cocos2d::Layer
{
public:
	StarDescLayer();
	~StarDescLayer();

	static StarDescLayer* create(std::string mapid);
	bool init(std::string mapid);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	static bool sortbyfinishstat(S_StarData a, S_StarData b);
	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
private:
	std::string m_mapid;
	cocos2d::ui::Widget* actionbtn;
};

#endif
