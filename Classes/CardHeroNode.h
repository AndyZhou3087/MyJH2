#ifndef _CARDHERO_NODE_H_
#define _CARDHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
USING_NS_CC;

class CardHeroNode :public Node
{
public:
	CardHeroNode();
	~CardHeroNode();

	virtual bool init();

	static CardHeroNode* create();

	void setData(Hero* herodata);

	CC_SYNTHESIZE(bool, m_isdraging, IsDrading);//是否拖动中

	void updateSelPosLbl();

private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* powertext;
	cocos2d::ui::Text* lvlbl;
	cocos2d::ui::Text* desclbl;
	cocos2d::ui::Widget* vocationbox;
	cocos2d::ui::ImageView* vocationicon;
	cocos2d::ui::ImageView* powericon;
	cocos2d::Node* stars[5];
	cocos2d::Node* cardnamebox;
	Hero* m_herodata;
	cocos2d::ui::Widget* arrowglv;
	cocos2d::ui::ImageView* powerclick;
	int lastvaction;

private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onPowerClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updatePowerCount(float dt);
	void updateIcon(float dt);
	void updateVocationData();
};
#endif

