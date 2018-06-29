#include "MarketLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "ConsumeResActionLayer.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "MyRes.h"
#include "MarketResNode.h"
#include "Const.h"

USING_NS_CC;

MarketLayer::MarketLayer()
{

}

MarketLayer::~MarketLayer()
{
	
}


MarketLayer* MarketLayer::create(Building* buidingData)
{
	MarketLayer *pRet = new(std::nothrow)MarketLayer();
	if (pRet && pRet->init(buidingData))
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
bool MarketLayer::init(Building* buidingData)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_buidingData = buidingData;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("marketLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("markettitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	lvUIlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	//升级按钮
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setTag(1000);
	lvUpbtn->addTouchEventListener(CC_CALLBACK_2(MarketLayer::onBtnClick, this));

	//升级按钮文字
	cocos2d::ui::ImageView* lvUpbtntxt = (cocos2d::ui::ImageView*)lvUpbtn->getChildByName("text");
	lvUpbtntxt->loadTexture(ResourcePath::makeTextImgPath("marketlvup_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//刷新按钮
	cocos2d::ui::Widget* refreshbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("refreshbtn");
	refreshbtn->setTag(1001);
	refreshbtn->addTouchEventListener(CC_CALLBACK_2(MarketLayer::onBtnClick, this));

	//刷新按钮文字
	cocos2d::ui::ImageView* refreshbtntxt = (cocos2d::ui::ImageView*)refreshbtn->getChildByName("text");
	refreshbtntxt->loadTexture(ResourcePath::makeTextImgPath("refreshbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MarketLayer::onBtnClick, this));

	//滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	timebar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("timebar");
	timebar->setPercent(100);

	timelbl = (cocos2d::ui::Text*)csbnode->getChildByName("timelbl");

	mycoinlbl = (cocos2d::ui::Text*)csbnode->getChildByName("coincountlbl");

	mysilverlbl = (cocos2d::ui::Text*)csbnode->getChildByName("silvercountlbl");

	loadData();
	updateContent();

	updateUI(0);
	this->schedule(schedule_selector(MarketLayer::updateUI), 1.0f);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MarketLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://升级
			if (m_buidingData->level.getValue() < m_buidingData->maxlevel.getValue() - 1)
			{
				ConsumeResActionLayer* layer = ConsumeResActionLayer::create(m_buidingData, CA_BUILDINGLVUP);
				this->addChild(layer);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["maxlv"]);
			}
			break;
		case 1001://刷新
			break;
		case 1002://关闭
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void MarketLayer::loadData()
{
	vec_Res.clear();
	for (int v = 0; v <= m_buidingData->level.getValue(); v++)
	{
		if (v < m_buidingData->vec_exdata.size())
		{
			for (unsigned int i = 0; i < m_buidingData->vec_exdata[v].size(); i++)
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(m_buidingData->vec_exdata[v][i], vec_tmp, "-");
				MK_RES mkres;
				mkres.resid = vec_tmp[0];
				mkres.maxcount = atoi(vec_tmp[1].c_str());
				mkres.stockcount = mkres.maxcount;
				vec_Res.push_back(mkres);
			}
		}
	}
	std::string stockstr = DataSave::getInstance()->getMarketStock();
	if (stockstr.length() > 0)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(stockstr, vec_one, ";");
		for (unsigned int i = 0; i < vec_one.size(); i++)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(vec_one[i], vec_tmp, "-");
			std::string resid = vec_tmp[0];
			int stockcount = atoi(vec_tmp[1].c_str());
			for (unsigned int m = 0; m < vec_Res.size(); m++)
			{
				if (vec_Res[m].resid.compare(resid) == 0)
				{
					vec_Res[m].stockcount = stockcount;
				}
			}
		}
	}
}

void MarketLayer::updateContent()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int itemheight = 190;

	int size = vec_Res.size();
	int innerheight = itemheight * size;
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(720, innerheight));

	for (int i = 0; i < size; i++)
	{
		Node* itemnode = MarketResNode::create(vec_Res[i].resid, vec_Res[i].stockcount);

		itemnode->setPosition(Vec2(m_contentscroll->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		m_contentscroll->addChild(itemnode, 0 , i);
	}
}


void MarketLayer::lvup()
{
	std::string str = StringUtils::format("%d%s", m_buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	loadData();
	updateContent();
}

void MarketLayer::buyRes(int iterindex, int count)
{
	std::string resid = vec_Res[iterindex].resid;

	MyRes::Add(resid, count);

	vec_Res[iterindex].stockcount -= count;
	DynamicValueInt dval;
	dval.setValue(-count * GlobalInstance::map_AllResources[resid].saleval);
	GlobalInstance::getInstance()->addMySoliverCount(dval);
	saveStockRes();
}

void MarketLayer::saveStockRes()
{
	std::string str;
	for (unsigned int i = 0; i < vec_Res.size(); i++)
	{
		if (vec_Res[i].stockcount < vec_Res[i].maxcount)
		{
			std::string onestr = StringUtils::format("%s-%d;", vec_Res[i].resid.c_str(), vec_Res[i].stockcount);
			str.append(onestr);
		}
	}
	if (str.length() > 0)
	{
		DataSave::getInstance()->setMarketStock(str.substr(0, str.length() - 1));
	}
}

void MarketLayer::updateUI(float dt)
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	mysilverlbl->setString(str);

	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	mycoinlbl->setString(str);

	int lefttime = 0;
	int refreshtime = GlobalInstance::getInstance()->getRefreshMarketTime();
	int pasttime = GlobalInstance::servertime - refreshtime;
	if (pasttime >= MARKET_RESETTIME)
	{
		int t = GlobalInstance::servertime % MARKET_RESETTIME;

		refreshtime = GlobalInstance::servertime - t;
		GlobalInstance::getInstance()->saveRefreshMarketTime(refreshtime);

		lefttime = MARKET_RESETTIME - t;
		resetStockRes();
	}
	else
	{
		lefttime = MARKET_RESETTIME - pasttime;
	}
	std::string timestr = StringUtils::format("%02d:%02d:%02d", lefttime / 3600, lefttime % 3600 / 60, lefttime % 3600 % 60);
	timelbl->setString(timestr);
	timebar->setPercent(lefttime * 100 / MARKET_RESETTIME);
}

void MarketLayer::resetStockRes()
{
	for (unsigned int i = 0; i < vec_Res.size(); i++)
	{
		if (vec_Res[i].stockcount != vec_Res[i].maxcount)
		{
			vec_Res[i].stockcount = vec_Res[i].maxcount;
			MarketResNode* marketnode = (MarketResNode*)m_contentscroll->getChildByTag(i);
			marketnode->reset(vec_Res[i].maxcount);
		}
	}
	DataSave::getInstance()->setMarketStock("");
}

void MarketLayer::onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
	}
}