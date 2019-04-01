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
	BTN_GOCITY = 2000,
	BTN_PACKAGE,
	BTN_TORCH,
	BTN_VISION,
	BTN_EXITMAZE,
	BTN_ALLOPEN,
	BTN_TRANS,
	BTN_HIDE
}MAPBTNTYPE;

typedef enum
{
	MAP_S_NOTING = 0,
	MAP_S_EVENT,
	MAP_S_FIGHT
}MAP_STATUS;

typedef struct
{
	int maid;
	int blockindex;
}MAZE_POS;

typedef enum
{
	MAP_USEINGTORCH = 0,//使用火把中
	MAP_USEINGTRANSER//使用传送卷轴
}MAP_USINGPROP;

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

	void showUnlockChapter();

	void delayShowExit(float dt);

	void goBackMainHomeScene();

	//延迟新手引导
	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
	void showNewerGuideNode(int step, std::vector<Node*> nodes);
	void showNewerGuideFight();
	void showNewerGuideGoBack();
	//引导第一次打败boss
	bool getFirstFightBoss();
	//获取是否移动//针对新手引导
	bool getIsMoving();

	void eventFight();

	void removeMazeStone(int blockindex);

	void removeMazeStoneAfterAnim(int blockindex);

	void useAllOpen();

	void useTranser();

private:
	static MapBlockScene* create(std::string mapname, int bgtype);
	virtual void onEnterTransitionDidFinish();
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onTaskAction(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onBlockClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onUsePropClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateLabel(float dt);

	void parseMapXml(std::string mapname);

	void createMap();

	void setMyPos();

	//视线移动到角色中心
	void ajustMyPos(bool isanim = true);

	void stopMoving();

	int checkRoad(MAP_KEYTYPE keyArrow);

	virtual void scrollViewDidScroll(ScrollView* view);

	virtual void scrollViewDidZoom(ScrollView* view);

	void createFog();

	void updateFog(float dt);

	void addFogBlock(int row, int col, int scale);

	void resetBlockData();
	
	void doMyStatus();

	//长按，1S以上算长按
	void longTouchUpdate(float delay);

	void cacelLongTouch();

	void go(MAP_KEYTYPE keyArrow);

	void createRndMonsters();

	void configRndMonstersAndRewards(int mbindex);

	void creatNpcOrBoss(MapBlock* mbolck);

	void createBoxRewards(MapBlock* mbolck);

	int getWinExp();

	void createMyRender();

	void updateMyRender(float dt);

	void checkFood();

	bool checklive();

	void loadTaskUI();

	void updateTaskInfo(float dt);

	void delayShowFightResult(float dt);

	void setMapOrderData();

	void openAllMap();

	void closeTaskTipNode(float dt);

	void createEventMonsters();

	void setBtnEnable(bool isval);

	void changeTaskTipTextColor(int type);//0--主线，1--支线

	void showThrowTorchParticle(int row, int col);

	void torchLight(int row, int col);

	void heroDelayTranse(float dt);

	void ajustStatus();

	void updateZ002Count(float dt);

	void allOpenAnim(float dt);

	int checkCanTrans(int blockindex);

	void showTransFoodDesc(int foodcount);

	void delaySetMyPos(float dt);

	static bool sortByBoxPos(MapBlock* a, MapBlock *b);

	void setBlockRange();

	void delayShowMazeHint(float dt);

private:
	Node* m_csbnode;
	Node* m_tasknode;
	cocos2d::ui::Text* mapnamelbl;
	spine::SkeletonAnimation* myposHero;
	cocos2d::ui::Text* carrycountlbl;
	cocos2d::ui::Text* foodcountlbl;
	cocos2d::ui::Text* solivercountlbl;
	cocos2d::ui::Text* sitelbl;
	cocos2d::ui::ImageView* taskclick;
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

	std::map<int, std::vector<Npc*>> map_allRndMonsters;
	std::vector<Npc*> vec_enemys;
	std::vector<FOURProperty> vec_winrewards;
	RenderTexture* _fogrender;
	RenderTexture* _myrender;
	Sprite* _mylight;
	int fogscale;

	int m_fightbgtype;
	
	int m_walkDirection;
	int m_lastWalkDirection;

	bool mapIsAllOpen;

	cocos2d::ui::Widget* keybtnArr[4];
	cocos2d::ui::Widget* gocitybtn;
	cocos2d::ui::Widget* torchbtn;
	cocos2d::ui::Widget* mypackagebtn;
	cocos2d::ui::Widget* visionbtn;
	cocos2d::ui::Widget* transbtn;

	std::vector<MapBlock*> vec_normalBlocks;
	std::vector<MapBlock*> vec_monsterBlocks;

	int firstpostype;

	bool isNewerGuideMap;
	int usefood;

	cocos2d::ui::TextBMFont* visioncountlbl;
	cocos2d::ui::TextBMFont* torchcountlbl;
	int usingprop;//正在使用的道具
	bool isDraging;
	bool isBlockClickCancel;
	bool isMaze;

	std::vector<MAZE_POS> vec_mazetranspoints;
	int max_mazepos;
	cocos2d::ui::ImageView* buildfocus;
	Label* z002countlbl;
	cocos2d::ui::Text* z002countlbl_1;
	Node* propbox;

	float mapFogScale;

	Sprite* mapAllOpenFog;
	std::vector<MapBlock*> vec_boxblock;//宝箱地图块
	std::map<int, std::vector<MapBlock*>> map_rangeblocks;
};
extern MapBlockScene* g_MapBlockScene;
#endif

