
#ifndef _MESSAGEDESC_LAYER_H_
#define _MESSAGEDESC_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GlobalInstance.h"

USING_NS_CC;

typedef struct
{
	std::string rid;
	int count;
	int qu;
}MSGAWDSDATA;

class MessageDescLayer :public Layer
{
public:
	MessageDescLayer();
	~MessageDescLayer();

	virtual bool init(int index);

	static MessageDescLayer* create(int index);

private:
	cocos2d::ui::Button* accbtn;

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void delay(float dt);

private:
	std::vector<MSGAWDSDATA> awdslist;
};
#endif

