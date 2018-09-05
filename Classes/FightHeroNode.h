#ifndef _FIGHTHERO_NODE_H_
#define _FIGHTHERO_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Hero.h"
#include "Npc.h"
USING_NS_CC;

typedef enum
{
	F_HERO,
	F_NPC
}FIGHTDATA_TYPE;

typedef enum
{
	FS_READY,//准备状态，地图块界面上的
	FS_FIGHTING,//战斗中
	FS_SUCC,//成功接收界面里的
	FS_FAIL//失败结算界面里的
}FIGHTNODE_STATE;


class FightHeroNode :public Node
{
public:
	FightHeroNode();
	~FightHeroNode();

	virtual bool init();

	static FightHeroNode* create();

	void setData(Npc* data, FIGHTDATA_TYPE datatype, FIGHTNODE_STATE state);

	Npc* getData();

	void pauseTimeSchedule();

	void resumeTimeSchedule();

	void hurt(float hp, int stat = 0);

	void updateHp();

	void setFightState(int winexp);

	//发动技能
	void playSkill(int stype, FightHeroNode* whosufferNode);

	void changeSkillValue(int stype, FightHeroNode* whosufferNode);

	//遭受技能
	void attackedSkill(int stype, int myHeroPos);

	//遭受技能回调
	void attackedSkillCB(int stype, int myHeroPos);

	void recoveHp(float hp);

	void nextRound(float dt);

	void playSkillEffect(int stype);

	void playMoreSkillEffectCB(int stype, int enemyindex);

	void attackedSkillEffect(int stype, int myHeroPos);

private:
	cocos2d::ui::ImageView* headbox;
	cocos2d::ui::ImageView* headimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::LoadingBar* hp_bar;
	cocos2d::ui::LoadingBar* atkspeed_bar;
	cocos2d::ui::Widget* atkspeed_barbg;
	cocos2d::ui::ImageView* statusimg;
	cocos2d::ui::ImageView* retbox;
	cocos2d::ui::ImageView* rettext;
	cocos2d::ui::Text* winexplbl;
	cocos2d::ui::ImageView* skilltext;
	cocos2d::ui::ImageView* skilltextbox;

	cocos2d::ui::Widget* vocationbox;
	cocos2d::ui::ImageView* vocationicon;
	cocos2d::ui::TextBMFont* numfnt;
	cocos2d::ui::Widget* critnumbg;
	cocos2d::ui::Text* lvtext;
	Npc* m_Data;
	FIGHTDATA_TYPE m_datatype;
	FIGHTNODE_STATE m_state;
	float atkspeed;

	float timedt;
	bool ispause;
	float hurtup;

private:
	void onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void atkAnimFinish();
	void hurtAnimFinish();
	void update(float dt);
	void hpAnim();
	void setBlankBox();

	float getAtkSpeed();

	void reviveOnce(float hp);

	bool checkReviveSkill();

	void showAtkOrHurtAnim(int type);

	void removeAtkOrHurtAnim(float dt);

	void removePlaySkillAnim(float dt);

	void removeSufferSkillAnim(float dt);

	void resetZorder(float dt);

	void showDeathAnim();

	void removeDeathAnim(float dt);
	void HideMe(float dt);

	void delayShowReviveAnim(float dt);

	void showSkillAttrIcon(float dt);

	void refreshSkillAttrIcon(int stype);
public:
	float atkspeedbns;
	float dfbns;
	bool isPlaySkillAnim;
	std::map<int, std::vector<Sprite*>> map_skillattricon;
};
#endif

