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
#include "HintBoxLayer.h"
#include "SoundManager.h"
#include "MyRes.h"
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
	GlobalInstance::legalcopyurl = "";
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

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("LoadingLayer.csb"));
	this->addChild(csbnode);

	m_loadingbar = csbnode->getChildByName("loadingbar");
	m_loadingbar->setVisible(false);

	m_userpro = (cocos2d::ui::ImageView*)csbnode->getChildByName("userpro");
	//m_userpro->addTouchEventListener(CC_CALLBACK_2(LoadingScene::onBtnClick, this));
	//m_userpro->setTag(0);

	cocos2d::ui::ImageView* textclick = (cocos2d::ui::ImageView*)m_userpro->getChildByName("textclick");
	textclick->addTouchEventListener(CC_CALLBACK_2(LoadingScene::onBtnClick, this));
	textclick->setTag(0);

	m_loadingbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("loadingbg");
	m_loadingbg->addTouchEventListener(CC_CALLBACK_2(LoadingScene::onBtnClick, this));
	m_loadingbg->setTag(1);
	m_loadingbg->setEnabled(false);

	m_loadingtext = csbnode->getChildByName("loadingtext");
	m_loadingtext->setVisible(false);

	m_loadingclicktext = csbnode->getChildByName("loadingclicktext");
	m_loadingclicktext->setVisible(false);

	tipslbl = (cocos2d::ui::Text*)csbnode->getChildByName("tips");
	tipslbl->setVisible(false);

	cocos2d::ui::CheckBox* checkBox = (cocos2d::ui::CheckBox*)m_userpro->getChildByName("checkbox");
	checkBox->addEventListener(CC_CALLBACK_2(LoadingScene::checkBoxCallback, this));

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

	checkBox->setPositionY(m_wordlbl->getPositionY() - 2);
	textclick->setPositionY(m_wordlbl->getPositionY());

	for (int i = 0; i < 3; i++)
	{
		std::string pointstr = StringUtils::format("loadingpoint%d", i);
		point[i] = csbnode->getChildByName(pointstr);
		point[i]->setVisible(false);
	}

	if (DataSave::getInstance()->getFirstEnter())
	{
		//Layer *userProlayer = UserProtocolLayer::create();
		//this->addChild(userProlayer, 0, "UserProtocolLayer");
		//AnimationEffect::openAniEffect(userProlayer);
		m_loadingbg->setEnabled(true);
		m_loadingclicktext->setVisible(true);
		m_loadingclicktext->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
		DataSave::getInstance()->setUserProtocal(true);
	}
	else
	{
		m_userpro->setVisible(false);
		m_wordlbl->setVisible(false);
		loadData();
	}
	//IOS第一次安装会有联网权限提示
	//if (DataSave::getInstance()->getFirstEnter())
	//{
	//	m_loadingbg->setEnabled(true);
	//	m_loadingclicktext->setVisible(true);
	//	m_loadingclicktext->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
	//}
	//else
	//{
	//	loadData();
	//}

    return true;
}

void LoadingScene::loadData()
{
	m_loadingclicktext->stopAllActions();
	m_loadingclicktext->setVisible(false);

	m_loadingtext->setVisible(true);
	m_loadingbar->setVisible(true);
	m_loadingbar->runAction(RepeatForever::create(RotateTo::create(0.8f, 720)));
	showTips(0);
	showPointAnim(0);
	this->schedule(schedule_selector(LoadingScene::showPointAnim), 1.5f);

	this->schedule(schedule_selector(LoadingScene::showTips), 3.0f);

	if (GlobalInstance::punishment == 0)
	{
		if (!GlobalInstance::isResetData)
		{
			//先获取服务器数据
			this->scheduleOnce(schedule_selector(LoadingScene::delayGetServerData), 0.1f);
		}
		else
		{
			this->scheduleOnce(schedule_selector(LoadingScene::resetLoadData), 1.0f);
		}
	}
	else
	{
		this->scheduleOnce(schedule_selector(LoadingScene::punishmentAction), 1.0f);
	}
}

void LoadingScene::resetLoadData(float dt)
{
	GlobalInstance::isResetData = false;
	GlobalInstance::getInstance()->resetData();
	GlobalInstance::getInstance()->cleanUserDefaultXmlData();

	DataSave::getInstance()->setUserProtocal(true);

	HttpDataSwap::init(NULL)->postAllData();
	parseCfgFiles();

	enterNewScene();
}

