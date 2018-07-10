#include "LoadingScene.h"
#include "Resource.h"
#include "MainScene.h"
#include "DataSave.h"
#include "GlobalInstance.h"
#include "Building.h"

USING_NS_CC;

LoadingScene::LoadingScene()
{

}

LoadingScene::~LoadingScene()
{
	
}

Scene* LoadingScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	auto mainLayer = LoadingScene::create();

    // add layer as a child to scene
	scene->addChild(mainLayer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoadingScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("LoadingLayer.csb"));
	this->addChild(csbnode);

	this->scheduleOnce(schedule_selector(LoadingScene::delayLoadData), 0.2f);

    return true;
}


void LoadingScene::delayLoadData(float dt)
{
	GlobalInstance::getInstance()->loadInitData();
	int langtype = DataSave::getInstance()->getLocalLang();
	GlobalInstance::getInstance()->setLang(langtype);

	//解析语言xml
	ResourceLang::load(langtype);

	//解析建筑物xml
	Building::parseData();

	//英雄基础属性
	GlobalInstance::getInstance()->loadHerosAttrData();

	//读取本地英雄
	GlobalInstance::getInstance()->loadMyHeros();

	GlobalInstance::getInstance()->loadRand3Heros();

	GlobalInstance::getInstance()->loadResCreatorData();

	GlobalInstance::getInstance()->loadAllResourcesData();

	GlobalInstance::getInstance()->loadMyResData();

	GlobalInstance::getInstance()->parseSuitJson();
	GlobalInstance::getInstance()->loadEquipData();

	GlobalInstance::getInstance()->loadGFData();

	GlobalInstance::getInstance()->loadNpcData();

	//读取主线任务配置
	GlobalInstance::getInstance()->loadTaskMainData();
	//读取主线任务
	GlobalInstance::getInstance()->loadMyTaskMainData();

	//读取支线任务配置
	GlobalInstance::getInstance()->loadTaskBranchData();
	//读取支线任务
	GlobalInstance::getInstance()->loadMyTaskBranchData();

	//读取每日任务配置
	GlobalInstance::getInstance()->loadDailyTaskData();
	//读取每日任务数据
	GlobalInstance::getInstance()->getMyDailyTaskData();

	//数据处理完，显示游戏场景
	this->scheduleOnce(schedule_selector(LoadingScene::showNextScene), 0.2f);
}

void LoadingScene::showNextScene(float dt)
{
	Director::getInstance()->replaceScene(MainScene::createScene());
}

void LoadingScene::onExit()
{
	Layer::onExit();
}
