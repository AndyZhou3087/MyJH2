#include "PlotStoryScene.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "Resource.h"
#include "MainScene.h"

std::string storytype[] = { "story", "end" };
PlotStoryScene::PlotStoryScene()
{
	m_wordlbl = NULL;
	isCanClick = false;
	clickcount = 0;
}


PlotStoryScene::~PlotStoryScene()
{
}

Scene* PlotStoryScene::createScene(int type)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = PlotStoryScene::create(type);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool PlotStoryScene::init(int type)
{
	m_type = type;

		////layer 点击事件，屏蔽下层事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		if (!isCanClick)
		{
			this->unschedule(schedule_selector(PlotStoryScene::showClickText));
			showClickText(0);
			m_action->gotoFrameAndPlay(m_action->getEndFrame(), false);
			return true;
		}
		if (clickcount == 1 || clickcount == 2)
		{
			isCanClick = false;
			this->scheduleOnce(schedule_selector(PlotStoryScene::delayShowNextStory), 0.1f);
		}
		else if (clickcount == 3)
		{
			isCanClick = false;
			this->scheduleOnce(schedule_selector(PlotStoryScene::showNextScene), 0.1f);
		}
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
	return true;
}

void PlotStoryScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	show();
}

void PlotStoryScene::show()
{
	std::string storystr = "story1.csb";
	m_csbnode = CSLoader::createNode(storystr);
	this->addChild(m_csbnode);

	m_action = CSLoader::createTimeline(storystr);
	m_csbnode->runAction(m_action);
	m_action->gotoFrameAndPlay(0, false);

	float delaytime = m_action->getEndFrame()*1.0f / (m_action->getTimeSpeed() * 60);
	this->scheduleOnce(schedule_selector(PlotStoryScene::showClickText), delaytime + 1.0f);
}

PlotStoryScene* PlotStoryScene::create(int type)
{
	PlotStoryScene *pRet = new PlotStoryScene();
	if (pRet && pRet->init(type))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = NULL;
	}
	return pRet;
}

void PlotStoryScene::delayShowNextStory(float dt)
{
	m_csbnode->removeFromParentAndCleanup(true);

	std::string storystr = StringUtils::format("story%d.csb", clickcount+1);

	m_csbnode = CSLoader::createNode(storystr);

	this->addChild(m_csbnode);

	m_action = CSLoader::createTimeline(storystr);
	m_csbnode->runAction(m_action);
	m_action->gotoFrameAndPlay(0, false);
	float delaytime = m_action->getEndFrame()*1.0f / (m_action->getTimeSpeed() * 60);
	this->scheduleOnce(schedule_selector(PlotStoryScene::showClickText), delaytime + 1.0f);
}

void PlotStoryScene::showClickText(float dt)
{
	m_wordlbl = Label::createWithTTF(ResourceLang::map_lang["clickcontinue"], FONT_NAME, 25);
	m_wordlbl->setPosition(Vec2(360, 30));
	m_wordlbl->setVisible(false);
	m_wordlbl->runAction(RepeatForever::create(Blink::create(2, 2)));
	m_csbnode->addChild(m_wordlbl);
	clickcount++;
	isCanClick = true;
}
void PlotStoryScene::showNextScene(float dt)
{
	Director::getInstance()->replaceScene(TransitionFade::create(1.5f, MainScene::createScene()));
	
}