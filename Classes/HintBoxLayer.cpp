#include "HintBoxLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "RandHeroLayer.h"
#include "InnRoomLayer.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "MyHeroNode.h"
#include "MainMapScene.h"
#include "MapBlockScene.h"
#include "NewGuideLayer.h"
#include "StoreHouseLayer.h"
#include "HeroAttrLayer.h"
#include "GoBackLayer.h"
#include "MyTransitionScene.h"
#include "LoadingScene.h"
#include "SettingLayer.h"
#include "BuyCoinLayer.h"
#include "MazeTransitionScene.h"

USING_NS_CC;

HintBoxLayer::HintBoxLayer()
{

}

HintBoxLayer::~HintBoxLayer()
{

}


HintBoxLayer* HintBoxLayer::create(std::string str, int forwhere)
{
	HintBoxLayer *pRet = new(std::nothrow)HintBoxLayer();
	if (pRet && pRet->init(str, forwhere))
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
bool HintBoxLayer::init(std::string str, int forwhere)
{
	if (!Layer::init())
	{
		return false;
	}

	m_forwhere = forwhere;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("hitBoxLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//按钮1
	okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(HintBoxLayer::onBtnClick, this));
	okbtn->setTag(1);
	//按钮1文字
	cocos2d::ui::ImageView* okbtntxt = (cocos2d::ui::ImageView*)okbtn->getChildByName("text");
	okbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(HintBoxLayer::onBtnClick, this));
	cancelbtn->setTag(0);
	//按钮2文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* content = (cocos2d::ui::Text*)csbnode->getChildByName("content");
	content->setString(str);

	Label* dlbl = (Label*)content->getVirtualRenderer();
	dlbl->setLineSpacing(10);

	if (forwhere == 4)
	{
		okbtn->setPositionX(490);
		cancelbtn->setPositionX(225);
	}
	else if (forwhere == 10 || forwhere == 11)
	{
		okbtn->setPositionX(360);
		cancelbtn->setVisible(false);
	}
	else if (m_forwhere == 12)//招募
	{
		this->scheduleOnce(schedule_selector(HintBoxLayer::delayShowNewerGuide), 0.31f);
	}

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		if (forwhere == 4 || forwhere == 11)
		{
			return;
		}
		else
			AnimationEffect::closeAniEffect(this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void HintBoxLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 0:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1:
			if (m_forwhere == 1)
			{
				RandHeroLayer* hlayer = (RandHeroLayer*)this->getParent();
				if (hlayer!=NULL)
				{
					hlayer->refresh3Hero(this->getTag());
				}
			}
			else if (m_forwhere == 2)
			{
				HeroAttrLayer* heroattrlayer = (HeroAttrLayer*)g_mainScene->getChildByName("heroattrlayer");
				if (heroattrlayer == NULL)
				{
					InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
					if (innroomLayer != NULL)
						innroomLayer->fireHero(this->getTag());
				}
				else
				{
					heroattrlayer->fireHero();
					return;
				}

			}
			else if (m_forwhere == 3)
			{
				MyHeroNode* myheronode = (MyHeroNode*)this->getUserData();
				myheronode->cacelTraining();
			}
			else if (m_forwhere == 4)
			{
#if USE_TRANSCENE
				Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAP)));
#else
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
#endif
			}
			else if (m_forwhere == 5 || m_forwhere == 7)
			{
				StoreHouseLayer* storelayer = (StoreHouseLayer*)g_mainScene->getChildByName("3storehouse");
				if (storelayer != NULL)
				{
					storelayer->decompose((ResBase*)this->getUserData());
					AnimationEffect::closeAniEffect((Layer*)this);
					if (storelayer->getChildByTag(1111) != NULL)
						AnimationEffect::closeAniEffect((Layer*)storelayer->getChildByTag(1111));
					return;
				}
			}
			else if (m_forwhere == 6)
			{
				GoBackLayer* golayer = (GoBackLayer*)this->getParent();
				if (golayer != NULL)
				{
					golayer->costCoinGoback(this->getTag());
				}
				return;
			}
			else if (m_forwhere == 8)
			{
				AnimationEffect::closeAniEffect((Layer*)this);
				GlobalInstance::isResetData = true;

				Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LoadingScene::createScene()));
				return;
			}
			else if (m_forwhere == 9)
			{
				SettingLayer* settinglayer = (SettingLayer*)this->getParent();
				if (settinglayer != NULL)
				{
					settinglayer->modifyNameCount();
				}
			}
			else if (m_forwhere == 11)
			{
#if USE_TRANSCENE
				Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif
			}
			else if (m_forwhere == 12)
			{
				HeroAttrLayer* heroattrlayer = (HeroAttrLayer*)this->getParent();
				if (heroattrlayer != NULL)
				{
					heroattrlayer->recruitHero();

					return;
				}
			}
			else if (m_forwhere == 13)
			{
				if (g_MapBlockScene != NULL)
				{
					int constcoin = 30;
					if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= constcoin)
					{
						DynamicValueInt dv;
						dv.setValue(constcoin);
						GlobalInstance::getInstance()->costMyCoinCount(dv);
						Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MazeTransitionScene::createScene(0, TO_EXIT)));
						return;
					}
					else
					{
						Layer* layer = BuyCoinLayer::create(constcoin - GlobalInstance::getInstance()->getMyCoinCount().getValue());
						Director::getInstance()->getRunningScene()->addChild(layer, 10000, "buycoinlayer");
						return;
					}
				}
			}
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case -1:
#if USE_TRANSCENE
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif
			break;
		default:
			break;
		}
	}
}

void HintBoxLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(25) && NewGuideLayer::checkifNewerGuide(88))
	{
		showNewerGuide(88);
	}
	else if (!NewGuideLayer::checkifNewerGuide(27) && NewGuideLayer::checkifNewerGuide(89))
	{
		showNewerGuide(89);
	}
	else if (!NewGuideLayer::checkifNewerGuide(29) && NewGuideLayer::checkifNewerGuide(90))
	{
		showNewerGuide(90);
	}
}

void HintBoxLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	nodes.push_back(okbtn);
	g_mainScene->showNewerGuideNode(step, nodes);
}