#ifndef __SKILL_START_LAYER_H__
#define __SKILL_START_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;

class SkillStartLayer : public cocos2d::Layer
{
public:
	SkillStartLayer();
	~SkillStartLayer();

	static SkillStartLayer* create(int herovacation, int skilltype, int onstate);//onstate//在哪个场景中，正常战斗0，竞技场中1
	bool init(int herovacation, int skilltype, int onstate);
private:
	void remove(float dt);

	void showTextAnim(float dt);
private:
	bool isLeft;
	int mstype;
};

#endif
