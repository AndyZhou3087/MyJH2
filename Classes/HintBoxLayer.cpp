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

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("hitBoxLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//按钮1
	cocos2d::ui::Widget* okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
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

	/*if (forwhere == 4)
	{
		if (NewGuideLayer::checkifNewerGuide(FIRSTGUIDESTEP))
		{
			if (g_MapBlockScene != NULL)
			{
				std::vector<Node*> nodes;
				okbtn->setTag(-1);
				nodes.push_back(okbtn);
				g_MapBlockScene->showNewerGuideNode(FIRSTGUIDESTEP, nodes);
			}
		}
	}*/

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
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
				InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
				if (innroomLayer != NULL)
					innroomLayer->fireHero(this->getTag());
			}
			else if (m_forwhere == 3)
			{
				MyHeroNode* myheronode = (MyHeroNode*)this->getUserData();
				myheronode->cacelTraining();
			}
			else if (m_forwhere == 4)
			{
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
			}
			AnimationEffect::closeAniEffect((Layer*)this);
		case -1:
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
			break;
		default:
			break;
		}
	}
}