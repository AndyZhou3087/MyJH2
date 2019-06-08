#include "FightingResultLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "FightHeroNode.h"
#include "WinRewardLayer.h"
#include "MyRes.h"
#include "MainScene.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "MyTransitionScene.h"
#include "NewGuideLayer.h"
#include "MapBlockScene.h"

USING_NS_CC;

#define WINSCORE 5
FightingResultLayer::FightingResultLayer()
{

}

FightingResultLayer::~FightingResultLayer()
{

}


FightingResultLayer* FightingResultLayer::create(std::vector<FOURProperty> reward_res, int winexp, int onstate)
{
	FightingResultLayer *pRet = new(std::nothrow)FightingResultLayer();
	if (pRet && pRet->init(reward_res, winexp, onstate))
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

// on "init" you need to initialize your instance
bool FightingResultLayer::init(std::vector<FOURProperty> reward_res, int winexp, int onstate)
{
	if (!Layer::init())
	{
		return false;
	}
	m_onstate = onstate;
	m_rewards = reward_res;

	int langtype = GlobalInstance::getInstance()->getLang();
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("fightResultLayer.csb"));
	this->addChild(csbnode);

	Node* matchinfonode = csbnode->getChildByName("matchinfonode");

	Node* continuenode = csbnode->getChildByName("continuenode");
	continuenode->setVisible(false);

	Node* failhintnode = csbnode->getChildByName("failhintnode");
	failhintnode->setVisible(false);

	if (m_onstate == 0)
	{
		matchinfonode->setVisible(false);
	
		cocos2d::ui::Text* continuewintext = (cocos2d::ui::Text*)continuenode->getChildByName("continuewintext");
		cocos2d::ui::Text* continuenodeathwintext = (cocos2d::ui::Text*)continuenode->getChildByName("continuenodeathwintext");

		if (winexp > 0)
		{
			continuenode->setVisible(true);
			failhintnode->setVisible(false);
			std::string str = StringUtils::format(ResourceLang::map_lang["continuefighttext"].c_str(), GlobalInstance::curmapcontinuefightsucccount);
			continuewintext->setString(str);
			str = StringUtils::format(ResourceLang::map_lang["continuefighnodeathttext"].c_str(), GlobalInstance::starcontinuefightsucccount);
			for (unsigned int i = 0; i < GlobalInstance::vec_stardata.size(); i++)
			{
				if (GlobalInstance::vec_stardata[i].sid == SA_NODEATH)
				{
					std::string strkey = StringUtils::format("stagestar%02d", GlobalInstance::vec_stardata[i].sid);
					str.append(ResourceLang::map_lang[strkey]);
					break;
				}
			}
			continuenodeathwintext->setString(str);
		}
		else
		{
			continuenode->setVisible(false);
			int unlockc = GlobalInstance::getInstance()->getUnlockChapter();
			if (unlockc >= 3)
			{
				failhintnode->setVisible(false);
			}
			else
			{
				failhintnode->setVisible(true);

				std::vector<int> vec_hintindex;
				for (int i = 0; i < 11; i++)
				{
					vec_hintindex.push_back(i);
				}
				if (GlobalInstance::takeoutherocount >= 6 || unlockc == 2)
				{
					vec_hintindex.erase(vec_hintindex.begin());
				}

				std::random_shuffle(vec_hintindex.begin(), vec_hintindex.end());

				for (int i = 0; i < 3; i++)
				{
					std::string textname = StringUtils::format("hint%d", i + 1);
					cocos2d::ui::Text* failhintext = (cocos2d::ui::Text*)failhintnode->getChildByName(textname);
					textname = StringUtils::format("failhinttext%d", i + 1);
					std::string showstr = StringUtils::format("%d.", i + 1);
					showstr.append(ResourceLang::map_lang[textname]);
					failhintext->setString(showstr);
				}
			}
		}
	}
	else
	{
		continuenode->setVisible(false);
		failhintnode->setVisible(false);
		cocos2d::ui::ImageView* getscoretext = (cocos2d::ui::ImageView*)matchinfonode->getChildByName("getscoretext");
		getscoretext->loadTexture(ResourcePath::makeTextImgPath("getscore_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::Text* winscoretext = (cocos2d::ui::Text*)matchinfonode->getChildByName("winscoretext");
		winscoretext->setString(ResourceLang::map_lang["matchwinscoretext"]);
		cocos2d::ui::Text* winscore = (cocos2d::ui::Text*)matchinfonode->getChildByName("winscore");

		int wscore = 0;
		if (winexp > 0)
		{
			wscore = WINSCORE;
			GlobalInstance::myMatchInfo.matchscore += wscore;
			GlobalInstance::myMatchInfo.wincount++;
		}
		else
		{
			GlobalInstance::myMatchInfo.lostcount++;
		}
		std::string str = StringUtils::format("+%d", wscore);
		winscore->setString(str);
		cocos2d::ui::Text* myscoretext = (cocos2d::ui::Text*)matchinfonode->getChildByName("myscoretext");
		myscoretext->setString(ResourceLang::map_lang["matchtotalscoretext"]);
		cocos2d::ui::Text* myscore = (cocos2d::ui::Text*)matchinfonode->getChildByName("myscore");
		str = StringUtils::format("%d", GlobalInstance::myMatchInfo.matchscore);
		myscore->setString(str);

		HttpDataSwap::init(NULL)->sendMatchResult(wscore);
	}

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(FightingResultLayer::onBtnClick, this));
	actionbtn->setTag(1000);

	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");

	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("continuebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	SoundManager::getInstance()->stopBackMusic();

	FIGHTNODE_STATE fs = FS_SUCC;
	Node* effectnode;
	if (winexp <= 0)
	{
		fs = FS_FAIL;
		actionbtn->setTag(1001);
		actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		MyRes::clearMyPackages();

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_FAIL);

		//特效
		effectnode = CSLoader::createNode("effect/slsb/zhandoushibai.csb");
		effectnode->setPosition(Vec2(360, 1010));
		this->addChild(effectnode);
		auto action = CSLoader::createTimeline("effect/slsb/zhandoushibai.csb");
		effectnode->runAction(action);
		action->gotoFrameAndPlay(0, false);
	}
	else
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_WIN);

		effectnode = CSLoader::createNode("effect/slsb/zhandoushengli.csb");
		effectnode->setPosition(Vec2(360, 1010));
		this->addChild(effectnode);
		auto action = CSLoader::createTimeline("effect/slsb/zhandoushengli.csb");
		effectnode->runAction(action);
		action->gotoFrameAndPlay(0, false);
	}

	for (int i = 0; i < 6; i++)
	{
		FightHeroNode * fightHeroNode = FightHeroNode::create();

		Hero* fighthero = NULL;
		if (onstate == 0)
			fighthero = GlobalInstance::myCardHeros[i];
		else
			fighthero = GlobalInstance::myOnChallengeHeros[i];

		fightHeroNode->setPosition(145 + i%3*215, 680 -  i/3*280);
		fightHeroNode->setData(fighthero, F_HERO, fs);
		addChild(fightHeroNode, 0, i);
		if (fighthero != NULL)
		{
			if (onstate == 0)
				fightHeroNode->setFightState(winexp);
			else
				fightHeroNode->setMatchFightState();
		}
	}

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void FightingResultLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (node->getTag() == 1000)
		{
			if (m_onstate == 0)
			{
				node->setVisible(false);
				WinRewardLayer* layer = WinRewardLayer::create(m_rewards);
				this->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			else
			{
				AnimationEffect::closeAniEffect(this);
				SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_HOME);
			}
		}
		else
		{
			if (m_onstate == 0)
			{
				if (NewGuideLayer::checkifNewerGuide(86))
				{
					AnimationEffect::closeAniEffect(this);
					g_MapBlockScene->scheduleOnce(schedule_selector(MapBlockScene::delayShowNewerGuide), 0.3f);
					return;
				}
#if USE_TRANSCENE
				Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif				
			}
			else
			{
				AnimationEffect::closeAniEffect(this);
				SoundManager::getInstance()->playBackMusic(SoundManager::MUSIC_ID_HOME);
			}
		}
	}
}

void FightingResultLayer::onExit()
{
	Layer::onExit();
}