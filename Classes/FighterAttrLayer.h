#ifndef __FIGHTERATTR_LAYER_H__
#define __FIGHTERATTR_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Npc.h"

USING_NS_CC;

class FighterAttrLayer : public cocos2d::Layer
{
public:
	FighterAttrLayer();
	~FighterAttrLayer();

	static FighterAttrLayer* create(Npc* npcdata);
	virtual void onExit();
	bool init(Npc* npcdata);
	void setContentPos(Vec2 pos);
private:
	cocos2d::ui::Widget* smallbg;
};

#endif
