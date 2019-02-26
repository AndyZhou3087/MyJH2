#ifndef __EQUIPLIBRARYINFOLAYER_LAYER_H__
#define __EQUIPLIBRARYINFOLAYER_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
USING_NS_CC;

class EquipLibraryInfoLayer : public cocos2d::Layer
{
public:
	EquipLibraryInfoLayer();
	~EquipLibraryInfoLayer();
	virtual void onEnterTransitionDidFinish();
	static EquipLibraryInfoLayer* create(ResBase* res);
	virtual void onExit();
	bool init(ResBase* res);
	void updateAttr();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int status;
	Equipable* m_res;
	cocos2d::ui::Text* attrlblArr[6];
	cocos2d::ui::Text* namelbl;
	Node* csbnode;
};

#endif
