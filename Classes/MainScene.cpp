#include "MainScene.h"
#include "ResourcePath.h"
#include "MainMenuLayer.h"

USING_NS_CC;

MainScene::MainScene()
{

}

MainScene::~MainScene()
{
	
}

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	auto mainLayer = MainScene::create();

    // add layer as a child to scene
	scene->addChild(mainLayer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("MainLayer.csb"));
	this->addChild(csbnode);

	Node* menunode = MainMenuLayer::create();
	this->addChild(menunode);

	scroll_3 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_3");
	scroll_3->setScrollBarEnabled(false);
	scroll_3->jumpToPercentHorizontal(32);
	scroll_3->addEventListener(CC_CALLBACK_2(MainScene::srollviewlistenEvent, this));
	scroll_3->setSwallowTouches(false);

	scroll_2 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_2");
	scroll_2->setScrollBarEnabled(false);
	scroll_2->jumpToPercentHorizontal(32);
	scroll_2->setSwallowTouches(false);

	scroll_1 = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scroll_1");
	scroll_1->setScrollBarEnabled(false);
	scroll_1->jumpToPercentHorizontal(32);

	for (int i = 1; i <= 10; i++)
	{
		std::string buidingNomalName;
		std::string buidingSelectName;
		cocos2d::ui::ImageView* buildingNomal;
		cocos2d::ui::ImageView* buildingSelect;
		if (i <= 5)
		{
			buidingNomalName = StringUtils::format("main_%02d_n", i);
			buildingNomal = (cocos2d::ui::ImageView*)scroll_3->getChildByName(buidingNomalName);

			buidingSelectName = StringUtils::format("main_%02d_s", i);
			buildingSelect = (cocos2d::ui::ImageView*)scroll_3->getChildByName(buidingSelectName);
		}

		else if (i <= 8)
		{
			buidingNomalName = StringUtils::format("main_%02d_n", i);
			buildingNomal = (cocos2d::ui::ImageView*)scroll_2->getChildByName(buidingNomalName);
			buidingSelectName = StringUtils::format("main_%02d_s", i);
			buildingSelect = (cocos2d::ui::ImageView*)scroll_2->getChildByName(buidingSelectName);
		}
		else
		{
			buidingNomalName = StringUtils::format("main_%02d_n", i);
			buildingNomal = (cocos2d::ui::ImageView*)scroll_1->getChildByName(buidingNomalName);
			buidingSelectName = StringUtils::format("main_%02d_s", i);
			buildingSelect = (cocos2d::ui::ImageView*)scroll_1->getChildByName(buidingSelectName);
		}
		buildingNomal->setSwallowTouches(true);
		buildingNomal->setUserData((void*)buildingSelect);
		buildingNomal->addTouchEventListener(CC_CALLBACK_2(MainScene::onBuildingClick, this));
		buildingSelect->setVisible(false);
	}

    return true;
}

void MainScene::srollviewlistenEvent(Ref* ref, ui::ScrollView::EventType eventType)
{
	Vec2 pos = scroll_3->getInnerContainerPosition();
	switch (eventType) 
	{
		case ui::ScrollView::EventType::CONTAINER_MOVED:
			scroll_2->startAutoScrollToDestination(pos, 0.1f, true);
			scroll_1->startAutoScrollToDestination(pos, 0.2f, true);
			break;
		default:
			break;
	}
}

void MainScene::onBuildingClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	Node* snode = (Node*)clicknode->getUserData();
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		snode->setVisible(true);
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		snode->setVisible(false);
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		snode->setVisible(false);
		break;
	default:
		break;
	}
}

void MainScene::onExit()
{
	Layer::onExit();
}
