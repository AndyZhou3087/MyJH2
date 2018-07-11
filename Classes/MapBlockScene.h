/********************************************************************
*地图界面
*********************************************************************/
#ifndef _MAPBLOCK_SCENE_H_
#define _MAPBLOCK_SCENE_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "cocos-ext.h"
#include "MapBlock.h"
#include "FightHeroNode.h"

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

	bool init(std::string mapname);

	virtual void onExit();

	static cocos2d::Scene* createScene(std::string mapname);

	FightHeroNode* getFightHeroNode(int index);

	void showFightResult(int result);

	void updateHeroUI(int which);
private:
	static MapBlockScene* create(std::string mapname);

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateLabel();

	void parseMapXml(std::string mapname);

	void createMap();

	void setMyPos();

	//视线移动到角色中心
	void ajustMyPos();

	void stopMoving();

	bool checkRoad(MAP_KEYTYPE keyArrow);

	virtual void scrollViewDidScroll(ScrollView* view);

	virtual void scrollViewDidZoom(ScrollView* view);

	void createBlackFog();

	void updateFogVisible();

	bool checkBlockVisible(int mapiter);

	void removeBlackFog(int mapiter);

	void initBlockData();
	
	void doMyStatus();

	//长按，1S以上算长按
	void longTouchUpdate(float delay);

	void cacelLongTouch();

	void go(MAP_KEYTYPE keyArrow);

	void createRndMonsters();

	void creatNpcOrBoss(MapBlock* mbolck);

private:
	Node* m_csbnode;
	ParticleSystemQuad* myposParticle;
	cocos2d::ui::Text* carrycountlbl;
	cocos2d::ui::Text* foodcountlbl;
	cocos2d::ui::Text* solivercountlbl;
	int blockRowCount;
	int blockColCount;
	ScrollView* scrollView;
	Node *m_mapscrollcontainer;
	std::map<int, MapBlock*> map_mapBlocks;
	std::map<int, Sprite*> map_mapFogBlacks;
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
};
extern MapBlockScene* g_MapBlockScene;
#endif

