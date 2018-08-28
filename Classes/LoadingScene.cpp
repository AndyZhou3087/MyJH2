#include "LoadingScene.h"
#include "Resource.h"
#include "MainScene.h"
#include "DataSave.h"
#include "GlobalInstance.h"
#include "Building.h"

USING_NS_CC;

LoadingScene::LoadingScene()
{
	isGetPlayerId = false;
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

	Node *loadingbar = csbnode->getChildByName("loadingbar");
	loadingbar->runAction(RepeatForever::create(RotateTo::create(0.8f, -720)));


	for (int i = 0; i < 3; i++)
	{
		std::string pointstr = StringUtils::format("loadingpoint%d", i);
		point[i] = csbnode->getChildByName(pointstr);
		point[i]->setVisible(false);
	}
	showPointAnim(0);
	this->schedule(schedule_selector(LoadingScene::showPointAnim), 1.5f);
	//先获取服务器数据
	this->scheduleOnce(schedule_selector(LoadingScene::delayGetServerData), 0.1f);

    return true;
}

void LoadingScene::showPointAnim(float dt)
{
	for (int i = 0; i < 3; i++)
	{
		point[i]->runAction(Sequence::create(DelayTime::create(0.4f*i), Show::create(), DelayTime::create(1.2f - 0.4f*i), Hide::create(), NULL));
	}
}

void LoadingScene::delayLoadLocalData(float dt)
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

	GlobalInstance::getInstance()->parseTBoxJson();

	//加载事件数据
	GlobalInstance::getInstance()->loadEventData();

	//加载成就数据
	GlobalInstance::getInstance()->loadAchieveData();
	GlobalInstance::getInstance()->getMyAchieveData();

	//数据处理完，
	this->scheduleOnce(schedule_selector(LoadingScene::showNextScene), 0.1f);
}


void LoadingScene::delayGetServerData(float dt)
{
	isGetPlayerId = true;
	HttpDataSwap::init(this)->getPlayerId();
}

void LoadingScene::showNextScene(float dt)
{
	Director::getInstance()->replaceScene(MainScene::createScene());
}

void LoadingScene::onFinish(int errcode)
{
	bool isLoadLocal = false;
	if (errcode == 0)
	{
		if (isGetPlayerId)
		{
			isGetPlayerId = false;
			HttpDataSwap::init(this)->getAllData();
		}
		else
			isLoadLocal = true;
	}
	else//网络异常
	{
		if (!isGetPlayerId && (errcode == 3 || errcode == 4))
			isLoadLocal = true;
	}

	if (isLoadLocal)
	{
		//加载本地数据
		this->scheduleOnce(schedule_selector(LoadingScene::delayLoadLocalData), 0.1f);
	}
	else//数据错误
	{

	}
}

void LoadingScene::onExit()
{
	Layer::onExit();
}
