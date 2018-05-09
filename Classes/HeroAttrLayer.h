#ifndef __HEROATTR_LAYER_H__
#define __HEROATTR_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

typedef enum
{
	NEWHERO,//招募新英雄
	MYHERO//已有英雄
}ENTERTYPE;

typedef enum
{
	ATTR_FIREBTN,
	ATTR_CHANGEBTN,
	ATTR_BACKBTN,
	ATTR_RECRUITBTN
}ATTRBTN;

class HeroAttrLayer : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate
{
public:
	HeroAttrLayer();
	~HeroAttrLayer();
	static HeroAttrLayer* create(ENTERTYPE etype, Hero* herodata);
	virtual void onExit();
    bool init(ENTERTYPE etype, Hero* herodata);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onEquipClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox);

	void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox);

	void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text);

	void editBoxReturn(cocos2d::ui::EditBox *editBox);
private:
	Hero* m_heroData;
	cocos2d::ui::EditBox* m_editName;
	cocos2d::ui::Text* hplbl;
	cocos2d::ui::Text* atkbl;
	cocos2d::ui::Text* dflbl;
	cocos2d::ui::Text* atkspeedlbl;
	cocos2d::ui::Text* critlbl;
	cocos2d::ui::Text* dodgelbl;
};

#endif
