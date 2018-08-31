#include "FightingResultLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "FightHeroNode.h"
#include "WinRewardLayer.h"
#include "MyRes.h"
#include "MainScene.h"
#include "SoundManager.h"
#include "AnimationEffect.h"

USING_NS_CC;

FightingResultLayer::FightingResultLayer()
{

}

FightingResultLayer::~FightingResultLayer()
{

}


FightingResultLayer* FightingResultLayer::create(std::vector<FOURProperty> reward_res, int winexp)
{
	FightingResultLayer *pRet = new(std::nothrow)FightingResultLayer();
	if (pRet && pRet->init(reward_res, winexp))
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
bool FightingResultLayer::init(std::vector<FOURProperty> reward_res, int winexp)
{
	if (!Layer::init())
	{
		return false;
	}
	m_rewards = reward_res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("fightResultLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(FightingResultLayer::onBtnClick, this));
	actionbtn->setTag(1000);

	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");

	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("continuebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	SoundManager::getInstance()->stopBackMusic();

	FIGHTNODE_STATE fs = FS_SUCC;
	Node* effectnode;
	if (winexp <= 0)
	{
		fs = FS_FAIL;
		actionbtn->setTag(1001);
		actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		MyRes::clearMyPackages();

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FAIL);

		//特效
		effectnode = CSLoader::createNode("effect/slsb/zhandoushibai.csb");
		effectnode->setPosition(Vec2(360, 1010));
		this->addChild(effectnode);
		auto action = CSLoader::createTimeline("effect/slsb/zhandoushibai.csb");
		effectnode->runAction(action);
		action->gotoFrameAndPlay(0, false);
	}
	else
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_WIN);

		effectnode = CSLoader::createNode("effect/slsb/zhandoushengli.csb");
		effectnode->setPosition(Vec2(360, 1010));
		this->addChild(effectnode);
		auto action = CSLoader::createTimeline("effect/slsb/zhandoushengli.csb");
		effectnode->runAction(action);
		action->gotoFrameAndPlay(0, false);
	}

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(145 + i%3*215, 680 -  i/3*280);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i], F_HERO, fs);
		addChild(fightHeroNode, 0, i);
		if (GlobalInstance::myCardHeros[i] != NULL)
			fightHeroNode->setFightState(winexp);
	}

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void FightingResultLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (node->getTag() == 1000)
		{
			node->setVisible(false);
			WinRewardLayer* layer = WinRewardLayer::create(m_rewards);
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		else
		{
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
		}
	}
}

void FightingResultLayer::onExit()
{
	Layer::onExit();
}