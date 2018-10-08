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
#include "MovingLabel.h"
#include "ErrorHintLayer.h"
#include "StoryScene.h"
#ifdef UMENG
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "iosfunc.h"
#endif
#endif

USING_NS_CC;

int skillEffectArr[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 17, 18, 20 };

LoadingScene* g_loadingScene = NULL;

LoadingScene::LoadingScene()
{
	isGetPlayerId = false;
}

LoadingScene::~LoadingScene()
{
	g_loadingScene = NULL;
}

Scene* LoadingScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	g_loadingScene = LoadingScene::create();

    // add layer as a child to scene
	scene->addChild(g_loadingScene);

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

	m_loadingbar = csbnode->getChildByName("loadingbar");
	m_loadingbar->setVisible(false);

	m_userpro = (cocos2d::ui::ImageView*)csbnode->getChildByName("userpro");
	m_userpro->addTouchEventListener(CC_CALLBACK_2(LoadingScene::onBtnClick, this));
	m_userpro->setTag(0);

	m_loadingbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("loadingbg");
	m_loadingbg->addTouchEventListener(CC_CALLBACK_2(LoadingScene::onBtnClick, this));
	m_loadingbg->setTag(1);
	m_loadingbg->setEnabled(false);

	m_loadingtext = csbnode->getChildByName("loadingtext");
	m_loadingtext->setVisible(false);

	m_loadingclicktext = csbnode->getChildByName("loadingclicktext");
	m_loadingclicktext->setVisible(false);

	//解析语言xml
	int langtype = DataSave::getInstance()->getLocalLang();
	GlobalInstance::getInstance()->setLang(langtype);

	ResourceLang::load(langtype);

	std::string wordstr = ResourceLang::map_lang["usertips0"];
	m_wordlbl = Label::createWithTTF(wordstr, FONT_NAME, 22);
	m_wordlbl->setColor(Color3B(255, 255, 255));
	m_wordlbl->setPosition(Vec2(360, 58));
	csbnode->addChild(m_wordlbl);
	DrawNode* underlineNode = DrawNode::create();
	m_wordlbl->addChild(underlineNode, 100000);
	underlineNode->setLineWidth(1.0f);
	underlineNode->drawLine(Vec2(0, 0), Vec2(m_wordlbl->getContentSize().width, 0), Color4F(m_wordlbl->getDisplayedColor()));

	std::u32string utf32String;
	StringUtils::UTF8ToUTF32(wordstr, utf32String);

	std::string resname = ResourceLang::map_lang["usertips1"];
	std::u32string tiputf32String;
	StringUtils::UTF8ToUTF32(resname, tiputf32String);
	std::size_t findpos = utf32String.find(tiputf32String);
	if (findpos != std::string::npos)
	{
		for (unsigned int i = findpos; i < findpos + tiputf32String.length(); i++)
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

	//未同意时弹出，同意后不再弹出
	if (!DataSave::getInstance()->getUserProtocal())
	{
		Layer *userProlayer = UserProtocolLayer::create();
		this->addChild(userProlayer, 0, "UserProtocolLayer");
		AnimationEffect::openAniEffect(userProlayer);
	}
	else
	{
		m_userpro->setVisible(false);
		m_wordlbl->setVisible(false);
	}
	//IOS第一次安装会有联网权限提示
	if (DataSave::getInstance()->getFirstEnter())
	{
		m_loadingbg->setEnabled(true);
		m_loadingclicktext->setVisible(true);
		m_loadingclicktext->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
	}
	else
	{
		loadData();
	}

    return true;
}

void LoadingScene::loadData()
{
	m_loadingclicktext->stopAllActions();
	m_loadingclicktext->setVisible(false);

	m_loadingtext->setVisible(true);
	m_loadingbar->setVisible(true);
	m_loadingbar->runAction(RepeatForever::create(RotateTo::create(0.8f, 720)));
	showPointAnim(0);
	this->schedule(schedule_selector(LoadingScene::showPointAnim), 1.5f);
	//先获取服务器数据
	if (DataSave::getInstance()->getFirstEnter())
		this->scheduleOnce(schedule_selector(LoadingScene::delayGetServerData), 0.1f);
	else
		loadLocalData();
}

