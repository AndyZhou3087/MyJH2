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

	cocos2d::ui::Text* text_1 = (cocos2d::ui::Text*)m_csbnode->getChildByName("text_1");
	cocos2d::ui::Text* text_2 = (cocos2d::ui::Text*)m_csbnode->getChildByName("text_2");

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
	else if (forwhere == 2)
	{
		text->setString(ResourceLang::map_lang["dataerr1"]);
		if (GlobalInstance::qq.length() > 0)
		{
			text_1->setString(ResourceLang::map_lang["comtactus"]);
			std::string str = ResourceLang::map_lang["qq"];
			str.append(GlobalInstance::qq);
			text_2->setString(str);
		}
		else
		{
			text_1->setVisible(false);
			text_2->setVisible(false);
		}
		actionbtn->setVisible(false);

		HttpDataSwap::init(NULL)->report("0");
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

		}
		else if (m_forwhere == 1)
		{
			AnimationEffect::closeAniEffect(this);
			GlobalInstance::isCheat = false;
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