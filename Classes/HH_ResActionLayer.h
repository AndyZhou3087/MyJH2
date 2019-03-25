#ifndef __HH_RES_ACTION_LAYER_H__
#define __HH_RES_ACTION_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "DynamicValue.h"
#include "ResCreator.h"
USING_NS_CC;

typedef enum
{
	HH_EMPLOYFARMER,//雇用工人
	HH_RESCREATORLVUP//升级资源容量
}HH_ACTIONTYPE;
class HH_ResActionLayer : public cocos2d::Layer
{
public:
	HH_ResActionLayer();
	~HH_ResActionLayer();
	/****************************************
	@para:data 需要传递的数据
	@para:actiontype HH_ACTIONTYPE
	*************************************/
	static HH_ResActionLayer* create(ResCreator* rcreator, HH_ACTIONTYPE actiontype);
	virtual void onExit();
    bool init(ResCreator* rcreator, HH_ACTIONTYPE actiontype);

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	//更新UI,数值
	void updateLabelColor(float dt);

	void updateLabelCount();
	//操作
	void action();

	void showNextLvUi();

private:
	cocos2d::ui::Text* coincountlbl;//需要的元宝数
	cocos2d::ui::Text* neddrescountlbl;//需要的元宝数
	int m_actiontype;//CACTIONTYPE
	DynamicValueInt costcoindv;//立即动作需要的元宝数
	Node* csbnode;
	ResCreator* m_rescreator;
	DynamicValueInt costresdv;//需要资源数
	std::string needresid;
	cocos2d::ui::LoadingBar* maxcapbar1;
	cocos2d::ui::LoadingBar* maxcapbar;

	cocos2d::ui::LoadingBar* mafmbar1;
	cocos2d::ui::LoadingBar* mafmbar;
	cocos2d::ui::Text* maxcapval;
	cocos2d::ui::Text* storeval;
};

#endif
