#include "SettingLayer.h"
#include "CommonFuncs.h"
#include "SoundManager.h"
#include "MovingLabel.h"
#include "WaitingProgress.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "AnimationEffect.h"
#include "MainScene.h"
#include "NewGuideLayer.h"
#include "Const.h"
#include "HintBoxLayer.h"
#include "RewardLayer.h"
#include "SimpleRuleLayer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#endif

SettingLayer::SettingLayer()
{
	nicknamecount_coin.setValue(MODIFYNICKNAMECOUNT_COIN);
	isAddModifyNameCount = false;
	httptag = 0;
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
	randnamebtn = (cocos2d::ui::Button*)csbnode->getChildByName("randnamebtn");
	randnamebtn->setTag(1001);
	randnamebtn->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));

	//昵称次数
	addnicknamecountbtn = (cocos2d::ui::Button*)csbnode->getChildByName("addnamecountbtn");
	addnicknamecountbtn->setTag(1003);
	addnicknamecountbtn->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));

	nicknamebox = (cocos2d::ui::ImageView*)csbnode->getChildByName("nicknamebox");
	nicknamebox->setTag(1004);
	nicknamebox->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));

	//删除存档
	cocos2d::ui::Button* cleandatabtn = (cocos2d::ui::Button*)csbnode->getChildByName("cleandatabtn");
	cleandatabtn->setTag(1002);
	cleandatabtn->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));

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
    
	nameTextField = (cocos2d::ui::TextField*)csbnode->getChildByName("nameinput");
	nameTextField->setString("");
	nameTextField->setVisible(false);

	mynickname = GlobalInstance::getInstance()->getMyNickName();

	m_editName = cocos2d::ui::EditBox::create(Size(nameTextField->getContentSize().width, 40), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName("ui/blank.png"));
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
	m_editName->setTag(0);
	csbnode->addChild(m_editName);

	cocos2d::ui::Text* verlbl = (cocos2d::ui::Text*)csbnode->getChildByName("versionlbl");
	std::string vertxt = StringUtils::format("v%s", GlobalInstance::getInstance()->getVersionCode().c_str());
	verlbl->setString(vertxt);

	cocos2d::ui::Text* copyrightlbl = (cocos2d::ui::Text*)csbnode->getChildByName("copyrighttext");
	copyrightlbl->setString(ResourceLang::map_lang["copyright"]);
	
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	copyrightlbl->setVisible(true);
#else
	copyrightlbl->setVisible(false);
