#include "MazeTransitionScene.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MapBlockScene.h"

MazeTransitionScene::MazeTransitionScene()
{
}


MazeTransitionScene::~MazeTransitionScene()
{

}

Scene* MazeTransitionScene::createScene(int mazechapter, int type)
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MazeTransitionScene::create(mazechapter, type);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void MazeTransitionScene::onExit()
{
	Layer::onExit();
}

MazeTransitionScene* MazeTransitionScene::create(int mazechapter, int type)
{
	MazeTransitionScene *pRet = new(std::nothrow)MazeTransitionScene();

	if (pRet && pRet->init(mazechapter, type))
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

bool MazeTransitionScene::init(int mazechapter, int type)
{
	if (!Layer::init())
	{
		return false;
	}

	Label *lbl = Label::createWithTTF(ResourceLang::map_lang["mazetext_0"], FONT_NAME, 25);
	lbl->setLineSpacing(8);
	lbl->setColor(Color3B(255, 255, 255));
	lbl->setMaxLineWidth(540);
	lbl->setHorizontalAlignment(TextHAlignment::CENTER);
	lbl->setPosition(Vec2(360, 640));
	this->addChild(lbl);
    
	this->runAction(Sequence::create(DelayTime::create(2.0f), CallFunc::create(CC_CALLBACK_0(MazeTransitionScene::loadfinish, this, mazechapter, type)), NULL));

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void MazeTransitionScene::loadfinish(int mazechapter, int type)
{
	std::string mapid;
	if (type == TO_ENTER)
	{
		mapid = StringUtils::format("mg%d-%d", mazechapter, GlobalInstance::vec_mazeroute[GlobalInstance::mazerouteindex]);
		
		resetMazeMap(mapid);
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MapBlockScene::createScene(mapid, mazesfightbg[mazechapter-1])));
	}
	else
	{
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MapBlockScene::createScene(GlobalInstance::eventfrommapid, GlobalInstance::map_mapsdata[GlobalInstance::eventfrommapid].map_sublist[mapid].bgtype)));
		
	}

}

void MazeTransitionScene::resetMazeMap(std::string mapid)
{
	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();

	std::string content = GlobalInstance::getInstance()->getUserDefaultXmlString(0);

	int err = pDoc->Parse(content.c_str());
	if (err != 0)
	{
		delete pDoc;
		return;
	}
	tinyxml2::XMLElement *rootEle = pDoc->RootElement();
	tinyxml2::XMLElement *element = rootEle->FirstChildElement();
	while (element != NULL)
	{
		std::string key = element->Name();
		if (key.find("mg") != std::string::npos)
		{
			UserDefault::getInstance()->deleteValueForKey(key.c_str());
		}
		element = element->NextSiblingElement();
	}
	UserDefault::getInstance()->flush();
}