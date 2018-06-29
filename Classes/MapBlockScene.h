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

class MapBlockScene :public Layer, public ScrollViewDelegate
{
public:
	MapBlockScene();
	~MapBlockScene();

	bool init(std::string mapname);

	static cocos2d::Scene* createScene(std::string mapname);

	FightHeroNode* getFightHeroNode(int index);

private:
	static MapBlockScene* create(std::string mapname);

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onArrowKey(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void updateLabel();

	void parseMapXml(std::string mapname);

	void createMap();

	void setMyPos();

	void ajustStartPos();

	void stopMoving();

	bool checkRoad(int blockindex);

	virtual void scrollViewDidScroll(ScrollView* view);

	virtual void scrollViewDidZoom(ScrollView* view);

	void createBlackFog();

	void updateFogVisible();

	bool checkBlockVisible(int mapiter);

	void removeBlackFog(int mapiter);

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
};
extern MapBlockScene* g_MapBlockScene;
#endif

