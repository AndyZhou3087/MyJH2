#include "MatchContentLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"
#include "MatchHeroNode.h"

USING_NS_CC;

MatchContentLayer::MatchContentLayer()
{

}

MatchContentLayer::~MatchContentLayer()
{

}


MatchContentLayer* MatchContentLayer::create(MyRankData herodata)
{
	MatchContentLayer *pRet = new(std::nothrow)MatchContentLayer();
	if (pRet && pRet->init(herodata))
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
bool MatchContentLayer::init(MyRankData herodata)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 160));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchContentLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	name->setString(herodata.nickname);

	cocos2d::ui::Text* pointtext = (cocos2d::ui::Text*)csbnode->getChildByName("pointtext");
	pointtext->setString(ResourceLang::map_lang["matchexptext_1"]);

	cocos2d::ui::Text* point = (cocos2d::ui::Text*)csbnode->getChildByName("point");
	std::string str = StringUtils::format("%d", herodata.matchscore);
	point->setString(str);

	cocos2d::ui::Text* lvtext = (cocos2d::ui::Text*)csbnode->getChildByName("lvtext");
	lvtext->setString(ResourceLang::map_lang["matchlvtext_1"]);

	//¶ÎÎ»
	cocos2d::ui::Text* lv = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	str = StringUtils::format("matchlvname_%d", GlobalInstance::getInstance()->getMatchLvByScroe(herodata.matchscore));
	lv->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MatchContentLayer::onBtnClick, this));

	std::vector<std::string> vec_heronode;
	std::map<std::string, std::string>::iterator it;
	for (it = herodata.map_otherheros.begin(); it != herodata.map_otherheros.end(); it++)
	{
		if (it->second.length() > 0)
		{
			vec_heronode.push_back(it->second);
		}
	}

	for (unsigned int i = 0; i < vec_heronode.size(); i++)
	{
		MatchHeroNode* node = MatchHeroNode::create(vec_heronode[i]);
		this->addChild(node);
		node->setPosition(Vec2(153 + (i % 3) * 215, 686 - (i / 3) * 251));
	}


	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	//listener->onTouchEnded = [=](Touch *touch, Event *event)
	//{
	//	AnimationEffect::closeAniEffect((Layer*)this);
	//};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void MatchContentLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}