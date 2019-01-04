#include "MyTransitionScene.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MyActionProgressTimer.h"
#include "MainMapScene.h"
#include "MainScene.h"

MyTransitionScene::MyTransitionScene()
{

}


MyTransitionScene::~MyTransitionScene()
{

}

Scene* MyTransitionScene::createScene(int type)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MyTransitionScene::create(type);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void MyTransitionScene::onExit()
{
	Layer::onExit();
}

MyTransitionScene* MyTransitionScene::create(int type)
{
	MyTransitionScene *pRet = new(std::nothrow)MyTransitionScene();

	if (pRet && pRet->init(type))
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

bool MyTransitionScene::init(int type)
{
	if (!Layer::init())
	{
		return false;
	}

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("transitionLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* img = (cocos2d::ui::ImageView*)csbnode->getChildByName("img");
	img->ignoreContentAdaptWithSize(true);

	int r = GlobalInstance::getInstance()->createRandomNum(2);

	std::string str = StringUtils::format("tansitionimg%d.jpg", r); 
	str = ResourcePath::makeImagePath(str);
	img->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);

	cocos2d::ui::Text* tipslbl = (cocos2d::ui::Text*)csbnode->getChildByName("tips");

	r = GlobalInstance::getInstance()->createRandomNum(TIPSCOUNT);
	std::string keystr = StringUtils::format("tips_%d", r);
	tipslbl->setString(ResourceLang::map_lang[keystr]);
    

	loadingbar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("loadingbar");
	MyProgressTo * to = MyProgressTo::create(3.0f, 100);
	loadingbar->runAction(Sequence::create(to, CallFunc::create(CC_CALLBACK_0(MyTransitionScene::loadfinish, this, type)), NULL));

	horse = csbnode->getChildByName("tansitiohorse");

	schedule(schedule_selector(MyTransitionScene::updateHorse));

	return true;
}

void MyTransitionScene::updateHorse(float dt)
{
	horse->setPositionX(loadingbar->getPositionX() + loadingbar->getContentSize().width*loadingbar->getPercent() / 100);
}

void MyTransitionScene::loadfinish(int type)
{
	if (type == TO_MAP)
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MainMapScene::createScene()));
	else if (type == TO_MAIN)
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MainScene::createScene()));
}