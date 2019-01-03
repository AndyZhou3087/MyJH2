
#ifndef _ERRORHINT_LAYER_H_
#define _ERRORHINT_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class ErrorHintLayer :public Layer, public HTTPDataDelegateProtocol
{
public:
	ErrorHintLayer();
	~ErrorHintLayer();

	virtual bool init(int forwhere);

	static ErrorHintLayer* create(int forwhere);//0--联网失败；1--作弊一次，清零警告；2，多次作弊，封停,3，服务器封号

	void resetBtn();
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onFinish(int code);
	int m_forwhere;
	cocos2d::ui::Button* actionbtn;
};
#endif

