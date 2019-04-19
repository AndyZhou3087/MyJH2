#ifndef __MatchRankLayer_H__
#define __MatchRankLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "HttpDataSwap.h"
#include "MatchRankNode.h"

USING_NS_CC;

class MatchRankLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	MatchRankLayer();
	~MatchRankLayer();

	static MatchRankLayer* create();
	bool init();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);
	void loadMyRankHeros();
	void updatelbl(float dt);
private:
	cocos2d::ui::ScrollView* scrollView;
	cocos2d::ui::Text* text;
	cocos2d::ui::Text* leftcountlbl;
	MatchRankNode* mynode;
};

#endif
