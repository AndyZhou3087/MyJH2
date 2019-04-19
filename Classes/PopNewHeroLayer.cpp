#include "PopNewHeroLayer.h"
#include "CommonFuncs.h"
#include "AnimationEffect.h"
#include "Resource.h"
#include "Const.h"
#include "GlobalInstance.h"
#include "MainMapScene.h"
#include "RandHeroLayer.h"

PopNewHeroLayer::PopNewHeroLayer()
{
	iscanclick = false;
}


PopNewHeroLayer::~PopNewHeroLayer()
{
	if (g_mainScene != NULL)
	{
		RandHeroLayer* randlayer = (RandHeroLayer*)g_mainScene->getChildByName("RandHeroLayer");
		randlayer->delayShowNewerGuide(0);
	}
}

PopNewHeroLayer* PopNewHeroLayer::create(Hero* hero)
{
	PopNewHeroLayer *pRet = new(std::nothrow)PopNewHeroLayer();
	if (pRet && pRet->init(hero))
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

bool PopNewHeroLayer::init(Hero* hero)
{
	if (!Layer::init())
		return false;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 220));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("popNewHeroLayer.csb"));
	this->addChild(csbnode, 0, "csbnode");
	
	int langtype = GlobalInstance::getInstance()->getLang();

	textnode = csbnode->getChildByName("textnode");
	aminnode = csbnode->getChildByName("animnode");
	heronode = csbnode->getChildByName("heronode");
	heronode->setVisible(false);
	textnode->setVisible(false);

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)textnode->getChildByName("title");
	title->loadTexture(ResourcePath::makeTextImgPath("newherotitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	vocimg = (cocos2d::ui::ImageView*)textnode->getChildByName("herovoc");
	std::string vocimgstr = StringUtils::format("newherovoc%d_text", hero->getVocation());
	vocimg->loadTexture(ResourcePath::makeTextImgPath(vocimgstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//品质
	quimg = (cocos2d::ui::ImageView*)textnode->getChildByName("heroqu");

	std::string quimgstr = StringUtils::format("heroattrqu_%d", hero->getPotential());
	quimg->loadTexture(ResourcePath::makeTextImgPath(quimgstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//头像
	headimg = (cocos2d::ui::ImageView*)heronode->getChildByName("heroimg");
	headimg->ignoreContentAdaptWithSize(true);
	headimg->setScale(0.5f);

	std::string str = StringUtils::format("hfull_%d_%d.png", hero->getVocation(), hero->getSex());
	headimg->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	light = heronode->getChildByName("light");
	light->runAction(RepeatForever::create(RotateTo::create(10, 720)));

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		if (iscanclick)
			AnimationEffect::closeAniEffect((Layer*)this);
	};

	if (hero->getPotential() <= 3)
	{
		aminnode->setVisible(false);
		heronode->setScale(0);
		heronode->setVisible(true);
		heronode->runAction(Sequence::create(DelayTime::create(0.2f), Show::create(), ScaleTo::create(0.2f, 1), CallFunc::create(CC_CALLBACK_0(PopNewHeroLayer::showText, this, 0)), NULL));
	}
	else if (hero->getPotential() <= 4)
	{
		aminnode->setVisible(true);
		heronode->setVisible(true);
		heronode->setScale(0);
		auto action = CSLoader::createTimeline("popNewHeroLayer.csb");
		csbnode->runAction(action);
		action->gotoFrameAndPlay(0, false);

		this->scheduleOnce(schedule_selector(PopNewHeroLayer::showHeroAnim), 1.5f);

		this->scheduleOnce(schedule_selector(PopNewHeroLayer::showText), 2.0f);

	}

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void PopNewHeroLayer::showText(float dt)
{
	textnode->setVisible(true);
	iscanclick = true;
}

void PopNewHeroLayer::showHeroAnim(float dt)
{
	heronode->runAction(Sequence::create(ScaleTo::create(0.2f, 1), NULL));
}