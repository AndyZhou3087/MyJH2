#ifndef __MAPEVENT_LAYER_H__
#define __MAPEVENT_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Const.h"
#include "GlobalInstance.h"

USING_NS_CC;

typedef enum
{
	MAP_JUMP = 0,//跳崖
	POS_CAVE,//山洞
	POS_WOMAN,//民女
	POS_ELDER,//老人
	POS_THIEF,//山贼
	POS_BET,//赌博
	POS_BUSINESS//商人
}MAPEVENTTYPE;

class MapEventLayer : public cocos2d::Layer
{
public:
	MapEventLayer();
	~MapEventLayer();

	static MapEventLayer* create(int eventindex);
	virtual void onExit();
    bool init(int eventindex);
	static std::string getDataIdByPr();//随机获取数据
	static int getEquipQuRand(std::string resid);//随机装备品质
	static int getResCountRand(std::string id);//随机数量
	void continueGamble();
	void updateCoin(float dt);

	static void loadEventData(std::string filename);
	static void loadPrData();

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void boxEventNode();
	void eventNothing();
	void eventHurt();
	static int getAllPr();//获取总概率
	static int getEventMaxQu(int maxqu);

	static bool larger_callback(EventData a, EventData b);

	void eventElderExtort(int type);
	void loadBoxUI(cocos2d::ui::ImageView* box,std::string resid);
#if MAP_BET
	void doGambling();

	void showResult(float dt);
	void openDice(float dt);
	void playGamblebox(float dt);
#endif
	void eventFight();
	void getSliverByEventW();

private:
	Node* eventnode_1;
	Node* eventnode_2;
	Node* eventnode_3;
	Node* eventnode_4;
	Node* animnode;

	cocos2d::ui::ImageView* eventimg;
	static std::vector<EventData> vec_eventdata;
	std::vector<std::vector<std::string>> vec_eventrewards;
	cocos2d::ui::Widget* betselectArr[2];
	cocos2d::ui::ImageView* betArr[2];
	cocos2d::ui::Button* betActionbtn;
	cocostudio::timeline::ActionTimeline* anim_action;
	cocos2d::ui::Widget* coin;
	cocos2d::ui::Text* coincount;
	cocos2d::ui::Button* m_closebtn;

	int langtype;
	int m_eventindex;
	int lastBetIndex;
	int winbs;
	int isWin;
	int hdcount;//事件死亡数

	DynamicValueInt betCostCoin;

	static std::map<std::string, EventData> map_eventdata;
};

#endif
