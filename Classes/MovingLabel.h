#ifndef _MOVING_LBAEL_H_
#define _MOVING_LBAEL_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class MovingLabel :public Sprite
{
public:
	MovingLabel();
	~MovingLabel();


	static void show(std::string text, Color4B color = Color4B(Color3B(255, 229, 188)), Vec2 pos = Vec2(360, 640), Node* parent = NULL);
	static void showbyNode(Node* node, std::string text, Color4B color = Color4B(Color3B::WHITE), Vec2 pos = Vec2(0, 0));

private:
	bool init(std::string text, Color4B color, Vec2 pos);

	static MovingLabel* create(std::string text, Color4B color, Vec2 pos);
	void showAction();
	void removeSelf();

	void showNext();
	void changeTextColor();
private:
	static std::queue<MovingLabel*> queue_labels;
	Label* mlbl;
	static Node* lastRunningScene;
};
#endif