void LoadingScene::punishmentAction(float dt)
{
	std::string hintstr;
	if (GlobalInstance::punishment == -1)
	{
		hintstr = ResourceLang::map_lang["punishment2"];
	}
	else if (GlobalInstance::punishment == -2)
	{
		hintstr = ResourceLang::map_lang["punishment3"];
	}
	HintBoxLayer* hint = HintBoxLayer::create(hintstr, 10);
	this->addChild(hint, 10000, this->getTag());
	AnimationEffect::openAniEffect(hint);

	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();

	std::string content = GlobalInstance::getInstance()->getUserDefaultXmlString(0);

	int err = pDoc->Parse(content.c_str());
	if (err != 0)
	{
		delete pDoc;
	}
	tinyxml2::XMLElement *rootEle = pDoc->RootElement();

	tinyxml2::XMLElement *element = rootEle->FirstChildElement();
	while (element != NULL)
	{
		std::string key = element->Name();
		if (key.find("newergd") == std::string::npos)
		{
			if (key.compare("jhpayrebate") != 0)
				UserDefault::getInstance()->deleteValueForKey(element->Name());
		}
		element = element->NextSiblingElement();
	}

	if (GlobalInstance::punishment == -2)
	{
		std::vector<Hero*>::iterator it;
		for (it = GlobalInstance::vec_myHeros.begin(); it != GlobalInstance::vec_myHeros.end();)
		{
			Hero* hero = *it;
			if (hero->getPotential() >= 4)
			{
				it = GlobalInstance::vec_myHeros.erase(it);
				break;
			}
			else
			{
				it++;
			}
		}
		GlobalInstance::getInstance()->saveMyHeros();

		GlobalInstance::getInstance()->saveResCreatorData();
		GlobalInstance::getInstance()->saveTotalFarmers(GlobalInstance::getInstance()->getTotalFarmers());

		std::string keephillstr = StringUtils::format("0-0-0-0-0-0-0-%d-0-0-0", Building::map_buildingDatas["7homehill"]->level.getValue());
		DataSave::getInstance()->setBuildingsLv(keephillstr);
	}

	GlobalInstance::punishment = 0;

	UserDefault::getInstance()->flush();

	GlobalInstance::getInstance()->resetData();
	DataSave::getInstance()->setUserProtocal(true);
	DataSave::getInstance()->setFirstEnter(false);

	parseCfgFiles();

	HttpDataSwap::init(NULL)->postAllData();

	this->scheduleOnce(schedule_selector(LoadingScene::showNextScene), 5.0f);
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

void LoadingScene::optimizeSaveData()
{
	if (DataSave::getInstance()->getMyHeroCount().length() <= 0)
	{
		int qus[QUMAX] = { 0 };
		int herocount = GlobalInstance::vec_myHeros.size();
		int deathherocount = 0;
		for (int i = 0; i < herocount; i++)
		{
			if (GlobalInstance::vec_myHeros[i]->getState() == HS_DEAD)
				deathherocount++;
			qus[GlobalInstance::vec_myHeros[i]->getPotential()]++;
		}

		std::string cstr = StringUtils::format("%d-%d", herocount - deathherocount, deathherocount);


		for (int i = 0; i < QUMAX; i++)
		{
			std::string pstr = StringUtils::format("-%d", qus[i]);
			cstr.append(pstr);
		}

		for (int i = 0; i < 6; i++)
		{
			Hero* hero = GlobalInstance::myCardHeros[i];
			if (hero != NULL)
			{
				std::string hstr = StringUtils::format(";%s-%d-%d-%d-%d", hero->getName().c_str(), hero->getLevel(), hero->getVocation(), hero->getPotential(), hero->getPos());
				cstr.append(hstr);
			}
		}
		DataSave::getInstance()->setMyHeroCount(cstr);
	}
	if (DataSave::getInstance()->getBuildingsLv().length() <= 0)
	{
		std::string str;
		std::map<std::string, Building*>::iterator it;
		for (it = Building::map_buildingDatas.begin(); it != Building::map_buildingDatas.end(); it++)
		{
			int blv = DataSave::getInstance()->getBuildingLv(it->first);
			Building::map_buildingDatas[it->first]->level.setValue(blv);
			std::string lvstr = StringUtils::format("%d-", blv);
			str.append(lvstr);
			DataSave::getInstance()->deleteDataByKey(it->first);
		}
		DataSave::getInstance()->setBuildingsLv(str.substr(0, str.length() - 1));
	}
	else
	{
		GlobalInstance::getInstance()->loadBuildingsLv();
	}

	std::string gstr = DataSave::getInstance()->getNewerGuides();
	unsigned int gsize = GlobalInstance::vec_newerguides.size();
	if (gstr.length() <= 0)
	{
		std::string str;
		for (unsigned int i = 0; i < gsize; i++)
		{
			GlobalInstance::vec_newerguides[i] = DataSave::getInstance()->getIsNewerGuide(i);

			std::string nstr = StringUtils::format("%d", GlobalInstance::vec_newerguides[i]);
			str.append(nstr);
		}
		DataSave::getInstance()->setNewerGuides(str);
		for (int i = 0; i < 94; i++)
		{
			std::string nstr = StringUtils::format("guide%d", i);
			DataSave::getInstance()->deleteDataByKey(nstr);
		}
	}
	else
	{
		for (unsigned int i = 0; i < gstr.length(); i++)
		{
			if (i <= gsize - 1)
				GlobalInstance::vec_newerguides[i] = atoi(gstr.substr(i, 1).c_str());
		}

		if (gstr.length() > 90)
		{
			if (GlobalInstance::vec_newerguides[30] == 0)//招募完毕后（2.0.0加入了招募需要银两的提示框）
			{
				for (int i = 88; i <= 90; i++)
				{
					GlobalInstance::vec_newerguides[i] = 0;
				}
			}
		}
	}
	//任务bug,做兼容

	std::string mtaskstr = DataSave::getInstance()->getMyOldMainTask();
	if (mtaskstr.length() > 0)
	{
		for (unsigned i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
		{
			if (GlobalInstance::vec_TaskMain[i].finishtype > 0)
			{
				for (unsigned int j = 0; j < GlobalInstance::vec_TaskMain[i].type.size(); j++)
				{
					if (GlobalInstance::vec_TaskMain[i].finishtype == GlobalInstance::vec_TaskMain[i].type[j])
					{
						GlobalInstance::vec_TaskMain[i].finishtype = j + 1;
						break;
					}
				}
			}
		}
		DataSave::getInstance()->deleteDataByKey("MainTask");
		GlobalInstance::getInstance()->saveMyTaskMainData();
	}

	std::string btaskstr = DataSave::getInstance()->getMyOldBranchTask();
	if (btaskstr.length() > 0)
	{
		for (unsigned i = 0; i < GlobalInstance::vec_TaskBranch.size(); i++)
		{
			if (GlobalInstance::vec_TaskBranch[i].finishtype > 0)
			{
				for (unsigned int j = 0; j < GlobalInstance::vec_TaskBranch[i].type.size(); j++)
				{
					if (GlobalInstance::vec_TaskBranch[i].finishtype == GlobalInstance::vec_TaskBranch[i].type[j])
					{
						GlobalInstance::vec_TaskBranch[i].finishtype = j + 1;
						break;
					}
				}
			}
		}
		DataSave::getInstance()->deleteDataByKey("BranchTask");
		GlobalInstance::getInstance()->saveMyTaskBranchData();
	}

	//保存的物品给角色穿戴，名字如果有乱码，无法保存数据到服务器

	bool issavedata = false;
	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		if (MyRes::vec_MyResources[i]->getType() >= T_ARMOR && MyRes::vec_MyResources[i]->getType() <= T_NG)
		{
			Equipable* equip = (Equipable*)MyRes::vec_MyResources[i];

			bool isfind = false;
			for (unsigned int m = 0; m < GlobalInstance::vec_myHeros.size(); m++)
			{
				if (equip->getWhos().compare(GlobalInstance::vec_myHeros[m]->getName()) == 0)
				{
					isfind = true;
					break;
				}
			}

			if (!isfind && equip->getWhos().length() > 0)
			{
				equip->setWhos("");
				equip->setWhere(MYSTORAGE);
				issavedata = true;
			}
		}
	}

	if (issavedata)
	{
		MyRes::saveData();
	}
}

void LoadingScene::parseCfgFiles()
{
	GlobalInstance::getInstance()->loadInitData();

	//解析建筑物xml
	Building::parseData();

	GlobalInstance::getInstance()->loadResCreatorData();

	GlobalInstance::getInstance()->loadAllResourcesData();

	GlobalInstance::getInstance()->parseSuitJson();
	GlobalInstance::getInstance()->loadEquipData();

	GlobalInstance::getInstance()->loadGFData();

	//英雄基础属性
	GlobalInstance::getInstance()->loadHerosAttrData();

	GlobalInstance::getInstance()->loadMyResData();

	//读取本地英雄
	GlobalInstance::getInstance()->loadMyHeros();

	GlobalInstance::getInstance()->loadRand3Heros();

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

	//加载npc好友数据
	GlobalInstance::getInstance()->loadNpcFriendData();

	//加载好友数据
	GlobalInstance::getInstance()->loadNpcFriendly();

	//加载每章节星奖奖励数据
	GlobalInstance::getInstance()->loadChapterStarAwds();

	GlobalInstance::getInstance()->parseBuildingRepairData();

	GlobalInstance::getInstance()->parseFormationData();

	GlobalInstance::getInstance()->parseMopupData();

	//本地英雄加上ID兼容上一个版本
	addHeroId();

	optimizeSaveData();
}

void LoadingScene::delayLoadLocalData(float dt)
{
	parseCfgFiles();
	//数据处理完，
	//加载技能特效
	curEffectPlistNum = 0;
	std::string str = StringUtils::format("effect/skill%dpacker.png", skillEffectArr[curEffectPlistNum]);
	Director::getInstance()->getTextureCache()->addImageAsync(str, CC_CALLBACK_1(LoadingScene::loadingSkillEffectOver, this));
	//this->scheduleOnce(schedule_selector(LoadingScene::showNextScene), 0.1f);
    
    if (DataSave::getInstance()->getLocalUUID().compare(GlobalInstance::getInstance()->UUID()) != 0)
    {
        GlobalInstance::isNotSameUUID = true;
        DataSave::getInstance()->setExitScene(0);
        HttpDataSwap::init(NULL)->report("1");
    }
}

//本地英雄加上ID兼容上一个版本
void LoadingScene::addHeroId()
{
	int count = 0;
	bool needsave = false;
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero * hero = GlobalInstance::vec_myHeros[i];
		if (hero->getId().length() <= 10)
		{
			needsave = true;
			count-=2;
			int systime = GlobalInstance::getInstance()->getSysSecTime() + count;
			std::string heroid = StringUtils::format("%d%02d", systime, GlobalInstance::getInstance()->createRandomNum(100));
			hero->setId(heroid);
		}
	}
	if (needsave)
		GlobalInstance::getInstance()->saveMyHeros();
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
			//Director::getInstance()->replaceScene(TransitionFade::create(2.2f, MapBlockScene::createScene("m0-0-0", 1)));
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
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
			else if (exitscene == 1 || exitscene == 2)
			{
				GlobalInstance::myOutMapCarry = DataSave::getInstance()->getHeroMapCarryCount();

				int count = 0;
				for (int i = 0; i < 6; i++)
				{
					if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
						count++;
				}
				if (count > 0 && MyRes::getMyResCount("r001", MYPACKAGE) > 0)
					Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainMapScene::createScene()));
				else
					Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
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
			if (DataSave::getInstance()->getFirstEnter())
				HttpDataSwap::init(this)->getAllData();
			else
				isLoadLocal = true;
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
		if (GlobalInstance::legalcopyurl.length() > 0 && errcode == 3)
		{
			ErrorHintLayer* layer = (ErrorHintLayer*)this->getChildByName("networkerrlayer");
			if (layer == NULL)
			{
				layer = ErrorHintLayer::create(6);
				this->addChild(layer, 0, "networkerrlayer");
				AnimationEffect::openAniEffect(layer);
				return;
			}
		}
		if (errcode == 2)
		{
			ErrorHintLayer* layer = (ErrorHintLayer*)this->getChildByName("networkerrlayer");
			if (layer == NULL)
			{
				layer = ErrorHintLayer::create(2);
				this->addChild(layer, 0, "networkerrlayer");
				AnimationEffect::openAniEffect(layer);
				return;
			}
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
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	cfgFileEncryp();
#endif
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

void LoadingScene::showTips(float dt)
{
	int r = GlobalInstance::getInstance()->createRandomNum(TIPSCOUNT);
	std::string keystr = StringUtils::format("tips_%d", r);
	tipslbl->setVisible(true);
	tipslbl->setString(ResourceLang::map_lang[keystr]);
}

void LoadingScene::cfgFileEncryp()
{
	std::vector<std::string> vec_files;
	
	CommonFuncs::dfsFolder("../Resources/jsonxml_original", vec_files);

	for (unsigned int i = 0; i < vec_files.size(); i++)
	{
		std::string filemame = vec_files[i].substr(vec_files[i].find("jsonxml_original"));

		std::string filecontent = FileUtils::getInstance()->getStringFromFile(filemame);
		CommonFuncs::encryptToFile(filecontent, filemame.substr(filemame.find("/") + 1));
	}
}

void LoadingScene::checkBoxCallback(cocos2d::Ref* pSender, cocos2d::ui::CheckBox::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED://选中
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		DataSave::getInstance()->setUserProtocal(true);
	}
	break;
	case cocos2d::ui::CheckBox::EventType::UNSELECTED://不选中
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		DataSave::getInstance()->setUserProtocal(false);
	}
	break;
	default:
		break;
	}
}
