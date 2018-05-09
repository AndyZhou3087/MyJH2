#ifndef _RANDHERO_NODE_H_
#define _RANDHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class RandHeroNode :public Node
{
public:
	RandHeroNode();
	~RandHeroNode();

	virtual bool init();

	CREATE_FUNC(RandHeroNode);

private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* vocationtextlbl;
	cocos2d::ui::Text* vocationlbl;
	cocos2d::ui::Text* potentialtextlbl;
	cocos2d::ui::Text* potentiallbl;
	Hero* heroData;
private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void generate();
	std::string generateName();
};
#endif

