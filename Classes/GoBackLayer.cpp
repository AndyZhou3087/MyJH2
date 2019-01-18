#include "GoBackLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "MainScene.h"
#include "MainMapScene.h"
#include "NewGuideLayer.h"
#include "MapBlockScene.h"
#include "HintBoxLayer.h"
#include "BuyCoinLayer.h"
#include "MyTransitionScene.h"

USING_NS_CC;

GoBackLayer::GoBackLayer()
{
	gobackcoin.setValue(GlobalInstance::map_AllResources["t001"].coinval);
}

GoBackLayer::~GoBackLayer()
{

}


GoBackLayer* GoBackLayer::create(int forwhere)
{
	GoBackLayer *pRet = new(std::nothrow)GoBackLayer();
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

// on "init" you need to initialize your instance
bool GoBackLayer::init(int forwhere)
{
	if (!Layer::init())
	{
		return false;
	}

	m_forwhere = forwhere;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("goBackLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["gobacktext"]);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources["t001"].name);

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desclbl");
	std::string desckey = StringUtils::format("gobackdesc%d", forwhere);
	desclbl->setString(ResourceLang::map_lang[desckey]);

	std::string str = StringUtils::format("1/%d", MyRes::getMyResCount("t001"));

	cocos2d::ui::Text* coutlbl = (cocos2d::ui::Text*)csbnode->getChildByName("coutlbl");
	coutlbl->setString(str);
	if (MyRes::getMyResCount("t001") < 1)
	{
		coutlbl->setTextColor(Color4B(255, 0, 0, 255));
	}

	//°´Å¥
	actionbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(GoBackLayer::onBtnClick, this));
	actionbtn->setTag(1);

	cocos2d::ui::ImageView* actionbtntext = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntext->loadTexture(ResourcePath::makeTextImgPath("usebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cancelbtn = (cocos2d::ui::Button*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(GoBackLayer::onBtnClick, this));
	cancelbtn->setTag(0);

	cocos2d::ui::ImageView* cancelbtntext = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntext->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	this->scheduleOnce(schedule_selector(GoBackLayer::delayShowNewerGuide), newguidetime);

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

void GoBackLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(86) && NewGuideLayer::checkifNewerGuide(87))
	{
		showNewerGuide(87);
	}
}

void GoBackLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 87)
	{
		nodes.push_back(actionbtn);
	}
	g_MapBlockScene->showNewerGuideNode(step, nodes);
}

void GoBackLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();

		if (tag == 1)
		{
			if (MyRes::getMyResCount("t001") >= 1)
			{
				MyRes::Use("t001");
				if (m_forwhere == 0)
				{
#if USE_TRANSCENE
					Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
					Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif
					
				}
				else
				{
#if USE_TRANSCENE
					Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAP)));
#else
					Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
#endif
				}
			}
			else
			{
				int coin = GlobalInstance::map_AllResources["t001"].coinval;
				std::string s = StringUtils::format(ResourceLang::map_lang["gobakccostcoin"].c_str(), coin);
				HintBoxLayer* hint = HintBoxLayer::create(s, 6);
				this->addChild(hint);
				AnimationEffect::openAniEffect((Layer*)hint);
			}
		}
		else
		{
			AnimationEffect::closeAniEffect((Layer*)this);
		}
	}
}

void GoBackLayer::costCoinGoback()
{
	if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= gobackcoin.getValue())
	{
		DynamicValueInt dal;
		dal.setValue(gobackcoin.getValue());
		GlobalInstance::getInstance()->costMyCoinCount(dal);

		if (m_forwhere == 0)
		{
#if USE_TRANSCENE
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif
		}
		else
		{
#if USE_TRANSCENE
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAP)));
#else
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
#endif
		}
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
		Layer* layer = BuyCoinLayer::create(gobackcoin.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
		Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
	}
}

void GoBackLayer::onExit()
{
	Layer::onExit();
}