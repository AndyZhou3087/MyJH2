#ifndef _FLOWWORLD_H_
#define _FLOWWORLD_H_
#include "cocos2d.h"

USING_NS_CC;

class FlowWorld :public Node
{
protected:
	Node* _holder;
	Sprite* _newsBg;
	Label* _news;
	DrawNode* _shap;
	ClippingNode* _cliper;

public:

	static FlowWorld* fWorld;
	static FlowWorld* getIntance();

	FlowWorld* createFW(Sprite* sp);

	FlowWorld();
	~FlowWorld();
private:

	void nextWord(std::string str);
	void scrollText(float);

	void changeTextColor(std::string str);

	CREATE_FUNC(FlowWorld);
};
#endif