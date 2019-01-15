#include "LoginRewardV2Layer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "GlobalInstance.h"
#include "WaitingProgress.h"
#include "ErrorHintLayer.h"
#include "Shake.h"
#include "LoginGetLayer.h"

USING_NS_CC;

LoginRewardV2Layer::LoginRewardV2Layer()
{
	isRetry = false;
	showwordindex = 0;
}

LoginRewardV2Layer::~LoginRewardV2Layer()
{

}


LoginRewardV2Layer* LoginRewardV2Layer::create()
{
	LoginRewardV2Layer *pRet = new(std::nothrow)LoginRewardV2Layer();
	if (pRet && pRet->init())
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

// on "init" you need to initialize your instance
bool LoginRewardV2Layer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 230));
	this->addChild(color,0,"colorLayer");


	csbnode = CSLoader::createNode(ResourcePath::makePath("loginRewardV2Layer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	wordtext = (cocos2d::ui::Text*)csbnode->getChildByName("wordtext");
	wordtext->setString("");

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(LoginRewardV2Layer::onBtnClick, this));

	for (int i = 1; i <= 3; i++)
	{
		std::string name = StringUtils::format("loginbox%d", i);
		cocos2d::ui::ImageView* loginbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(name);
		loginbox->setTag(i);
		loginbox->addTouchEventListener(CC_CALLBACK_2(LoginRewardV2Layer::onBtnClick, this));
		if (i == 2)
		{
			std::string strkey = StringUtils::format("loginbox%ddesc", i);

			//wordtext->setString(ResourceLang::map_lang[strkey]);
			if (GlobalInstance::loginData.logindays > 7)
			{
				vec_words.push_back(ResourceLang::map_lang["loginbox4desc"]);
				//wordtext->setString(ResourceLang::map_lang["loginbox4desc"]);
			}
			else
			{
				vec_words.push_back(ResourceLang::map_lang[strkey]);
			}
		}
	}

	desctext = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desctext->setVisible(false);

	getLoginData();

	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void LoginRewardV2Layer::getLoginData()
{
	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	this->addChild(wp, 0, "waitingprogress");
	HttpDataSwap::init(this)->getLoginData();
}

void LoginRewardV2Layer::updateWords(float dt)
{
	if (showwordindex >= vec_words.size())
		showwordindex = 0;
	wordtext->setString(vec_words[showwordindex]);
	showwordindex++;
}

void LoginRewardV2Layer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag < 1000)
		{
			std::string strkey = StringUtils::format("loginbox%ddesc", tag);
			//wordtext->setString(ResourceLang::map_lang[strkey]);

			if (GlobalInstance::loginData.logindays > 7)
			{
				if (vec_words.size() > 0)
					vec_words[0] = ResourceLang::map_lang["loginbox4desc"];
				//wordtext->setString(ResourceLang::map_lang["loginbox4desc"]);
			}
			else
			{
				if (vec_words.size() > 0)
					vec_words[0] = ResourceLang::map_lang[strkey];
			}

			if (tag == 2)
			{
				if (GlobalInstance::loginData.isGeted)
				{
					//wordtext->setString(ResourceLang::map_lang["loginbox5desc"]);
					if (vec_words.size() > 0)
						vec_words[0] = ResourceLang::map_lang["loginbox5desc"];
				}
				else
				{
					if (GlobalInstance::loginData.logindays == 0)
					{
						isRetry = true;
						getLoginData();
					}
					else
					{
						LoginGetLayer* layer = LoginGetLayer::create();
						this->addChild(layer);
					}
				}
			}
			showWords();
		}
		else if (tag == 1001)
		{
			removeSelf();
		}

	}
}

void LoginRewardV2Layer::removeSelf()
{
	AnimationEffect::closeAniEffect((Layer*)this);

}

void LoginRewardV2Layer::changeLogin()
{
	Node* loginbox = csbnode->getChildByName("loginbox2");
	loginbox->stopAllActions();
	loginbox->setRotation(0);
	CommonFuncs::changeGray(loginbox);
}

void LoginRewardV2Layer::onFinish(int code)
{
	this->removeChildByName("waitingprogress");
	if (code == SUCCESS)
	{
		if (isRetry)
		{
			LoginGetLayer* layer = LoginGetLayer::create();
			this->addChild(layer);
		}
		Node* loginbox = csbnode->getChildByName("loginbox2");
		if (!GlobalInstance::loginData.isGeted)
		{
			loginbox->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.1f, 7), RotateTo::create(0.1f, 0), RotateTo::create(0.1f, -7), RotateTo::create(0.1f, 0), DelayTime::create(0.5f), NULL)));
		}
		else
		{
			//wordtext->setString(ResourceLang::map_lang["loginbox5desc"]);
			if (vec_words.size() > 0)
				vec_words[0] = ResourceLang::map_lang["loginbox5desc"];
			changeLogin();
		}
		vec_words.push_back(ResourceLang::map_lang["loginrewarddesc"]);

		showWords();
		desctext->setVisible(true);
		std::string daysdesc = StringUtils::format(ResourceLang::map_lang["loginbox6desc"].c_str(), GlobalInstance::loginData.logindays);
		desctext->setString(daysdesc);
	}
	else
	{
		ErrorHintLayer* layer = ErrorHintLayer::create(5);
		this->addChild(layer);
	}
	isRetry = false;
}

void LoginRewardV2Layer::showWords()
{
	if (this->isScheduled(schedule_selector(LoginRewardV2Layer::updateWords)))
		this->unschedule(schedule_selector(LoginRewardV2Layer::updateWords));
	showwordindex = 0;
	updateWords(0);
	this->schedule(schedule_selector(LoginRewardV2Layer::updateWords), 2.0f);
}