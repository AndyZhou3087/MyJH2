#ifndef __MAKERES_LAYER_H__
#define __MAKERES_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
#include "Equip.h"
USING_NS_CC;

class MakeResLayer : public cocos2d::Layer
{
public:
	MakeResLayer();
	~MakeResLayer();
	/****************************************
	@para:data 需要传递的数据
	*************************************/
	static MakeResLayer* create(void* data);
	virtual void onExit();
    bool init(void* building);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

	void changeEquip(Equip* equip);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onResClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	//更新UI,数值
	void updateUI(float dt);

	//资源足够升级
	bool checkResIsEnough();

	//操作
	void action();

	void showMakeAnim();

	void finishMakeAnim();

	void updateRndUI();

private:
	cocos2d::ui::TextBMFont* countlbl[3];//资源数量LABEL
	cocos2d::ui::Text* coincountlbl;//需要的元宝数
	void* m_data;//需要传递的数据
	std::vector<std::map<std::string, int>> vec_res;//需要的资源的数据
	DynamicValueInt costcoindv;//立即动作需要的元宝数
	Node* csbnode;
	Equip* canSelectEquip;
	cocos2d::ui::ImageView* canSelectEquipImg;

	cocos2d::ui::ImageView* canSelectEquipImgbox;
	cocos2d::ui::Text* tips;

	int originequip_qu;
	int originequip_lv;
};

#endif
