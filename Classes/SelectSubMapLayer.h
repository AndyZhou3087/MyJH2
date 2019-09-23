#ifndef __SELECTSUBMAP_LAYER_H__
#define __SELECTSUBMAP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

class SelectSubMapLayer : public cocos2d::Layer
{
public:
	SelectSubMapLayer();
	~SelectSubMapLayer();
	static SelectSubMapLayer* create(std::string mainmapid);
    bool init(std::string mapid);
	
	void showNewerGuide(int step, Node* node);
	void delayShowNewerGuide(float dt);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onNodeClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onAwdBoxClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void showCloudAnim(Node* target, Vec2 pos);

	void showCStarAwdUI();

	bool checkMapIsPass(std::string mapid);

	void saveGetChapterStarAwd();

private:
	std::string m_mainmapid;
	bool clickflag;
	Vec2 beginTouchPoint;
	std::vector<Node*> nodes;
	int m_step;
	bool isentermap;
	int curchapter;
	int mychapterstar;
	Node* starAwdNode;
	cocos2d::ui::ImageView* box[3];
};

#endif