#endif

	if (GlobalInstance::getInstance()->getIsNichnameCountMax())
	{
		randnamebtn->setVisible(false);
		addnicknamecountbtn->setVisible(true);
		nicknamebox->setEnabled(true);
		m_editName->setEnabled(false);
	}
	else
	{
		randnamebtn->setVisible(true);
		addnicknamecountbtn->setVisible(false);
		nicknamebox->setEnabled(false);
		m_editName->setEnabled(true);
	}
	cocos2d::ui::Text* aurltext = (cocos2d::ui::Text*)csbnode->getChildByName("androidurltext");
	if (GlobalInstance::androidurl.length() > 0)
	{
		DrawNode* underlineNode = DrawNode::create();
		aurltext->addChild(underlineNode, 1);
		underlineNode->setLineWidth(2.0f);
		underlineNode->drawLine(Vec2(0, 0), Vec2(aurltext->getContentSize().width, 0), Color4F(aurltext->getTextColor()));
		aurltext->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onUrlClick, this));
	}
	else
	{
		aurltext->setVisible(false);
	}

	cocos2d::ui::Text* mycardnotext = (cocos2d::ui::Text*)csbnode->getChildByName("mycardnotext");
	mycardnotext->setString(ResourceLang::map_lang["mycardnotext"]);

	cocos2d::ui::Text* mycardno = (cocos2d::ui::Text*)csbnode->getChildByName("mycardno");
	mycardno->setString(GlobalInstance::myinvitationcode);
	mycardno->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onInvitationcodeClick, this));

	if (GlobalInstance::myinvitationcode.length() > 0)
	{
		DrawNode* underlineNode = DrawNode::create();
		mycardno->addChild(underlineNode, 1);
		underlineNode->setLineWidth(2.0f);
		underlineNode->drawLine(Vec2(0, 0), Vec2(mycardno->getContentSize().width, 0), Color4F(mycardno->getTextColor()));
	}

	cocos2d::ui::TextField* cardnoinput = (cocos2d::ui::TextField*)csbnode->getChildByName("cardnoinput");
	cardnoinput->setString("");
	cardnoinput->setVisible(false);
	
	cardnoedit = cocos2d::ui::EditBox::create(Size(cardnoinput->getContentSize().width, 40), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName("ui/blank.png"));
	cardnoedit->setPosition(cardnoinput->getPosition());
	cardnoedit->setFontColor(Color3B::BLACK);
	cardnoedit->setPlaceHolder(ResourceLang::map_lang["mycardinputhint"].c_str());
	cardnoedit->setPlaceholderFontSize(cardnoinput->getFontSize());
	cardnoedit->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	cardnoedit->setPlaceholderFontColor(Color3B(112, 116, 109));
	cardnoedit->setMaxLength(10);//英文字符长度，中文2个
	cardnoedit->setText("");
	//editName->setReturnType(EditBox::KeyboardReturnType::DONE);
	cardnoedit->setDelegate(this);
	cardnoedit->setTag(1);
	csbnode->addChild(cardnoedit);

	exchangebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("exchangebtn");
	exchangebtn->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));
	exchangebtn->setTag(1005);

	int langtype = GlobalInstance::getInstance()->getLang();
	cocos2d::ui::ImageView* exchangebtntxt = (cocos2d::ui::ImageView*)exchangebtn->getChildByName("text");
	exchangebtntxt->loadTexture(ResourcePath::makeTextImgPath("exchangebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* rulebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("rulebtn");
	rulebtn->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onBtnClick, this));
	rulebtn->setTag(1006);

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
		{
			AnimationEffect::closeAniEffect((Layer*)this);
			g_mainScene->delayShowNewerGuide(0);
		}
			break;
		case 1001:
			modifyName(1);
			break;
		case 1002:
		{
			HintBoxLayer* layer = HintBoxLayer::create(ResourceLang::map_lang["resetdatatext"], 8);
			layer->setUserData((void*)this);
			g_mainScene->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case 1003:
		{
			showAddModifyNameCount();
		}
			break;
		case 1004:
		{
			showAddModifyNameCount();
		}
		break;
		case 1005:
		{
			WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["doingtext"]);
			Director::getInstance()->getRunningScene()->addChild(waitbox, 1, "waitbox");
			httptag = 1;
			std::string cardno = cardnoedit->getText();
			HttpDataSwap::init(this)->getExchangeInvitationcode(CommonFuncs::trim(cardno));
		}
		break;
		case 1006:
		{
			std::vector<std::string> vec_;
			CommonFuncs::split(GlobalInstance::myinvitationrwd, vec_, "-");
			std::string desc = StringUtils::format(ResourceLang::map_lang["mycardnoruletext"].c_str(), atoi(vec_[0].c_str()), atoi(vec_[1].c_str()));
			SimpleRuleLayer* layer = SimpleRuleLayer::create(desc);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
			break;
		default:
			break;
		}
	}
}

void SettingLayer::onUrlClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo methodInfo;
		char p_str1[128] = { 0 };
		sprintf(p_str1, "%s", GlobalInstance::androidurl.c_str());
		std::string clsname = StringUtils::format("%s/AppActivity", ANDOIRJNICLS);
		if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "copyToClipboard", "(Ljava/lang/String;)V"))
		{
			jstring str1 = methodInfo.env->NewStringUTF(p_str1);
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, str1);
		}
		MovingLabel::show(ResourceLang::map_lang["copyurl"]);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		copytoclipboard((char*)GlobalInstance::androidurl.c_str());
		MovingLabel::show(ResourceLang::map_lang["copyurl"]);
#endif
	}
}

void SettingLayer::onInvitationcodeClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo methodInfo;
		char p_str1[128] = { 0 };
		sprintf(p_str1, "%s", GlobalInstance::myinvitationcode.c_str());
		std::string clsname = StringUtils::format("%s/AppActivity", ANDOIRJNICLS);
		if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "copyToClipboard", "(Ljava/lang/String;)V"))
		{
			jstring str1 = methodInfo.env->NewStringUTF(p_str1);
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, str1);
		}
		MovingLabel::show(ResourceLang::map_lang["mycardcopy"]);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		copytoclipboard((char*)GlobalInstance::myinvitationcode.c_str());
		MovingLabel::show(ResourceLang::map_lang["mycardcopy"]);
#endif
	}
}

