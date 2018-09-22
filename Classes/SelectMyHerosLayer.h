#ifndef __SELECTMYHEROS_LAYER_H__
#define __SELECTMYHEROS_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "MyHeroNode.h"
USING_NS_CC;

class SelectMyHerosLayer : public cocos2d::Layer
{
public:
	SelectMyHerosLayer();
	~SelectMyHerosLayer();
	static SelectMyHerosLayer* create(int wheretype);//1--出城选择，2-竞技场选择
	virtual void onExit();
    bool init(int wheretype);
	//更新scrollview
	void refreshMyHerosUi();
	MyHeroNode* getMyHeroNode(int index);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	cocos2d::ui::ScrollView* m_contentscroll;
	int m_wheretype;
	cocos2d::ui::Widget* closebtn;
};

#endif
