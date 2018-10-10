#ifndef __NpcLayer_H__
#define __NpcLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class NpcLayer : public cocos2d::Layer
{
public:
	NpcLayer();
	~NpcLayer();
	static NpcLayer* create(std::string npcid);
	virtual void onExit();
	bool init(std::string npcid);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void checkWordLblColor(std::string wordstr);
	void showTypeText(float dt);
	void showFastWords();

private:
	std::string m_npcid;
	cocos2d::ui::ScrollView* scrollView;
	Label* m_wordlbl;
	int m_wordcount;
	bool isShowWord;
	std::vector<std::string> m_wordslist;
	int m_wordindex;
	cocos2d::ui::ImageView* npc;
	cocos2d::ui::ImageView* hero;
};

#endif
