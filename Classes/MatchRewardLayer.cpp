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

	//≤‚ ‘
	//GlobalInstance::myMatchInfo.rewardstr = "r001-10-0,r006-100-0,a001-1-1;w001-1-0,r006-100-0;x001-1-0,r012-100-0;e001-1-1,f001-1-1,r012-100-0;a002-1-1,f002-1-1,r012-100-0";
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
			CommonFuncs::split(vec_match[m], vec_res, "-");
			vec_mstr.push_back(vec_res);
		}
		vec_matchlv.push_back(vec_mstr);
	}

	int lvscores[] = { 100, 300, 700, 1000, INT32_MAX };
	int lvsize = sizeof(lvscores) / sizeof(lvscores[0]);
	int itemheight = 220;
	int innerheight = (lvsize - 1) * itemheight;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));
	int startx[] = { 0, -103 ,-168 };
	int offsetx[] = { 0, 217, 168 };
	for (int i = 0; i < lvsize; i++)
	{
		if (i + 1 == lvsize)
		{
			break;
		}
		Node* node = CSLoader::createNode(ResourcePath::makePath("matchRewardNode.csb"));
		scrollView->addChild(node);
		node->setPosition(Vec2(240, innerheight - i * itemheight - itemheight / 2));
		std::string lvname = StringUtils::format("matchlvname_%d", i);
		std::string str = StringUtils::format(ResourceLang::map_lang["matchlv"].c_str(), ResourceLang::map_lang[lvname].c_str(), lvscores[i] + 1, lvscores[i + 1]);
		cocos2d::ui::Text* title = (cocos2d::ui::Text*)node->getChildByName("title");
		title->setString(str);
		if (vec_matchlv.size() == lvsize)
		{
			int rewardsize = vec_matchlv[i].size();
			for (int n = 0; n < 3; n++)
			{
				str = StringUtils::format("resbox_%d", n);
				cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)node->getChildByName(str);
				cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
				cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");
				cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");
				if (n < rewardsize)
				{
					resbox->setPositionX(startx[rewardsize - 1] + offsetx[rewardsize - 1] * n);

					int t = 0;
					std::string resid = vec_matchlv[i][n][0];
					int count = atoi(vec_matchlv[i][n][1].c_str());
					int qu = atoi(vec_matchlv[i][n][2].c_str());
					CommonFuncs::playResBoxEffect(resbox, qu);

					std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
					res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);
					namelbl->setString(GlobalInstance::map_AllResources[resid].name);
					std::string countstr = StringUtils::format("%d", count);
					countlbl->setString(countstr);
				}
				else
				{
					resbox->setVisible(false);
				}
			}
		}
	}


	//∆¡±Œœ¬≤„µ„ª˜
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