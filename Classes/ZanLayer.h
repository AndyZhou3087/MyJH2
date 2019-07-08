#ifndef __ZanLayer_H__
#define __ZanLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "HttpDataSwap.h"
USING_NS_CC;

class ZanLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	ZanLayer();
	~ZanLayer();

	static ZanLayer* create();
	bool init();
	void exchange(int count);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onitemBtnClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);

	void clicktag(int tag);

	void updateLefttime(float dt);

	void addAskList();

	void addPaiseRankList();

	void addToMyPraiseList();
	void updatePraiseCount(int itemindex);

	void getPraiseRankList();
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
	cocos2d::ui::Text* myzancount;
	cocos2d::ui::Text* mynackname;
	cocos2d::ui::Widget* myzancountbox;
	cocos2d::ui::Widget* j002countbox;
	cocos2d::ui::Text* j002count;
	cocos2d::ui::Text* myzanquancount;
	cocos2d::ui::Widget* actionbtn;
	cocos2d::ui::Widget* helpbtn;

	cocos2d::ui::Text* lefttime;

	int httptag;
	int paisetoplayerindex;

	ResBase* j002res;
};

#endif
