#ifndef __REPAIRBUILDING_LAYER_H__
#define __REPAIRBUILDING_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"

USING_NS_CC;

class RepairBuildingLayer : public cocos2d::Layer
{
public:
	RepairBuildingLayer();
	~RepairBuildingLayer();

	static RepairBuildingLayer* create(std::string buildingname, int type = 0);
	bool init(std::string buildingname, int type = 0);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void removeSelf();

	void upatetimelbl(float dt);

private:
	std::vector<MSGAWDSDATA> vec_repairres;
	std::string m_buildingname;
	cocos2d::ui::Text* timelbl;
	int m_type;
};

#endif
