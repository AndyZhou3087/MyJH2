#include "MainMapScene.h"
#include "Resource.h"

MainMapScene::MainMapScene()
{

}


MainMapScene::~MainMapScene()
{

}

Scene* MainMapScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto mainLayer = MainMapScene::create();

	// add layer as a child to scene
	scene->addChild(mainLayer);

	// return the scene
	return scene;
}

bool MainMapScene::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mainMapLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ScrollView* mapscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("mapscroll");
	mapscroll->setScrollBarEnabled(false);
	mapscroll->setSwallowTouches(false);

	cocos2d::ui::Widget* mapbg = (cocos2d::ui::Widget*)mapscroll->getChildByName("mapbg");

	int mapnamecount = mapbg->getChildrenCount();

	for (int i = 0; i < mapnamecount; i++)
	{
		cocos2d::ui::Widget* mapname = (cocos2d::ui::Widget*)mapbg->getChildren().at(i);
		mapname->addTouchEventListener(CC_CALLBACK_2(MainMapScene::onclick, this));
		mapname->setSwallowTouches(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		mapname->setVisible(true);
#else
		mapname->setVisible(false);
#endif
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		m_isDraging = false;
		m_startClickX = touch->getLocation().x;
		m_startClickY = touch->getLocation().y;
		return true;
	};

	listener->onTouchMoved = [=](Touch *touch, Event *event)
	{
		if (fabsf(m_startClickX - touch->getLocation().x) > 20 || fabsf(m_startClickY - touch->getLocation().y) > 20)
			m_isDraging = true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void MainMapScene::onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_isDraging)
			return;
	}
}