void LoadingScene::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (node->getTag() == 0)
		{
			Layer *userProlayer = UserProtocolLayer::create();
			this->addChild(userProlayer, 0, "UserProtocolLayer");
			AnimationEffect::openAniEffect(userProlayer);
		}
		else
		{
			if (!DataSave::getInstance()->getUserProtocal())
			{
				MovingLabel::show(ResourceLang::map_lang["usertipshint"]);
			}
			else
			{
				m_loadingbg->setEnabled(false);
				m_userpro->setVisible(false);
				m_wordlbl->setVisible(false);
				loadData();
			}
		}
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

	//加载地图数据
	GlobalInstance::getInstance()->parseMapJson();

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
	if (DataSave::getInstance()->getFirstEnter())
	{
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, StoryScene::createScene()));
	}
	else
	{
		int index = -1;
		int STEPS[] = { FIRSTGUIDESTEP, SECONDGUIDESTEP, THRIDGUIDESTEP, MIDELEGUIDESTEP, FOURTHGUIDESTEP };
		for (int i = 0; i < sizeof(STEPS) / sizeof(STEPS[0]); i++)
		{
			if (NewGuideLayer::checkifNewerGuide(STEPS[i]))
			{
				NewGuideLayer::setNewGuideInfo(STEPS[i]);
				index = i;
				break;
			}
		}
		if (index == 0)
		{
			Director::getInstance()->replaceScene(TransitionFade::create(2.2f, MapBlockScene::createScene("m0-0-0", 1)));
		}
		else if (index > 0)
		{
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
		}
		else
		{
			int exitscene = DataSave::getInstance()->getExitScene();
			if (exitscene == 0)
			{
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
			}
			else if (exitscene == 1)
			{
				GlobalInstance::myOutMapCarry = DataSave::getInstance()->getHeroMapCarryCount();
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
			}
			else if (exitscene == 2)
			{
				GlobalInstance::myOutMapCarry = DataSave::getInstance()->getHeroMapCarryCount();
				std::string str = DataSave::getInstance()->getMapScenePos();
				std::vector<std::string> vec_map;
				CommonFuncs::split(str, vec_map, ",");
				Director::getInstance()->replaceScene(TransitionFade::create(2.2f, MapBlockScene::createScene(vec_map[0], 1)));
			}
		}
	}
}

void LoadingScene::showNextScene(float dt)
{
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
	bool isDataOk = false;
	if (errcode == 0)
	{
		if (isGetPlayerId)
		{
			isGetPlayerId = false;
			HttpDataSwap::init(this)->getAllData();
		}
		else
			isLoadLocal = true;
		isDataOk = true;
	}
	else//网络异常
	{
		if (!isGetPlayerId && (errcode == 3 || errcode == 4))
		{
			isLoadLocal = true;
			isDataOk = true;
		}
	}

	if (!isDataOk)//数据错误
	{
		ErrorHintLayer* layer = (ErrorHintLayer*)this->getChildByName("networkerrlayer");
		if (layer == NULL)
		{
			layer = ErrorHintLayer::create(0);
			this->addChild(layer, 0, "networkerrlayer");
			AnimationEffect::openAniEffect(layer);
		}
		else
		{
			layer->resetBtn();
		}
		return;
	}
	if (isLoadLocal)
	{
		loadLocalData();
	}
}

void LoadingScene::loadLocalData()
{
	//加载本地数据
	this->scheduleOnce(schedule_selector(LoadingScene::delayLoadLocalData), 0.1f);
#ifdef UMENG
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	UMengInit();
#endif
#endif
}

void LoadingScene::onExit()
{
	Layer::onExit();
}
