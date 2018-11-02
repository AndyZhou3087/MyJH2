#ifndef _MatchRankNode_H_
#define _MatchRankNode_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class MatchRankNode :public Node
{
public:
	MatchRankNode();
	~MatchRankNode();

	bool init(MyRankData herodata, int type = 0);//0表示其他排行榜数据

	static MatchRankNode* create(MyRankData herodata, int type = 0);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	std::string getFirstHeroId();
	std::string getFirstHeroPotential();

private:
	bool clickflag;
	Vec2 beginTouchPoint;
	MyRankData m_herodata;

};
#endif

