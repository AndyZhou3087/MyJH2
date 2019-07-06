#include "LoginGetLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "LoginRewardV2Layer.h"
#include "MovingLabel.h"
#include "WaitingProgress.h"
#include "SoundManager.h"

USING_NS_CC;

LoginGetLayer::LoginGetLayer()
{
	getcount = 0;
	desclbl = NULL;
}

LoginGetLayer::~LoginGetLayer()
{

}


LoginGetLayer* LoginGetLayer::create()
{
	LoginGetLayer *pRet = new(std::nothrow)LoginGetLayer();
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
bool LoginGetLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 230));
	this->addChild(color,0,"colorLayer");

	std::vector<std::string> vec_str;
	CommonFuncs::split(GlobalInstance::loginData.rwds, vec_str, ",");
	for (unsigned int n = 0; n < vec_str.size(); n++)
	{
		if (vec_str[n].length() > 0)
		{
			std::vector<std::string> one_str;
			CommonFuncs::split(vec_str[n], one_str, "-");

			MSGAWDSDATA adata;
			adata.rid = one_str[0];
			adata.count = atoi(one_str[1].c_str());
			adata.qu = atoi(one_str[2].c_str());

			if (GlobalInstance::map_AllResources.find(adata.rid) != GlobalInstance::map_AllResources.end())
				vec_rewards.push_back(adata);
		}
	}

	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	this->addChild(wp, 0, "waitingprogress");
	HttpDataSwap::init(this)->getLoginAward();
	SoundManager::getInstance()->pauseBackMusic();
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

void LoginGetLayer::showRwd()
{


	int startx[] = { 360, 270 ,160 };
	int offsetx[] = { 0, 180, 200 };

	int starty[] = { 710, 780 };
	int offsety[] = { 0, 170 };

	int rewardsize = vec_rewards.size();

	int row = (rewardsize - 1) / 3;

	for (int i = 0; i < rewardsize; i++)
	{

		std::string boxstr = "ui/resbox.png";
		int t = 0;
		int qu = vec_rewards[i].qu;
		std::string resid = vec_rewards[i].rid;
		int count = vec_rewards[i].count;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (vec_rewards[i].rid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}
		if (t >= T_ARMOR && t <= T_FASHION)
		{
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_WG && t <= T_NG)
		{
			qu = GlobalInstance::map_GF[resid].qu;
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_RENS && t <= T_BOX)
		{
			qu = atoi(resid.substr(1).c_str()) - 1;
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}

		cocos2d::ui::ImageView* resbox = cocos2d::ui::ImageView::create(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
		resbox->addTouchEventListener(CC_CALLBACK_2(LoginGetLayer::onclick, this));
		resbox->setTouchEnabled(true);

		resbox->setPosition(Vec2(360, 500));
		resbox->setScale(0);

		int index = 0;
		if (i < row * 3)
			index = (3 - 1) % 3;
		else
			index = (rewardsize - 1) % 3;

		int x = startx[index] + offsetx[index] * (i % 3);
		int y = starty[(rewardsize - 1) / 3] - offsety[(rewardsize - 1) / 3] * (i / 3);
		//box->setPosition(Vec2(x, y));

		resbox->runAction(Spawn::create(MoveTo::create(0.3f, Vec2(x, y)), ScaleTo::create(0.3f, 1), DelayTime::create(2 + i*0.5), CallFunc::create(CC_CALLBACK_0(LoginGetLayer::showRotation, this, resbox)),  NULL));
		//resbox->setPositionX(startx[rewardsize - 1] + offsetx[rewardsize - 1] * i);

		this->addChild(resbox);

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(resbox->getContentSize().width / 2, resbox->getContentSize().height / 2));
		resbox->addChild(res, 0, "res");

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setColor(Color3B(250, 250, 250));
		namelbl->setPosition(Vec2(resbox->getContentSize().width / 2, -10));
		resbox->addChild(namelbl);

		std::string countstr = StringUtils::format("x%d", count);
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(resbox->getContentSize().width - 10, 8));
		resbox->addChild(countlbl);

		CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

		if (resid.compare("r006") == 0)
		{
			DynamicValueInt dvint;
			dvint.setValue(count);
			GlobalInstance::getInstance()->addMySoliverCount(dvint);
		}
		else if (resid.compare("r012") == 0)
		{
			DynamicValueInt dvint;
			dvint.setValue(count);
			GlobalInstance::getInstance()->addMyCoinCount(dvint);
		}
		else
			MyRes::Add(resid, count, MYSTORAGE, qu, GlobalInstance::getInstance()->generateStoneCount(qu));
	}
}

void LoginGetLayer::showRotation(Node* node)
{
	node->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.1f, 5), RotateTo::create(0.1f, 0), RotateTo::create(0.1f, -5), RotateTo::create(0.1f, 0), DelayTime::create(0.5f), NULL)));
	
	if (desclbl == NULL)
	{
		desclbl = Label::createWithTTF(ResourceLang::map_lang["getloginrwdhint"], FONT_NAME, 21);
		desclbl->setColor(Color3B(255, 255, 255));
		desclbl->setOpacity(128);
		desclbl->setPosition(Vec2(360, 500));
		this->addChild(desclbl);
	}
}

void LoginGetLayer::onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	//CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (desclbl != NULL)
			desclbl->setVisible(false);
		cocos2d::ui::ImageView* node = (cocos2d::ui::ImageView*)pSender;
		node->setTouchEnabled(false);
		node->stopAllActions();
		node->setRotation(0);
		int tag = node->getTag();
		Node* resnode = node->getChildByName("res");
		CommonFuncs::changeGray(resnode);
		node->runAction(Spawn::create(MoveTo::create(0.3f, Vec2(-70, 250)), ScaleTo::create(0.3f, 0.0f), NULL));
		getcount++;
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_GETLOGINAWARD);
		if (getcount == vec_rewards.size())
		{
			LoginRewardV2Layer * parent = (LoginRewardV2Layer *)this->getParent();
			parent->changeLogin();
			this->scheduleOnce(schedule_selector(LoginGetLayer::removeSelf), 0.8f);
		}
	}
}

void LoginGetLayer::removeSelf(float dt)
{
	SoundManager::getInstance()->resumeBackMusic();

	AnimationEffect::closeAniEffect((Layer*)this);
}

void LoginGetLayer::onFinish(int code)
{
	this->removeChildByName("waitingprogress");
	if (code == SUCCESS)
	{
		showRwd();
		GlobalInstance::loginData.isGeted = true;
	}
	else
	{
		removeSelf(0);
		MovingLabel::show(ResourceLang::map_lang["networkerror"]);
	}
}