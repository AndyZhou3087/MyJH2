#include "MatchRewardLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"

USING_NS_CC;

MatchRewardLayer::MatchRewardLayer()
{

}

MatchRewardLayer::~MatchRewardLayer()
{

}


MatchRewardLayer* MatchRewardLayer::create()
{
	MatchRewardLayer *pRet = new(std::nothrow)MatchRewardLayer();
	if (pRet && pRet->init())
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
bool MatchRewardLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 160));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchRuleLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["matchruletitle"]);

	cocos2d::ui::Text* title2 = (cocos2d::ui::Text*)csbnode->getChildByName("title2");
	title2->setString(ResourceLang::map_lang["matchrulecontent"]);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");

	std::vector<std::vector<std::vector<std::string>>> vec_matchlv;
	std::vector<std::string> vec_retstr;
	CommonFuncs::split(GlobalInstance::myMatchInfo.rewardstr, vec_retstr, ";");
	for (unsigned int i = 0; i < vec_retstr.size(); i++)
	{
		std::vector<std::vector<std::string>> vec_mstr;
		std::vector<std::string> vec_match;
		CommonFuncs::split(vec_retstr[i], vec_match, ",");
		for (unsigned m = 0; m < vec_match.size(); m++)
		{
			std::vector<std::string> vec_res;
			CommonFuncs::split(vec_match[i], vec_res, "-");
			vec_mstr.push_back(vec_res);
		}
		vec_matchlv.push_back(vec_mstr);
	}

	int lv = 0;
	int lvscores[] = { -1, 100, 300, 700, 1000, INT32_MAX };
	int lvsize = sizeof(lvscores) / sizeof(lvscores[0]);
	int itemheight = 220;
	int innerheight = (lvsize - 1) * itemheight;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));
	//int posX[3][] = { {-168,0,165},{ -94,119 },{ 0 } };
	for (int i = 1; i < lvsize; i++)
	{
		Node* node = CSLoader::createNode(ResourcePath::makePath("matchRewardNode.csb"));
		scrollView->addChild(node);
		node->setPosition(Vec2(240, innerheight - (i - 1) * itemheight - itemheight / 2));
		std::string str = StringUtils::format(ResourceLang::map_lang["matchlv"].c_str(), i, lvscores[i - 1] + 1, lvscores[i]);
		cocos2d::ui::Text* title = (cocos2d::ui::Text*)node->getChildByName("title");
		title->setString(str);
		if (vec_matchlv.size() == lvsize - 1)
		{
			for (unsigned n = 0; n < vec_matchlv[i - 1].size(); n++)
			{
				str = StringUtils::format("resbox_%d", n);
				cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)node->getChildByName(str);
			}
		}
	}


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