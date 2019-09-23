#ifndef __STARRESULT_LAYER_H__
#define __STARRESULT_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
USING_NS_CC;

class StarResultLayer : public cocos2d::Layer
{
public:
	StarResultLayer();
	~StarResultLayer();

	static StarResultLayer* create(std::string mapid, int towherescene);
	bool init(std::string mapid, int towherescene);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	std::string m_mapid;
	int m_cwhere;
	cocos2d::ui::Widget* actionbtn;
	Node* besttextimg;
	void showAnim(float dt);
	void showf3starAwd(float dt);
};

#endif
