#include "SimpleResPopLayer.h"
#include "Resource.h"
#include "Const.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "AnimationEffect.h"
#include "Building.h"
#include "BuyResLayer.h"
#include "MovingLabel.h"
#include "MarketLayer.h"

USING_NS_CC;

SimpleResPopLayer::SimpleResPopLayer()
{

}

SimpleResPopLayer::~SimpleResPopLayer()
{

}


SimpleResPopLayer* SimpleResPopLayer::create(std::string resid, int forwhere, int needcount)//0--普通提示；1--市场购买；2--强化用闯王山洞；3--礼包购买
{
	SimpleResPopLayer *pRet = new(std::nothrow)SimpleResPopLayer();
	if (pRet && pRet->init(resid, forwhere, needcount))
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
bool SimpleResPopLayer::init(std::string resid, int forwhere, int needcount)
{
	if (!Layer::init())
	{
		return false;
	}
	m_resid = resid;
	m_needcount = needcount;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 120));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("simpleResPopLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* smallbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	std::string resboxstr;
	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	int qu = 0;
	std::string qustr = "ui/resbox.png";

	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_RENS && t <= T_BOX)
	{
		qu = atoi(resid.substr(1).c_str()) - 1;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_HEROCARD && t <= T_ARMCARD)
	{
		qu = 3;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	CommonFuncs::playResBoxEffect(resbox, qu);

	resbox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* actionclick = (cocos2d::ui::ImageView*)csbnode->getChildByName("actionclick");
	actionclick->addTouchEventListener(CC_CALLBACK_2(SimpleResPopLayer::onClick, this));
	actionclick->setVisible(false);
	cocos2d::ui::Text* clicktext = (cocos2d::ui::Text*)actionclick->getChildByName("text");
	clicktext->setString(ResourceLang::map_lang["getwheretext"]);
	clicktext->setSwallowTouches(false);

	std::string str = GlobalInstance::map_AllResources[resid].desc;

	if (forwhere == 1)
	{
		str.append(ResourceLang::map_lang["reswheregettext"]);
		actionclick->setVisible(true);
	}

	else if (forwhere == 2)
		str.append(ResourceLang::map_lang["reswheregettext1"]);

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desclbl");
	desclbl->setString(str);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[resid].name);


	//屏蔽下层点击
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

void SimpleResPopLayer::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		bool isfind = false;
		int resinlv = 0;
		Building* marketBuilding = Building::map_buildingDatas["5market"];
		;
		for (int v = 0; v < marketBuilding->maxlevel.getValue(); v++)
		{
			int vsize = marketBuilding->vec_exdata.size();
	
			for (unsigned int i = 0; i < marketBuilding->vec_exdata[v].size(); i++)
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(marketBuilding->vec_exdata[v][i], vec_tmp, "-");

				if (m_resid.compare(vec_tmp[0]) == 0)
				{
					resinlv = v;
					isfind = true;
					break;
				}
			}
			if (isfind)
				break;
		}

		if (isfind)
		{
			if (marketBuilding->level.getValue() >= resinlv)//市场等级够，直接弹出购买
			{

				std::vector< MSGAWDSDATA> vec_res;
				MSGAWDSDATA rdata;
				rdata.rid = m_resid;
				rdata.count = m_needcount;
				rdata.qu = 0;
				vec_res.push_back(rdata);
				BuyResLayer* layer = BuyResLayer::create(vec_res);
				g_mainScene->addChild(layer, 1000);
				this->removeFromParentAndCleanup(true);

			}
			else//弹升级界面
			{
				std::string str = StringUtils::format(ResourceLang::map_lang["marketlvlow"].c_str(), GlobalInstance::map_AllResources[m_resid].name.c_str(), resinlv + 1);
				MovingLabel::show(str);
				this->removeFromParentAndCleanup(true);
				if (g_mainScene != NULL)
				{
					MarketLayer* layer = MarketLayer::create(Building::map_buildingDatas["5market"]);
					g_mainScene->addChild(layer, 1000, "5market");
					AnimationEffect::openAniEffect(layer);
				}
			}
		}
	}
}