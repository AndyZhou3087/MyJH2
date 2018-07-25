#ifndef _WINREWARD_LAYER_H_
#define _WINREWARD_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "MapBlock.h"
#include "ResBase.h"
USING_NS_CC;

class WinRewardLayer :public Layer
{
public:
	WinRewardLayer();
	~WinRewardLayer();

	virtual bool init(std::vector<FOURProperty> reward_res);

	static WinRewardLayer* create(std::vector<FOURProperty> reward_res);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	std::vector<cocos2d::ui::ScrollView*> vec_scrollview;

	void updateScrollviewContent();

	void loadScrollviewData();

	void loadMyPackageRes();

	void addDropRes(ResBase* res);

	void reduceDropRes(ResBase* res, int count, int iteindex);

	void delayClose(float dt);

	void releaseDropRes(int interindex);

	static bool sortDropResByType(ResBase* a, ResBase* b);

	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void longTouchAction(int tag);

private:
	cocos2d::ui::Text* carrycountlbl;
	std::vector<FOURProperty> m_rewards;

	std::vector<ResBase*> vec_dropdownres;
	std::vector<ResBase*> vec_mypackagres;

	bool m_isLongPress;
	int m_longTouchTag;
};
#endif

