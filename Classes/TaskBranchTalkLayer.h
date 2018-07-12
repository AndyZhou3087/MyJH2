#pragma once
#pragma once
#pragma once
#ifndef _TaskBranchTalk_LAYER_H_
#define _TaskBranchTalk_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"
#include "Npc.h"

USING_NS_CC;

class TaskBranchTalkLayer :public Layer
{
public:
	TaskBranchTalkLayer();
	~TaskBranchTalkLayer();

	virtual bool init(std::string npcid, std::vector<Npc*> vec_enemys);

	static TaskBranchTalkLayer* create(std::string npcid, std::vector<Npc*> vec_enemys);

private:
	cocos2d::ui::Button* givebtn;
	cocos2d::ui::Button* fightbtn;
	cocos2d::ui::Button* closebtn;

	std::string m_npcid;
	std::vector<Npc*> m_vec_enemys;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

};
#endif

