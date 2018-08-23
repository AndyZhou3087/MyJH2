#include "CutScenesLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "DataSave.h"
#include "SoundManager.h"
#include "MapBlockScene.h"
#include "FightingLayer.h"

USING_NS_CC;

CutScenesLayer::CutScenesLayer()
{
	curReloadPlistNum = 0;
}

CutScenesLayer::~CutScenesLayer()
{

}


CutScenesLayer* CutScenesLayer::create(std::vector<Npc*> enemyHeros, int bgtype)
{
	CutScenesLayer *pRet = new(std::nothrow)CutScenesLayer();
	if (pRet && pRet->init(enemyHeros, bgtype))
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
bool CutScenesLayer::init(std::vector<Npc*> enemyHeros, int bgtype)
{
	if (!Layer::init())
	{
		return false;
	}

	enemys = enemyHeros;
	m_fightbgtype = bgtype;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//特效
	effectnode = CSLoader::createNode("effect/zhandou/zyzd.csb");
	effectnode->setPosition(Vec2(0, 0));
	this->addChild(effectnode);

	auto action = CSLoader::createTimeline("effect/zhandou/zyzd.csb");
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);

	std::string str;
	curReloadPlistNum = 0;
	for (int i = 0; i < 20; i++)
	{
		str = StringUtils::format("effect/skill%dpacker.png", i + 1);
		curReloadPlistNum++;
		if (i + 1 != 11 && i + 1 != 14 && i + 1 != 16 && i + 1 != 19)
		{
			Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(CutScenesLayer::loadingOver, this));
		}
	}

	this->scheduleOnce(schedule_selector(CutScenesLayer::delayShowResult), 1.0f);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void CutScenesLayer::loadingOver(cocos2d::Texture2D* texture)
{
	//传入的obj即是异步生成的纹理
	std::string str = StringUtils::format("effect/skill%dpacker.plist", curReloadPlistNum);
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(str, texture);
}

void CutScenesLayer::delayShowResult(float dt)
{
	effectnode->runAction(Sequence::create(FadeOut::create(1), CallFunc::create(CC_CALLBACK_0(CutScenesLayer::updateFadeOut, this)), NULL));
}

void CutScenesLayer::updateFadeOut()
{
	MapBlockScene* mapblock = (MapBlockScene*)this->getParent();
	if (mapblock!=NULL)
	{
		FightingLayer* layer = FightingLayer::create(enemys, m_fightbgtype);
		mapblock->addChild(layer);
	}
	this->removeFromParentAndCleanup(true);
}