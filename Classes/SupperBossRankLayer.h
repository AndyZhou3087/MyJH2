#ifndef __SupperBossLayer_H__
#define __SupperBossLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "HttpDataSwap.h"
USING_NS_CC;

class SupperBossRankLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	SupperBossRankLayer();
	~SupperBossRankLayer();

	static SupperBossRankLayer* create(bool isclickrank = false);
	bool init(bool isclickrank = false);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);

	void clicktag(int tag);

	void getRankList(int type);//type:0--上期；type:1--本期

	void addRankNodes();

	void resetHeroHp(float dt);

private:
	cocos2d::ui::ScrollView* scrollView;
	cocos2d::ui::ImageView* title;
	cocos2d::ui::Text* tabletitle;
	int lastclicktag;
	cocos2d::ui::ImageView* tag0;
	cocos2d::ui::ImageView* tag1;
	int langtype;
	cocos2d::ui::ImageView* myrankicon;
	cocos2d::ui::Text* myrank;
	cocos2d::ui::Text* mynackname;

	cocos2d::ui::Text* mytotalhurttext;
	cocos2d::ui::Text* mycurhurtlbl;
	cocos2d::ui::Text* mytotalhurt;

	int httptag;

	int ranktype;
};

#endif
