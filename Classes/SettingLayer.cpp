#include "SettingLayer.h"
#include "CommonFuncs.h"
#include "SoundManager.h"
#include "MovingLabel.h"
#include "WaitingProgress.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "AnimationEffect.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#endif

SettingLayer::SettingLayer()
{
}


SettingLayer::~SettingLayer()
{

}

bool SettingLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//蒙版
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color, 0, "colorLayer");

	//加载csb文件
	Node* csbnode = CSLoader::createNode("settingLayer.csb");
	this->addChild(csbnode);

	//关闭按钮
	cocos2d::ui::Button* backbtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	backbtn->setTag(1000);
	backbtn->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));

	//随机名字
	cocos2d::ui::Button* randnamebtn = (cocos2d::ui::Button*)csbnode->getChildByName("randnamebtn");
	randnamebtn->setTag(1001);
	randnamebtn->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));

	//音乐checkbox
	m_musicCheckBox = (cocos2d::ui::CheckBox*)csbnode->getChildByName("musiccheck");
	m_musicCheckBox->addEventListener(CC_CALLBACK_2(SettingLayer::checkBoxCallback, this));
	m_musicCheckBox->setTag(100);
	m_musicOnOffText = (cocos2d::ui::Text*)csbnode->getChildByName("musicstatus");
	updateSoundStatus(0);

	//音效checkbox
	m_soundCheckBox = (cocos2d::ui::CheckBox*)csbnode->getChildByName("soundcheck");
	m_soundCheckBox->addEventListener(CC_CALLBACK_2(SettingLayer::checkBoxCallback, this));
	m_soundCheckBox->setTag(101);
	m_soundOnOffText = (cocos2d::ui::Text*)csbnode->getChildByName("soundstatus");
	updateSoundStatus(1);

	cocos2d::ui::Text* idlbl = (cocos2d::ui::Text*)csbnode->getChildByName("id");
	idlbl->setString(GlobalInstance::getInstance()->getMyID());
    
	cocos2d::ui::TextField* nameTextField = (cocos2d::ui::TextField*)csbnode->getChildByName("nameinput");
	nameTextField->setString("");
	nameTextField->setVisible(false);

	mynickname = GlobalInstance::getInstance()->getMyNickName();

	m_editName = cocos2d::ui::EditBox::create(nameTextField->getContentSize(), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName("ui/blank.png"));
	m_editName->setPosition(nameTextField->getPosition());
	m_editName->setFontColor(Color3B::BLACK);
	m_editName->setPlaceHolder(ResourceLang::map_lang["nicknameplaceholder"].c_str());
	m_editName->setPlaceholderFontSize(nameTextField->getFontSize());
	m_editName->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	m_editName->setPlaceholderFontColor(Color3B(112, 116, 109));
	m_editName->setMaxLength(16);//英文字符长度，中文2个
	m_editName->setText(mynickname.c_str());
	//editName->setReturnType(EditBox::KeyboardReturnType::DONE);
	m_editName->setDelegate(this);
	csbnode->addChild(m_editName);

	//layer 点击事件，屏蔽下层事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void SettingLayer::updateSoundStatus(int type)
{
	if (type == 0)
	{
		if (SoundManager::getInstance()->getMusicIsOn())
		{
			m_musicCheckBox->setSelected(true);
			m_musicOnOffText->setString(ResourceLang::map_lang["soundontext"]);
		}
		else
		{
			m_musicCheckBox->setSelected(false);
			m_musicOnOffText->setString(ResourceLang::map_lang["soundofftext"]);
		}
	}
	else
	{
		if (SoundManager::getInstance()->getSoundIsOn())
		{
			m_soundCheckBox->setSelected(true);
			m_soundOnOffText->setString(ResourceLang::map_lang["soundontext"]);
		}
		else
		{
			m_soundCheckBox->setSelected(false);
			m_soundOnOffText->setString(ResourceLang::map_lang["soundofftext"]);
		}
	}
}

void SettingLayer::checkBoxCallback(cocos2d::Ref* pSender, cocos2d::ui::CheckBox::EventType type)
{
	int tag = ((Node*)pSender)->getTag();
	switch(type)
	{
		case cocos2d::ui::CheckBox::EventType::SELECTED://选中
		{
			if (tag == 100)
			{
				SoundManager::getInstance()->setIsMusicOn(true);
				SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_HOME);
				updateSoundStatus(0);
			}
			else
			{
				SoundManager::getInstance()->setIsSoundOn(true);
				updateSoundStatus(1);
			}
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		}
			break;
		case cocos2d::ui::CheckBox::EventType::UNSELECTED://不选中
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			if (tag == 100)
			{
				SoundManager::getInstance()->stopBackMusic();
				SoundManager::getInstance()->setIsMusicOn(false);
				updateSoundStatus(0);
			}
			else
			{
				SoundManager::getInstance()->setIsSoundOn(false);
				updateSoundStatus(1);
			}
		}
			break;
		default:
			break;
	}
}

void SettingLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		int tag = ((Node*)pSender)->getTag();
		switch (tag)
		{
		case 1000:
			AnimationEffect::closeAniEffect((Layer*)this);
			//this->removeFromParentAndCleanup(true);
			break;
		case 1001:
			modifyName(1);
			break;
		default:
			break;
		}
	}
}


void SettingLayer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
	lastchangedname = editBox->getText();
}

void SettingLayer::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action)
{
	if (GlobalInstance::getInstance()->getMyNickName().compare(editBox->getText()) == 0)
	{
		return;
	}
	else
	{
		std::string utf8str = editBox->getText();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		utf8str = gbkToUTF8(editstr.c_str());
#endif
		modifyName(0, utf8str);
	}
}

void SettingLayer::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text)
{
	int encount = 0;
	int cncount = 0;
	CommonFuncs::checkEnAndCnCount(text, encount, cncount);
	if (encount + cncount * 2 > editBox->getMaxLength())
	{
		editBox->setText(lastchangedname.c_str());
	}
	else
	{
		lastchangedname = text;
	}

}

void SettingLayer::editBoxReturn(cocos2d::ui::EditBox *editBox)
{

}

void SettingLayer::modifyName(int type, std::string utf8name)
{
	if (type == 0)
	{
		if (utf8name.length() > 0)
		{
			WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["doingtext"]);
			Director::getInstance()->getRunningScene()->addChild(waitbox, 1, "waitbox");

			HttpDataSwap::init(this)->modifyName(0, utf8name);
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["nicknameempty"]);
		}
	}
	else
	{
		WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["doingtext"]);
		Director::getInstance()->getRunningScene()->addChild(waitbox, 1, "waitbox");
		HttpDataSwap::init(this)->modifyName(1);
	}
}

void SettingLayer::onFinish(int errcode)
{
	Director::getInstance()->getRunningScene()->removeChildByName("waitbox");
	if (errcode == 0)
	{	
		MovingLabel::show(ResourceLang::map_lang["modifynicknamesucc"]);
		mynickname = GlobalInstance::getInstance()->getMyNickName();
		m_editName->setText(mynickname.c_str());
	}
	else
	{
		if (errcode == 2)
			MovingLabel::show(ResourceLang::map_lang["modifynicknametimeslimit"]);
		else
			MovingLabel::show(ResourceLang::map_lang["modifynicknamefail"]);
		m_editName->setText(mynickname.c_str());
	}
}
