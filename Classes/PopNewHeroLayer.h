#ifndef _POPNEWHERO_LAYER_H_
#define _POPNEWHERO_LAYER_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class PopNewHeroLayer :public Layer
{
public:
	PopNewHeroLayer();
	~PopNewHeroLayer();

	virtual bool init(Hero* hero);

	static PopNewHeroLayer* create(Hero* hero);

public:
private:
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* vocationtextlbl;
	cocos2d::ui::Text* vocationlbl;
	cocos2d::ui::ImageView* quimg;
	Node* light;

private:
	void delayShowLightAnim(float dt);
};
#endif

