#ifndef __MATCHMAIN_LAYER_H__
#define __MATCHMAIN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CardHeroNode.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class MatchMainLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	MatchMainLayer();
	~MatchMainLayer();
	static MatchMainLayer* create();
	virtual void onExit();
    bool init();
	CardHeroNode* getMyCardHeroNode(int index);

	void showFightResult(int ret);

	void delayShowFightResultLayer(float dt);

	void showFightSuccAwdLayer(float dt);

	void getMatchVsPairData(std::string playerid="");//playerid:空字串，任意匹配
private:
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onFinish(int code);

	void updateUI();

	void updateScore(float dt);

	void setMatchBtnStatus(int s);

	bool checkEmptyHeros();

	void bindHeroData();

private:
	CardHeroNode* m_myCardHerosNode[6];
	cocos2d::ui::Button* matchbtn;
	cocos2d::ui::ImageView* matchbtntxt;
	int clickHero;
	Vec2 beginTouchPoint;
	cocos2d::ui::Text* matchlv;
	cocos2d::ui::Text* matchexp;
	cocos2d::ui::Text* needexp;
	cocos2d::ui::Text* nextlvtext;
	cocos2d::ui::Text* matchwin;
	cocos2d::ui::Text* endtime;
	cocos2d::ui::Text* rewardtext;
	cocos2d::ui::Text* endtimetxt;
	cocos2d::ui::ImageView* matchrewardicon;

	int httptag;
	int fightret;
};

#endif
