#ifndef _MACTHFIGHTSUCC_LAYER_H_
#define _MACTHFIGHTSUCC_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ResBase.h"
#include "GlobalInstance.h"
USING_NS_CC;

class MacthFightSucAwdLayer :public Layer
{
public:
	MacthFightSucAwdLayer();
	~MacthFightSucAwdLayer();

	virtual bool init();

	CREATE_FUNC(MacthFightSucAwdLayer);

private:
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void delayShowResAnim(float dt);

	void finishAnim();
private:
	cocos2d::ui::ImageView* box[3];

	cocos2d::ui::ImageView* resbox;
	cocos2d::ui::ImageView* res;
	Node* animnode;
	cocos2d::ui::Text* desc;
	bool iscancolse;

};
#endif

