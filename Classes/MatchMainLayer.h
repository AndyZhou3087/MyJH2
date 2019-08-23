#ifndef __MATCHMAIN_LAYER_H__
#define __MATCHMAIN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CardHeroNode.h"
#include "HttpDataSwap.h"
#include "GlobalInstance.h"

USING_NS_CC;

class MatchMainLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	MatchMainLayer();
	~MatchMainLayer();
	static MatchMainLayer* create();
	virtual void onExit();
    bool init();
	CardHeroNode* getMyCardHeroNode(int index);

	void showFightResult(int ret);

	void delayShowFightResultLayer(float dt);

	void showFightSuccAwdLayer(float dt);

	void getMatchVsPairData(std::string playerid="");//playerid:空字串，任意匹配

	void checkFormation();
private:
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void onFinish(int code);

	void updateUI();

	void updatelbl(float dt);

	void setMatchBtnStatus(int s);

	bool checkEmptyHeros();

	void bindHeroData();


	void addFormationUi();
	void onFormationClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);

	void selectFormation(int index);

	void updateFormationInfo(int index);

	void updateChangeHint(float dt);

	void takeOnFormation(int formationindex);

	static bool formationCandidateSort(Hero* a, Hero* b);

	void updateFormationLvRes(int index);

	void upLvFormation(int index);

private:
	int langtype;
	CardHeroNode* m_myCardHerosNode[6];
	cocos2d::ui::Button* matchbtn;
	cocos2d::ui::ImageView* matchbtntxt;
	int clickHero;
	Vec2 beginTouchPoint;
	cocos2d::ui::Text* matchlv;
	cocos2d::ui::Text* matchexp;
	cocos2d::ui::Text* needexp;
	cocos2d::ui::Text* nextlvtext;
	cocos2d::ui::Text* matchwin;
	cocos2d::ui::Text* endtime;
	cocos2d::ui::Text* endtimetxt;
	cocos2d::ui::Text* leftcountlbl;
	int httptag;
	int fightret;
	bool isspecifiedfight;
	int mynormalstageformaion;

	cocos2d::ui::ScrollView* scrollview;
	Node* formationInfoNode;
	int lastselectformation;

	cocos2d::ui::ImageView* fneedres;
	cocos2d::ui::Text* fname;
	cocos2d::ui::Text* fdesc;
	std::vector<cocos2d::ui::ImageView*> vec_formationboxs;
	cocos2d::ui::Text* changehintlbl;
	int changehintindex;
	Node* carryoutherobox;

	bool clickflag;
	Node* bigformation;
	cocos2d::ui::Widget* studybtn;
	cocos2d::ui::ImageView* studybtntxt;
	std::vector<Hero*> vec_formationCandidate;

	std::vector<Hero*> vec_selformation;
	static bool sortbylearned(S_FORMATION* a, S_FORMATION* b);
};

#endif
