#include "WaitingProgress.h"
#include "Resource.h"
#include "Const.h"

bool WaitingProgress::init(std::string text)
{
    if (Layer::init()) 
	{
		LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 100));
		this->addChild(color,0,"colorLayer");

		boxbg = Sprite::createWithSpriteFrameName("ui/waitingbg.png");
		boxbg->setPosition(Vec2(360, 640));
		addChild(boxbg);

		Sprite* progress = Sprite::createWithSpriteFrameName("ui/waitingprogress.png");
		progress->setPosition(Vec2(progress->getContentSize().width/2 + 5, boxbg->getContentSize().height/2));
		boxbg->addChild(progress);
		progress->runAction(RepeatForever::create(RotateTo::create(2, 720)));

		textstr = text;
		//this->scheduleOnce(schedule_selector(WaitingProgress::delayShowText), 0.01f);
		delayShowText(0);
    }

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void WaitingProgress::delayShowText(float dt)
{
	Label *textlbl = Label::createWithTTF(textstr, FONT_NAME, 23);

	textlbl->setPosition(Vec2(boxbg->getContentSize().width / 2, boxbg->getContentSize().height / 2));
	textlbl->setColor(Color3B(255, 255, 255));
	boxbg->addChild(textlbl);
}

WaitingProgress* WaitingProgress::create(std::string content)
{
	WaitingProgress *pRet = new WaitingProgress();
	if (pRet && pRet->init(content))
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