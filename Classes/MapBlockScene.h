/********************************************************************
*地图界面
*********************************************************************/
#ifndef _MAPBLOCK_SCENE_H_
#define _MAPBLOCK_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "cocos-ext.h"
#include <spine/spine-cocos2dx.h>
#include "spine/spine.h"
#include "MapBlock.h"
#include "FightHeroNode.h"
#include "GlobalInstance.h"

USING_NS_CC;
USING_NS_CC_EXT;

typedef enum
{
	KEY_UP = 1000,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
}MAP_KEYTYPE;

typedef enum
{
	BTN_MAP = 2000,
	BTN_GOCITY,
	BTN_PACKAGE
}BTNTYPE;

typedef enum
{
	MAP_S_NOTING = 0,
	MAP_S_EVENT,
	MAP_S_FIGHT
}MAP_STATUS;

class MapBlockScene :public Layer, public ScrollViewDelegate
{
public:
	MapBlockScene();
	~MapBlockScene();

	bool init(std::string mapname, int bgtype);

	virtual void onExit();

	static cocos2d::Scene* createScene(std::string mapname, int bgtype);

	FightHeroNode* getFightHeroNode(int index);

	void showFightResult(int result);

	void updateHeroUI(int which);

	void showFightingLayer(std::vector<Npc*> enemys);

private:
	static MapBlockScene* create(std::string mapname, int bgtype);

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onTaskAction(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateLabel(float dt);

	void parseMapXml(std::string mapname);

	void createMap();

	void setMyPos();

	//视线移动到角色中心
	void ajustMyPos();

	void stopMoving();

	bool checkRoad(MAP_KEYTYPE keyArrow);

	virtual void scrollViewDidScroll(ScrollView* view);

	virtual void scrollViewDidZoom(ScrollView* view);

	void createFog();

	void updateFog(float dt);

	void addFogBlock(int row, int col);

	void resetBlockData();
	
	void doMyStatus();

	//长按，1S以上算长按
	void longTouchUpdate(float delay);

	void cacelLongTouch();

	void go(MAP_KEYTYPE keyArrow);

	void createRndMonsters();

	void creatNpcOrBoss(MapBlock* mbolck);

	int getWinExp();

	void createMyRender();

	void updateMyRender(float dt);

	void checkFood();

	bool checklive();

	void loadTaskUI();

	void updateTaskInfo(float dt);

	void delayShowFightResult(float dt);

private:
	Node* m_csbnode;
	Node* m_tasknode;

	spine::SkeletonAnimation* myposHero;
	cocos2d::ui::Text* carrycountlbl;
	cocos2d::ui::Text* foodcountlbl;
	cocos2d::ui::Text* solivercountlbl;
	cocos2d::ui::Text* sitelbl;
	cocos2d::ui::ImageView* questbtn;
	cocos2d::ui::Text* textmain;
	cocos2d::ui::Text* textbranch;

	int blockRowCount;
	int blockColCount;
	ScrollView* scrollView;
	Node *m_mapscrollcontainer;
	std::map<int, MapBlock*> map_mapBlocks;
	std::vector<int> vec_startpos;
	int mycurCol;
	int mycurRow;
	bool isMoving;
	bool m_isLongPress;
	Node* m_longTouchNode;
	int randStartPos;
	std::string m_mapid;
	int walkcount;
	int monsterComeRnd;

	std::vector<Npc*> vec_enemys;
	std::vector<FOURProperty> vec_winrewards;
	RenderTexture* _fogrender;
	RenderTexture* _myrender;
	Sprite* _mylight;
	float fogscale;

	int m_fightbgtype;
	
	int m_walkDirection;
	int m_lastWalkDirection;
};
extern MapBlockScene* g_MapBlockScene;
#endif

