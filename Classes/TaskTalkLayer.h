#pragma once
#pragma once
#pragma once
#ifndef _TaskTalk_LAYER_H_
#define _TaskTalk_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"
#include "Npc.h"

USING_NS_CC;

class TaskTalkLayer :public Layer
{
public:
	TaskTalkLayer();
	~TaskTalkLayer();

	virtual bool init(std::string npcid, std::vector<Npc*> vec_enemys);

	static TaskTalkLayer* create(std::string npcid, std::vector<Npc*> vec_enemys);

private:
	cocos2d::ui::Button* givebtn;
	cocos2d::ui::Button* fightbtn;
	cocos2d::ui::Button* closebtn;

	std::string m_npcid;
	std::vector<Npc*> m_vec_enemys;
	cocos2d::ui::Text* desc;
	Label* m_wordlbl;
	Node* lasttalklbl;
	bool isShowWord;
	int m_wordindex;
	int m_wordcount;
	int isGo;
	bool isFight;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onBtn2Click(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void checkWordLblColor(std::string wordstr);
	void showTypeText(float dt);

	void questGive(std::string bwords, std::vector<std::map<std::string, int>> need);
	void questFight(std::string bwords);
	void questNotFight(std::string bwords);
};
#endif

