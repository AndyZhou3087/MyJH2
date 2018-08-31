#ifndef __CutScenesLayer_H__
#define __CutScenesLayer_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Npc.h"
USING_NS_CC;

class CutScenesLayer : public cocos2d::Layer
{
public:
	CutScenesLayer();
	~CutScenesLayer();
	static CutScenesLayer* create(std::vector<Npc*> enemyHeros, int bgtype);
	bool init(std::vector<Npc*> enemyHeros, int bgtype);

private:
	void updateFadeOut();
	//void loadingOver(cocos2d::Texture2D* texture);
	void delayShowResult(float dt);
	
private:
	//int curReloadPlistNum;
	std::vector<Npc*> enemys;
	int m_fightbgtype;
	Node* effectnode;
};

#endif
