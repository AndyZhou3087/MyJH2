#ifndef __NpcgiveLayer_H__
#define __NpcgiveLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class NpcgiveLayer : public cocos2d::Layer
{
public:
	NpcgiveLayer();
	~NpcgiveLayer();
	static NpcgiveLayer* create(std::string npcid);
	bool init(std::string npcid);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void longTouchUpdate(float delay);
	void cancelLongTouch();
	void addRes();
	void subRes();
	void updateCaryyCountLbl();

private:
	std::string m_npcid;
	cocos2d::ui::Text* m_friendly;
	cocos2d::ui::Text* m_count;
	int lastSelectIndex;
	int lastIndexCount;
	cocos2d::ui::Widget* selectArr[3];
	Node* m_longTouchNode;
	bool m_isLongPress;
	std::vector<std::string> vec_rewards;
	cocos2d::ui::Text* desc;
};

#endif
