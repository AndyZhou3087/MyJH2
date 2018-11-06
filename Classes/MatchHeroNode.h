#ifndef _MatchHeroNode_H_
#define _MatchHeroNode_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"

USING_NS_CC;

class MatchHeroNode :public Node
{
public:
	MatchHeroNode();
	~MatchHeroNode();

	bool init(int index, std::string herostr);

	static MatchHeroNode* create(int index, std::string herostr);

private:
	cocos2d::Node* stars[5];
	Hero* hero;

private:
	//void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

