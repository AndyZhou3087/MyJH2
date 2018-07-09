#ifndef __RESDESCLAYER_LAYER_H__
#define __RESDESCLAYER_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "ResBase.h"
USING_NS_CC;

typedef enum
{
	S_CAN_USE,
	S_CAN_DECOMPOSE,
	S_OTHER
}S_STATUS;
class ResDescLayer : public cocos2d::Layer
{
public:
	ResDescLayer();
	~ResDescLayer();

	static ResDescLayer* create(ResBase* res, int fromwhere);
	virtual void onExit();
    bool init(ResBase* res, int fromwhere);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	int status;
	ResBase* m_res;
};

#endif