void SettingLayer::showAddModifyNameCount()
{
	std::string str = StringUtils::format(ResourceLang::map_lang["nicknamecountmaxtext"].c_str(), nicknamecount_coin.getValue());
	HintBoxLayer* layer = HintBoxLayer::create(str, 9);
	layer->setUserData((void*)this);
	this->addChild(layer);
	AnimationEffect::openAniEffect((Layer*)layer);
}


void SettingLayer::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
	if (editBox->getTag() == 0)
	{
		lastchangedname = editBox->getText();
	}
}

void SettingLayer::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox, EditBoxEndAction action)
{
	if (editBox->getTag() == 0)
	{
		if (GlobalInstance::getInstance()->getMyNickName().compare(editBox->getText()) == 0)
		{
			return;
		}
		else
		{
			std::string utf8str = editBox->getText();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			utf8str = gbkToUTF8(editBox->getText());
#endif
			modifyName(0, utf8str);
		}
	}
	else
	{
		std::string inputtext = editBox->getText();
		std::string noblanktext = CommonFuncs::trim(inputtext);
		if (GlobalInstance::myinvitationcode.compare(noblanktext) == 0)
		{
			editBox->setText("");
			MovingLabel::show(ResourceLang::map_lang["mycardnoinvail"]);
			return;
		}

	}
}

void SettingLayer::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string &text)
{
	if (editBox->getTag() == 0)
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
		if (httptag == 0)
		{
			if (isAddModifyNameCount)
			{
				randnamebtn->setVisible(true);
				addnicknamecountbtn->setVisible(false);
				GlobalInstance::getInstance()->setIsNichnameCountMax(false);
				m_editName->setEnabled(true);
				nicknamebox->setEnabled(false);
				MovingLabel::show(ResourceLang::map_lang["buynicknamecountmaxtext"]);

				GlobalInstance::getInstance()->costMyCoinCount(nicknamecount_coin);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["modifynicknamesucc"]);
				mynickname = GlobalInstance::getInstance()->getMyNickName();
				m_editName->setText(mynickname.c_str());
			}
		}
		else
		{
			cardnoedit->setEnabled(false);
			exchangebtn->setEnabled(false);
			std::vector<MSGAWDSDATA> vec_rewards;
			MSGAWDSDATA wdata;
			wdata.rid = "r012";

			std::vector<std::string> vec_;
			CommonFuncs::split(GlobalInstance::myinvitationrwd, vec_, "-");
			if (vec_.size() > 1)
				wdata.count = atoi(vec_[0].c_str());
			else
				wdata.count = 100;
			wdata.qu = 0;
			vec_rewards.push_back(wdata);

			if (g_mainScene != NULL)
			{
				RewardLayer* layer = RewardLayer::create(vec_rewards);
				g_mainScene->addChild(layer, 100);
				AnimationEffect::openAniEffect(layer);
			}
		}
	}
	else
	{
		if (httptag == 0)
		{
			if (errcode == 2)//1--player not exit;2--timesout;3--used;4--senstive
			{
				MovingLabel::show(ResourceLang::map_lang["modifynicknametimeslimit"]);
				randnamebtn->setVisible(false);
				addnicknamecountbtn->setVisible(true);
				GlobalInstance::getInstance()->setIsNichnameCountMax(true);
				m_editName->setEnabled(false);
				nicknamebox->setEnabled(true);
			}
			else if (errcode == 3)
				MovingLabel::show(ResourceLang::map_lang["nicknameused"]);
			else if (errcode == 4)
				MovingLabel::show(ResourceLang::map_lang["senstiveword"]);
			else
				MovingLabel::show(ResourceLang::map_lang["modifynicknamefail"]);
			m_editName->setText(mynickname.c_str());
		}
		else
		{
			if (errcode == 4 || errcode == 7)
			{
				MovingLabel::show(ResourceLang::map_lang["mycardnoinvail"]);
			}
			else if (errcode == 5)
			{
				cardnoedit->setEnabled(false);
				exchangebtn->setEnabled(false);
				MovingLabel::show(ResourceLang::map_lang["mycardnoisexchange"]);
			}
			else if (errcode == 6)
			{
				cardnoedit->setEnabled(false);
				exchangebtn->setEnabled(false);
				MovingLabel::show(ResourceLang::map_lang["mycardnoexpire"]);
			}
		}
	}
	isAddModifyNameCount = false;
	httptag = 0;
}

void SettingLayer::modifyNameCount()
{
	WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["doingtext"]);
	Director::getInstance()->getRunningScene()->addChild(waitbox, 1, "waitbox");
	HttpDataSwap::init(this)->modifyName(2);
	isAddModifyNameCount = true;
}
