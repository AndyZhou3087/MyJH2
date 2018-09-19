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

	//放入loading界面加载
	//curReloadPlistNum = 0;
	//std::string str = StringUtils::format("effect/skill%dpacker.png", reloadArr[curReloadPlistNum]);
	//Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(CutScenesLayer::loadingOver, this));

	int r = GlobalInstance::getInstance()->createRandomNum(5);
	SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_FIGHT_0 + r);

	this->scheduleOnce(schedule_selector(CutScenesLayer::delayShowResult), 1.5f);
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
//放入loading界面加载
//void CutScenesLayer::loadingOver(cocos2d::Texture2D* texture)
//{
//	float fdt;
//	//传入的obj即是异步生成的纹理
//	std::string str = StringUtils::format("effect/skill%dpacker.plist", reloadArr[curReloadPlistNum]);
//	if (!SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(str))
//	{
//		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(str, texture);
//		fdt = 0.01f;
//	}
//	else
//	{
//		fdt = 1.5f;
//	}
//	curReloadPlistNum++;
//	if (curReloadPlistNum == sizeof(reloadArr) / sizeof(reloadArr[0]))
//	{
//		this->scheduleOnce(schedule_selector(CutScenesLayer::delayShowResult), fdt);
//		return;
//	}

//	str = StringUtils::format("effect/skill%dpacker.png", reloadArr[curReloadPlistNum]);
//	Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(CutScenesLayer::loadingOver, this));
//}

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
		mapblock->addChild(layer, 0, "FightingLayer");
	}
	this->removeFromParentAndCleanup(true);
}