#ifndef __BUILDINGLVUP_LAYER_H__
#define __BUILDINGLVUP_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
USING_NS_CC;

typedef enum
{
	CA_BUILDINGLVUP,//建筑物升级
	CA_EMPLOYFARMER,//雇用工人
	CA_RESCREATORLVUP//升级资源容量
}CACTIONTYPE;
class ConsumeResActionLayer : public cocos2d::Layer
{
public:
	ConsumeResActionLayer();
	~ConsumeResActionLayer();
	/****************************************
	@para:data 需要传递的数据
	@para:actiontype CACTIONTYPE
	*************************************/
	static ConsumeResActionLayer* create(void* data, int actiontype);
	virtual void onExit();
    bool init(void* building, int actiontype);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	//更新UI,数值
	void updateUI(float dt);

	//资源足够升级
	bool checkResIsEnough();
private:
	cocos2d::ui::Text* namelbl[3];//资源名字ABEL
	cocos2d::ui::Text* countlbl[3];//资源数量LABEL
	cocos2d::ui::Text* coincountlbl;//需要的元宝数
	void* m_data;//需要传递的数据
	int m_actiontype;//CACTIONTYPE
	std::vector<std::map<std::string, int>> vec_res;//需要的资源的数据
};

#endif
