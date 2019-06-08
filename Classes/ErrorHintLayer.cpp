#include "ErrorHintLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "AnimationEffect.h"
#include "LoadingScene.h"
#include "DataSave.h"
#include "MyRes.h"
#include "HttpDataSwap.h"
#include "MainScene.h"
#include "MovingLabel.h"
#include "SoundManager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#endif

ErrorHintLayer::ErrorHintLayer()
{

}


ErrorHintLayer::~ErrorHintLayer()
{

}

ErrorHintLayer* ErrorHintLayer::create(int forwhere)
{
	ErrorHintLayer *pRet = new(std::nothrow)ErrorHintLayer();
	if (pRet && pRet->init(forwhere))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool ErrorHintLayer::init(int forwhere)
{
	if (!Layer::init())
	{
		return false;
	}

	m_forwhere = forwhere;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("errorHintLayer.csb"));
	this->addChild(m_csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("title");
	title->loadTexture(ResourcePath::makeTextImgPath("usertitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	actionbtn = (cocos2d::ui::Button*)m_csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(ErrorHintLayer::onBtnClick, this));

	cocos2d::ui::ImageView* actionbtntext = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");


	cocos2d::ui::Text* text = (cocos2d::ui::Text*)m_csbnode->getChildByName("text");
	Label* dlbl = (Label*)text->getVirtualRenderer();
	dlbl->setLineSpacing(10);

	cocos2d::ui::Text* text_1 = (cocos2d::ui::Text*)m_csbnode->getChildByName("text_1");
	cocos2d::ui::Text* text_2 = (cocos2d::ui::Text*)m_csbnode->getChildByName("text_2");
	text_2->addTouchEventListener(CC_CALLBACK_2(ErrorHintLayer::onQQClick, this));

	cocos2d::ui::Text* idtext = (cocos2d::ui::Text*)m_csbnode->getChildByName("idtext");

	cocos2d::ui::Text* text3 = (cocos2d::ui::Text*)m_csbnode->getChildByName("text_3");
	text3->setString(GlobalInstance::getInstance()->UUID());

	if (GlobalInstance::getInstance()->getMyID().length() > 0)
		text3->setVisible(false);

	cocos2d::ui::Text* text4 = (cocos2d::ui::Text*)m_csbnode->getChildByName("text_4");
	text4->setString("v" + GlobalInstance::getInstance()->getVersionCode());

	std::string actiontextstr = "okbtn_text";
	 
	if (forwhere == 0)
	{
		text->setString(ResourceLang::map_lang["networkerror"]);
		text_1->setVisible(false);
		text_2->setVisible(false);
		actiontextstr = "retry_text";
	}
	else if (forwhere == 1)
	{
		text->setString(ResourceLang::map_lang["dataerr0"]);
		text_1->setVisible(false);
		text_2->setVisible(false);
		int cheatcount = DataSave::getInstance()->getCheatCount();
		DataSave::getInstance()->setCheatCount(cheatcount + 1);
	}
	else if (forwhere == 2 || forwhere == 3 || forwhere == 4)
	{
		text->setString(ResourceLang::map_lang["dataerr1"]);

		if (forwhere == 2)
			HttpDataSwap::init(NULL)->report("0");
		else if (forwhere == 4)
		{
			text->setString(ResourceLang::map_lang["dataerr2"]);

			if (GlobalInstance::qq.length() <= 0)
				GlobalInstance::qq = "1703153046";
		}

		if (GlobalInstance::qq.length() > 0)
		{
			text_1->setString(ResourceLang::map_lang["comtactus"]);
			std::string str = ResourceLang::map_lang["qq"];
			str.append(GlobalInstance::qq);
			text_2->setString(str);

			DrawNode* underlineNode = DrawNode::create();
			text_2->addChild(underlineNode, 1);
			underlineNode->setLineWidth(2.0f);
			underlineNode->drawLine(Vec2(0, 0), Vec2(text_2->getContentSize().width, 0), Color4F(text_2->getTextColor()));
		}
		else
		{
			text_1->setVisible(false);
			text_2->setVisible(false);
		}
		actionbtn->setVisible(false);

		std::string idstr = StringUtils::format("ID:%s", GlobalInstance::getInstance()->getMyID().c_str());
		idtext->setString(idstr);
		text3->setVisible(false);
	}
	else if (forwhere == 5)
	{
		text->setString(ResourceLang::map_lang["networkerror"]);
		text_1->setVisible(false);
		text_2->setVisible(false);
		actiontextstr = "okbtn_text";
	}
	else if (forwhere == 6)
	{
		text->setString(ResourceLang::map_lang["crackerr"]);
		if (GlobalInstance::qq.length() > 0)
		{
			std::string text2str;
			text2str.append(ResourceLang::map_lang["comtactus"]);
			text2str.append(ResourceLang::map_lang["qq"]);
			text2str.append(GlobalInstance::qq);
			text_2->setString(text2str);
			DrawNode* underlineNode = DrawNode::create();
			text_2->addChild(underlineNode, 1);
			underlineNode->setLineWidth(2.0f);
			underlineNode->drawLine(Vec2(0, 0), Vec2(text_2->getContentSize().width, 0), Color4F(text_2->getTextColor()));
		}
		else
		{
			text_2->setVisible(false);
		}

		if (GlobalInstance::legalcopyurl.length() > 0)
		{
			text_1->setString(ResourceLang::map_lang["downloadtext"]);
			text_1->setTextColor(Color4B(58, 151, 234, 255));
			DrawNode* underlineNode = DrawNode::create();
			text_1->addChild(underlineNode, 1);
			underlineNode->setLineWidth(2.0f);
			underlineNode->drawLine(Vec2(0, 0), Vec2(text_1->getContentSize().width, 0), Color4F(text_2->getTextColor()));
			text_1->setTag(1000);
			text_1->addTouchEventListener(CC_CALLBACK_2(ErrorHintLayer::onQQClick, this));
		}
		else
		{
			text_1->setVisible(false);
		}
	}


	if (forwhere == 1 || forwhere == 2)
	{
		GlobalInstance::getInstance()->saveMyHeros();
		MyRes::saveData();

		DataSave::getInstance()->setMyCoinCount(GlobalInstance::getInstance()->getMyCoinCount().getValue());
		DataSave::getInstance()->setMySoliverCount(GlobalInstance::getInstance()->getMySoliverCount().getValue());
	}


	actionbtntext->loadTexture(ResourcePath::makeTextImgPath(actiontextstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntext->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath(actiontextstr, langtype))->getContentSize());

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	/*listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
	 
	};*/
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void ErrorHintLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)pSender;
		btn->setEnabled(false);
		if (m_forwhere == 0)
		{
			if (g_loadingScene != NULL)
				HttpDataSwap::init(g_loadingScene)->getPlayerId();
			else if (g_mainScene != NULL)
				HttpDataSwap::init(g_mainScene)->getServerTime();
		}
		else if (m_forwhere == 1)
		{
			AnimationEffect::closeAniEffect(this);
			GlobalInstance::isCheat = false;
		}
		else if (m_forwhere == 5)
		{
			AnimationEffect::closeAniEffect(this);
		}
	}
}

void ErrorHintLayer::resetBtn()
{
	actionbtn->setEnabled(true);
}

void ErrorHintLayer::onFinish(int code)
{
	if (code == SUCCESS)
	{

	}
}

void ErrorHintLayer::onQQClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Node* clicknode = (Node*)pSender;
		if (clicknode->getTag() == 1000)
		{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			JniMethodInfo methodInfo;
			char p_str1[128] = { 0 };
			sprintf(p_str1, "%s", GlobalInstance::legalcopyurl.c_str());
			std::string clsname = StringUtils::format("%s/AppActivity", ANDOIRJNICLS);
			if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "openBrower", "(Ljava/lang/String;)V"))
			{
				jstring str1 = methodInfo.env->NewStringUTF(p_str1);
				methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, str1);
			}
#endif
		}
		else
		{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			JniMethodInfo methodInfo;
			char p_str1[32] = { 0 };
			sprintf(p_str1, "%s", GlobalInstance::qq.c_str());
			std::string clsname = StringUtils::format("%s/AppActivity", ANDOIRJNICLS);
			if (JniHelper::getStaticMethodInfo(methodInfo, clsname.c_str(), "copyToClipboard", "(Ljava/lang/String;)V"))
			{
				jstring str1 = methodInfo.env->NewStringUTF(p_str1);
				methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, str1);
			}
			MovingLabel::show(ResourceLang::map_lang["copyqq"]);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			copytoclipboard((char*)GlobalInstance::qq.c_str());
			MovingLabel::show(ResourceLang::map_lang["copyqq"]);
#endif
		}
	}
}