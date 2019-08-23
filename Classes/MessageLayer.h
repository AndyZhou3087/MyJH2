#ifndef __MESSAGE_LAYER_H__
#define __MESSAGE_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "HttpDataSwap.h"
USING_NS_CC;

class MessageLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	MessageLayer();
	~MessageLayer();
	static MessageLayer* create(int mode = 0);//0--showall;1--活动
	virtual void onExit();
    bool init(int mode = 0);
	//更新scrollview
	void refreshScrollViewUi();
	void updateStatus(int index);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);

private:
	cocos2d::ui::ScrollView* m_contentscroll;
	int _mode;
};

#endif
