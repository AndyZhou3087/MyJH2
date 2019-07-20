#include "StarFrist3AwdLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "GlobalInstance.h"

USING_NS_CC;

StarFrist3AwdLayer::StarFrist3AwdLayer()
{

}

StarFrist3AwdLayer::~StarFrist3AwdLayer()
{

}


StarFrist3AwdLayer* StarFrist3AwdLayer::create(std::string awdstr)
{
	StarFrist3AwdLayer *pRet = new(std::nothrow)StarFrist3AwdLayer();
	if (pRet && pRet->init(awdstr))
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
bool StarFrist3AwdLayer::init(std::string awdstr)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("first3starawdLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
	cocos2d::ui::Text* rescountlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");
	cocos2d::ui::Text* resname = (cocos2d::ui::Text*)resbox->getChildByName("name");

	std::vector<std::string> vec_str;
	CommonFuncs::split(awdstr, vec_str, "-");
	std::string resid = vec_str[0];
	int rescount = atoi(vec_str[1].c_str());
	resname->setString(GlobalInstance::map_AllResources[resid].name);

	std::string boxstr = "ui/resbox.png";
	int t = 0;
	int qu = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_RENS && t <= T_BOX)
	{
		qu = atoi(resid.substr(1).c_str()) - 1;
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t == T_EPIECE)
	{
		Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
		pieceicon->setAnchorPoint(Vec2(0, 1));
		pieceicon->setPosition(10, resbox->getContentSize().height - 10);
		resbox->addChild(pieceicon);
	}
	resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);

	Node* effectnode = CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

	std::string residstr = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);//StringUtils::format("ui/%s.png", resid.c_str());
	res->loadTexture(residstr, cocos2d::ui::Widget::TextureResType::PLIST);

	std::string countstr = StringUtils::format("%d", rescount);
	rescountlbl->setString(countstr);

	if (resid.compare("r006") == 0)
	{
		DynamicValueInt dvint;
		dvint.setValue(rescount);
		GlobalInstance::getInstance()->addMySoliverCount(dvint);
	}
	else if (resid.compare("r012") == 0)
	{
		DynamicValueInt dvint;
		dvint.setValue(rescount);
		GlobalInstance::getInstance()->addMyCoinCount(dvint);
	}
	else
		MyRes::Add(resid, rescount, MYSTORAGE, qu, GlobalInstance::getInstance()->generateStoneCount(qu));

	Node* light = csbnode->getChildByName("light");
	light->runAction(RepeatForever::create(RotateTo::create(10, 720)));

	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}