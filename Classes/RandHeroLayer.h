#ifndef __RANDHERO_LAYER_H__
#define __RANDHERO_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "RandHeroNode.h"
USING_NS_CC;

typedef enum
{
	BTN_S_REFRESH,//银子刷新
	BTN_C_REFRESH,//元宝刷新
	BTN_ADD_SILVERBOX,//增加银子
	BTN_ADD_SILVER,//增加银子
	BTN_ADD_COINBOX,//增加元宝
	BTN_ADD_COIN,//增加元宝
	BTN_CLOSE//关闭按钮
}BTNTYPE;
class RandHeroLayer : public cocos2d::Layer
{
public:
	RandHeroLayer();
	~RandHeroLayer();
	static RandHeroLayer* create();
	virtual void onExit();
    bool init();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	//更新UI,数值
	void updateUI(float dt);

	//创建3个随机英雄数据
	void create3RandHero();

	//删除3个随机英雄数据
	void delete3RandHero();

private:
	cocos2d::ui::Text* mysilverlbl;//银子数量LABEL
	cocos2d::ui::Text* mycoinlbl;//金币数量LABEL

	cocos2d::ui::Text* refreshsilverlbl;//银子刷新LABEL
	cocos2d::ui::Text* refreshcoinlbl;//金币刷新LABEL
	RandHeroNode* heronode[3];//3个随机的英雄UI
	cocos2d::ui::LoadingBar* m_timebar;
	cocos2d::ui::Text* m_timelbl;
};

#endif
