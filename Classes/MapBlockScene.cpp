#include "MapBlockScene.h"
#include "Resource.h"
#include "FightHeroNode.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "CommonFuncs.h"
#include "MainScene.h"

MapBlockScene::MapBlockScene()
{

}


MapBlockScene::~MapBlockScene()
{

}

Scene* MapBlockScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto mapLayer = MapBlockScene::create();

	// add layer as a child to scene
	scene->addChild(mapLayer);

	// return the scene
	return scene;
}

bool MapBlockScene::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mapBlockLayer.csb"));
	this->addChild(csbnode);

	carrycountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrycount");
	foodcountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("r001count");
	solivercountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("solivercountlbl");

	updateLabel();

	cocos2d::ui::Widget* gocitybtn = (cocos2d::ui::Widget*)csbnode->getChildByName("gocitybtn");
	gocitybtn->setTag(BTN_GOCITY);
	gocitybtn->addTouchEventListener(CC_CALLBACK_2(MapBlockScene::onBtnClick, this));

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();
		fightHeroNode->setPosition(60 + 120 * i, 1196);
		fightHeroNode->setScale(0.84f);
		fightHeroNode->setData(GlobalInstance::myCardHeros[i]);
		addChild(fightHeroNode);
	}

	return true;
}

void MapBlockScene::updateLabel()
{
	std::string str = StringUtils::format("%d/%d", MyRes::getMyPackageResCount(), GlobalInstance::getInstance()->getTotalCaryy());
	carrycountlbl->setString(str);
	str = StringUtils::format("%d", MyRes::getMyResCount("r001"), MYPACKAGE);
	foodcountlbl->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	foodcountlbl->setString(str);
}

void MapBlockScene::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case BTN_GOCITY:
			Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MainScene::createScene()));
			break;
		default:
			break;
		}
	}
}