#ifndef __HEROATTR_LAYER_H__
#define __HEROATTR_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
#include "ResBase.h"
USING_NS_CC;

typedef enum
{
	ATTR_FIREBTN = 1000,//解雇按钮
	ATTR_CHANGEBTN,//转职按钮
	ATTR_BACKBTN,//返回按钮
	ATTR_RECRUITBTN,//招募按钮
	ATTR_LVBTN//升级按钮
}ATTRBTN;

class HeroAttrLayer : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate
{
public:
	HeroAttrLayer();
	~HeroAttrLayer();
	static HeroAttrLayer* create(Hero* herodata);
	virtual void onExit();
    bool init(Hero* herodata);

	void changeEquip(ResBase* res);

	void takeOn(ResBase* res);

	void takeOff(ResBase* res);
private:

	/************************************
	点击按钮事件
	*************************************/
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	/************************************
	点击装备事件
	*************************************/
	void onEquipClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	/************************************
	修改名字editBox控件
	*************************************/
	void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox);

	void editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action);

	void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text);

	void editBoxReturn(cocos2d::ui::EditBox *editBox);

	void updateEquipUi(ResBase* res, int barindex);

	void updataAtrrUI(float dt);
private:

	//装备栏
	Node* equipnode;
	//英雄数据
	Hero* m_heroData;
	//名字编辑控件
	cocos2d::ui::EditBox* m_editName;

	//修改PEN按钮
	cocos2d::ui::Widget* moditybtn;

	//血量label
	cocos2d::ui::Text* hplbl;

	//攻击label
	cocos2d::ui::Text* atkbl;

	//防御label
	cocos2d::ui::Text* dflbl;

	//攻击速度label
	cocos2d::ui::Text* atkspeedlbl;

	//暴击label
	cocos2d::ui::Text* critlbl;

	//闪避label
	cocos2d::ui::Text* dodgelbl;

	//点击的装备栏位置
	int clickindex;

	int langtype;
};

#endif
