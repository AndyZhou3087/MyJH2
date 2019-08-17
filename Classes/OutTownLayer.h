#ifndef __OUTTOWN_LAYER_H__
#define __OUTTOWN_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CardHeroNode.h"
#include "GlobalInstance.h"
USING_NS_CC;

class OutTownLayer : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate
{
public:
	OutTownLayer();
	~OutTownLayer();
	static OutTownLayer* create();
	virtual void onExit();
    bool init();
	CardHeroNode* getMyCardHeroNode(int index);

	void updateHeroCarry();

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
	void checkFormation();
private:
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	bool isCarryOver();

	void updateCaryyCountLbl();
	void longTouchUpdate(float delay);
	void cancelLongTouch();

	void addRes(Node* clicknode);
	void subRes(Node* clicknode);


	void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox);

	void editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action);

	void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text);

	void editBoxReturn(cocos2d::ui::EditBox *editBox);

	void addFormationUi();
	void onFormationClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);

	void selectFormation(int index);

	void updateFormationInfo(int index);

	void updateChangeHint(float dt);

	void takeOnFormation(int formationindex);

	static bool formationCandidateSort(Hero* a, Hero* b);

private:
	int langtype;
	CardHeroNode* m_myCardHerosNode[6];
	cocos2d::ui::Text* carrylbl;
	int caryycount;
	cocos2d::ui::Text* caryycountlbl;

	bool m_isLongPress;
	Node* m_longTouchNode;
	cocos2d::ui::Button* actionbtn;
	int clickHero;
	Vec2 beginTouchPoint;
	cocos2d::ui::Widget* changebtn;

	cocos2d::ui::EditBox* m_editCount;


	bool clickflag;
	//Vec2 beginTouchPoint;

	cocos2d::ui::ScrollView* scrollview;
	Node* formationInfoNode;
	int lastselectformation;

	std::vector<cocos2d::ui::ImageView*> vec_formationboxs;
	cocos2d::ui::Text* changehintlbl;
	int changehintindex;
	Node* carryoutherobox;

	Node* bigformation;
	cocos2d::ui::Widget* studybtn;
	cocos2d::ui::ImageView* studybtntxt;
	std::vector<Hero*> vec_formationCandidate;

	std::vector<Hero*> vec_selformation;
	static bool sortbylearned(S_FORMATION* a, S_FORMATION* b);
};

#endif
