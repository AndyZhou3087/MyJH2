#ifndef __NpcLayer_H__
#define __NpcLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Npc.h"

USING_NS_CC;

class NpcLayer : public cocos2d::Layer
{
public:
	NpcLayer();
	~NpcLayer();
	static NpcLayer* create(std::string npcid, std::vector<Npc*> vec_enemys);
	virtual void onExit();
	bool init(std::string npcid, std::vector<Npc*> vec_enemys);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void checkWordLblColor(std::string wordstr);
	void showTypeText(float dt);
	void showFastWords();
	bool checkMutexNpc();
	bool checkNpcRelation(int relation);
	void checkEnterFight(float dt);
	void loadFriendlyPro();
	std::string checkOtherMater();
	void mutexNpcBreak();

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
	Sprite* npctalkframe;
	std::vector<Npc*> m_vec_enemys;
	int friendper;//��Ҫ�ύ�ĺøж�
	int masterper;//��Ҫ��ʦ�ĺøж�
	int marryper;//��Ҫ���׵ĺøж�
	int myfriendly;
	std::vector<int> myrelation;
	int btntag;
	cocos2d::ui::ImageView* btnArr[6];
	Node* csbnode;
	int langtype;
	cocos2d::ui::Button* closebtn;
};

#endif
