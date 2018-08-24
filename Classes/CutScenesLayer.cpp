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

int reloadArr[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 17, 19, 20 };

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

	std::string str = StringUtils::format("effect/skill%dpacker.plist", reloadArr[sizeof(reloadArr) / sizeof(reloadArr[0]) - 1]);
	if (SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(str))
	{
		fdt = 1.3f;
	}
	else
	{
		fdt = 0.01f;
	}

	curReloadPlistNum = 0;
	str = StringUtils::format("effect/skill%dpacker.png", reloadArr[curReloadPlistNum]);
	Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(CutScenesLayer::loadingOver, this));

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
	std::string str = StringUtils::format("effect/skill%dpacker.plist", reloadArr[curReloadPlistNum]);
	if (!SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(str))
	{
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(str, texture);
	}
	if (curReloadPlistNum == sizeof(reloadArr) / sizeof(reloadArr[0]))
	{
		this->scheduleOnce(schedule_selector(CutScenesLayer::delayShowResult), fdt);
		return;
	}
	curReloadPlistNum++;
	str = StringUtils::format("effect/skill%dpacker.png", reloadArr[curReloadPlistNum]);
	Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(CutScenesLayer::loadingOver, this));
}

void CutScenesLayer::delayShowResult(float dt)
{
	effectnode->runAction(Sequence::create(FadeOut::create(0.2f), CallFunc::create(CC_CALLBACK_0(CutScenesLayer::updateFadeOut, this)), NULL));
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