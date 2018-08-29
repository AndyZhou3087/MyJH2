#include "SkillStartLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"

USING_NS_CC;

SkillStartLayer::SkillStartLayer()
{
	isLeft = false;
}

SkillStartLayer::~SkillStartLayer()
{

}


SkillStartLayer* SkillStartLayer::create(int herovacation, int skilltype)
{
	SkillStartLayer *pRet = new(std::nothrow)SkillStartLayer();
	if (pRet && pRet->init(herovacation, skilltype))
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
bool SkillStartLayer::init(int herovacation, int skilltype)
{
	if (!Layer::init())
	{
		return false;
	}

	mstype = skilltype;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	std::string startskillstr;
	if (herovacation % 4 == 2 || herovacation % 4 == 3 || herovacation == 8)
	{
		startskillstr = ResourcePath::makePath("skillstartl.csb");
		isLeft = true;
	}
	else
	{
		startskillstr = ResourcePath::makePath("skillstartr.csb");
	}

	auto effectnode = CSLoader::createNode(startskillstr);
	this->addChild(effectnode);
	auto action = CSLoader::createTimeline(startskillstr);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
	this->scheduleOnce(schedule_selector(SkillStartLayer::remove), action->getDuration()/30.f);
	this->scheduleOnce(schedule_selector(SkillStartLayer::showTextAnim), 0.3f);
	cocos2d::ui::ImageView* hero = (cocos2d::ui::ImageView*)effectnode->getChildByName("Node")->getChildByName("hero");
	std::string herostr = StringUtils::format("hfull_%d_0.png", herovacation);
	hero->ignoreContentAdaptWithSize(true);
	hero->loadTexture(ResourcePath::makeImagePath(herostr));

	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}


void SkillStartLayer::showTextAnim(float dt)
{
	int langtype = GlobalInstance::getInstance()->getLang();
	std::string filename = "skillstarttextanim.csb";
	auto effectnode = CSLoader::createNode(ResourcePath::makePath(filename));
	this->addChild(effectnode);

	if (isLeft)
		effectnode->setPosition(Vec2(580, 430));
	else
		effectnode->setPosition(Vec2(170, 430));
	std::string str; 
	for (int i = 0; i < 4; i++)
	{
		str = StringUtils::format("t_%d", i);
		cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)effectnode->getChildByName(str);
		str = StringUtils::format("startskill%dtext_%d", mstype, i);
		text->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}

	auto action = CSLoader::createTimeline(filename);
	effectnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
}

void SkillStartLayer::remove(float dt)
{
	this->removeFromParentAndCleanup(true);
}