#include "LoadingScene.h"
#include "Resource.h"
#include "MainScene.h"
#include "DataSave.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "DataSave.h"
#include "MapBlockScene.h"
#include "NewGuideLayer.h"
#include "AnimationEffect.h"
#include "Const.h"
#include "CommonFuncs.h"
#include "MainMapScene.h"

USING_NS_CC;

int skillEffectArr[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 17, 18, 20 };

LoadingScene::LoadingScene()
{
	isGetPlayerId = false;
	protocal = -1;
	loadSuccess = false;
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
	loadingbar->runAction(RepeatForever::create(RotateTo::create(0.8f, 720)));

	cocos2d::ui::ImageView* userpro = (cocos2d::ui::ImageView*)csbnode->getChildByName("userpro");
	userpro->addTouchEventListener(CC_CALLBACK_2(LoadingScene::onBtnClick, this));

	//解析语言xml
	int langtype = DataSave::getInstance()->getLocalLang();
	GlobalInstance::getInstance()->setLang(langtype);

	ResourceLang::load(langtype);

	std::string wordstr = ResourceLang::map_lang["usertips0"];
	Label* m_wordlbl = Label::createWithTTF(wordstr, FONT_NAME, 22);
	m_wordlbl->setColor(Color3B(255, 255, 255));
	m_wordlbl->setPosition(Vec2(360, 58));
	csbnode->addChild(m_wordlbl);
	DrawNode* underlineNode = DrawNode::create();
	m_wordlbl->addChild(underlineNode, 100000);
	underlineNode->setLineWidth(1.0f);
	underlineNode->drawLine(Vec2(0, 0), Vec2(m_wordlbl->getContentSize().width, 0), Color4F(m_wordlbl->getDisplayedColor()));

	std::u32string utf32String;
	StringUtils::UTF8ToUTF32(m_wordlbl->getString(), utf32String);

	std::string resname = ResourceLang::map_lang["usertips1"];
	std::u32string m_utf32String;
	StringUtils::UTF8ToUTF32(resname, m_utf32String);
	std::size_t findpos = utf32String.find(m_utf32String);
	if (findpos != std::string::npos)
	{
		int len = m_utf32String.length();
		for (int i = findpos; i < findpos + len; i++)
		{
			m_wordlbl->getLetter(i)->setColor(Color3B(255, 194, 99));
		}
	}

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

	//未同意时弹出，同意后不再弹出
	if (!DataSave::getInstance()->getUserProtocal())
	{
		userProlayer = UserProtocolLayer::create();
		this->addChild(userProlayer, 0, "UserProtocolLayer");
		AnimationEffect::openAniEffect((Layer*)userProlayer);
	}
	else
	{
		userpro->setVisible(false);
		m_wordlbl->setVisible(false);
	}

    return true;
}

void LoadingScene::setUserProtocol(int ar)
{
	protocal = ar;
	DataSave::getInstance()->setUserProtocal(protocal);
	if (loadSuccess && protocal == 1)
	{
		enterNewScene();
	}
}

void LoadingScene::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		userProlayer = UserProtocolLayer::create();
		this->addChild(userProlayer, 0, "UserProtocolLayer");
		AnimationEffect::openAniEffect((Layer*)userProlayer);
	}
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
	int langtype = GlobalInstance::getInstance()->getLang();

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

	//加载商城
	GlobalInstance::getInstance()->loadShopData();

	//数据处理完，
	//加载技能特效
	curEffectPlistNum = 0;
	std::string str = StringUtils::format("effect/skill%dpacker.png", skillEffectArr[curEffectPlistNum]);
	Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(LoadingScene::loadingSkillEffectOver, this));
	//this->scheduleOnce(schedule_selector(LoadingScene::showNextScene), 0.1f);
}


void LoadingScene::delayGetServerData(float dt)
{
	isGetPlayerId = true;
	HttpDataSwap::init(this)->getPlayerId();
}

void LoadingScene::enterNewScene()
{
	if (NewGuideLayer::checkifNewerGuide(FIRSTGUIDESTEP))
	{
		NewGuideLayer::setNewGuideInfo(FIRSTGUIDESTEP);
		Director::getInstance()->replaceScene(TransitionFade::create(2.2f, MapBlockScene::createScene("m0-0-0", 1)));
	}
	else
	{
		if (NewGuideLayer::checkifNewerGuide(SECONDGUIDESTEP))
		{
			NewGuideLayer::setNewGuideInfo(SECONDGUIDESTEP);
		}
		else if (NewGuideLayer::checkifNewerGuide(THRIDGUIDESTEP))
		{
			NewGuideLayer::setNewGuideInfo(THRIDGUIDESTEP);
		}
		else if (NewGuideLayer::checkifNewerGuide(MIDELEGUIDESTEP))
		{
			NewGuideLayer::setNewGuideInfo(MIDELEGUIDESTEP);
		}
		else if (NewGuideLayer::checkifNewerGuide(FOURTHGUIDESTEP))
		{
			NewGuideLayer::setNewGuideInfo(FOURTHGUIDESTEP);
		}

		if (DataSave::getInstance()->getExitScene() == 0)
		{
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
		}
		else if (DataSave::getInstance()->getExitScene() == 1)
		{
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
		}
		else if (DataSave::getInstance()->getExitScene() == 2)
		{
			std::string str = DataSave::getInstance()->getMapScenePos();
			std::vector<std::string> vec_map;
			CommonFuncs::split(str, vec_map, ",");
			Director::getInstance()->replaceScene(TransitionFade::create(2.2f, MapBlockScene::createScene(vec_map[0], 1)));
		}
	}
}

void LoadingScene::showNextScene(float dt)
{
	loadSuccess = true;
	if ((userProlayer != NULL || protocal == 0) && !DataSave::getInstance()->getUserProtocal())
	{
		return;
	}
	enterNewScene();
}

void LoadingScene::loadingSkillEffectOver(cocos2d::Texture2D* texture)
{
	//传入的obj即是异步生成的纹理
	std::string str = StringUtils::format("effect/skill%dpacker.plist", skillEffectArr[curEffectPlistNum]);
	if (!SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(str))
	{
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile(str, texture);
	}
	curEffectPlistNum++;

	if (curEffectPlistNum == sizeof(skillEffectArr) / sizeof(skillEffectArr[0]))
	{
		this->scheduleOnce(schedule_selector(LoadingScene::showNextScene), 0.1f);
		return;
	}

	str = StringUtils::format("effect/skill%dpacker.png", skillEffectArr[curEffectPlistNum]);
	Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(LoadingScene::loadingSkillEffectOver, this));
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
