#ifndef _OPENHOLES_LAYER_H_
#define _OPENHOLES_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Equip.h"

USING_NS_CC;

class OpenHolesLayer :public Layer
{
public:
	OpenHolesLayer();

	~OpenHolesLayer();

	virtual bool init(Equip* res_equip);

	static OpenHolesLayer* create(Equip* res_equip);

private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onResClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void updateUi();
private:
	Node* csbnode;
	Equip* m_equip;
	cocos2d::ui::Text* tipstext;
	Node* selectNode;
	int selectindex;

	cocos2d::ui::Text* rescount[2];

	bool checkRes();

	void updateResCount(float dt);
};
#endif

