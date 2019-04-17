#include "MapZoomGuideLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "Resource.h"
#include "MapBlockScene.h"


MapZoomGuideLayer::MapZoomGuideLayer()
{

}


MapZoomGuideLayer::~MapZoomGuideLayer()
{
}

MapZoomGuideLayer* MapZoomGuideLayer::create()
{
	MapZoomGuideLayer *pRet = new(std::nothrow)MapZoomGuideLayer();
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
bool MapZoomGuideLayer::init()
{
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode("mapZoomGuideLayer.csb");
	this->addChild(csbnode);

	auto action = CSLoader::createTimeline("mapZoomGuideLayer.csb");
	csbnode->runAction(action);
	action->gotoFrameAndPlay(0, true);
	////layer 点击事件，屏蔽下层事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		this->removeFromParentAndCleanup(true);
		GlobalInstance::getInstance()->saveNewerGuide(0, false);
		g_MapBlockScene->zoomGuideEnd();
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}