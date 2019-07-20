/********************************************************************
* 设置界面
*********************************************************************/
#ifndef _SETTING_LAYER_H_
#define _SETTING_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "HttpDataSwap.h"
#include "DynamicValue.h"
USING_NS_CC;

class SettingLayer :public Layer, public cocos2d::ui::EditBoxDelegate, public HTTPDataDelegateProtocol
{
public:
	SettingLayer();
	~SettingLayer();

	virtual bool init();
	CREATE_FUNC(SettingLayer);

	void modifyNameCount();
private:

	/****************************
	开关checkbox点击回调
	****************************/
	void checkBoxCallback(cocos2d::Ref* pSender, cocos2d::ui::CheckBox::EventType);

	/****************************
	更新状态
	@para 0音乐，1音效
	****************************/
	void updateSoundStatus(int type);

	/****************************
	按钮点击
	****************************/
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox);

	void editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action);

	void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text);

	void editBoxReturn(cocos2d::ui::EditBox *editBox);

	void onFinish(int errcode);

	void modifyName(int type, std::string utf8name = "");

	void showAddModifyNameCount();

	void onUrlClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type);

private:
	cocos2d::ui::CheckBox* m_musicCheckBox;//checkbox控件
	cocos2d::ui::Text* m_musicOnOffText;//点击checkbox的音乐说明控件 “开”,“关”

	cocos2d::ui::CheckBox* m_soundCheckBox;//checkbox控件
	cocos2d::ui::Text* m_soundOnOffText;//点击checkbox的音效说明控件 “开”,“关”
	cocos2d::ui::ImageView* nicknamebox;
	cocos2d::ui::EditBox* m_editName;
	std::string mynickname;
	std::string lastchangedname;
	cocos2d::ui::TextField* nameTextField;
	cocos2d::ui::Button* randnamebtn;
	cocos2d::ui::Button* addnicknamecountbtn;
	DynamicValueInt nicknamecount_coin;
	bool isAddModifyNameCount;
};
#endif

