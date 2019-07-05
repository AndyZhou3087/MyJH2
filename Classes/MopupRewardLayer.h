#ifndef _MOPUPRWD_LAYER_H_
#define _MOPUPRWD_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ResBase.h"
#include "GlobalInstance.h"
USING_NS_CC;

class MopupRewardLayer :public Layer
{
public:
	MopupRewardLayer();
	~MopupRewardLayer();

	virtual bool init(std::vector<MSGAWDSDATA> reward_res, int perwinexp);

	static MopupRewardLayer* create(std::vector<MSGAWDSDATA> reward_res, int perwinexp);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	cocos2d::ui::ScrollView * dropscrollview;

	void updateScrollviewContent();

	void loadScrollviewData();

	static bool sortDropResByType(ResBase* a, ResBase* b);
private:

	std::vector<MSGAWDSDATA> m_rewards;

	std::vector<ResBase*> vec_dropdownres;
};
#endif

