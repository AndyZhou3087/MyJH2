#ifndef _HILLRES_NODE_H_
#define _HILLRES_NODE_H_
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResCreator.h"
USING_NS_CC;

class HillResNode :public Node
{
public:
	HillResNode();
	~HillResNode();

	bool init(ResCreator* data);

	static HillResNode* create(ResCreator* data);

private:
	cocos2d::ui::ImageView* resimg;
	cocos2d::ui::Text* namelbl;
	cocos2d::ui::Text* maxcaptext;
	cocos2d::ui::Text* maxcap;
	cocos2d::ui::Text* outputtext;
	cocos2d::ui::Text* output;
	cocos2d::ui::Text* farmercounttext;
	cocos2d::ui::Text* farmercount;
	ResCreator* m_Data;
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateData(float dt);
};
#endif

