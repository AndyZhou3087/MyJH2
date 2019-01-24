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
#include "MarketRefreshLayer.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "MainScene.h"

USING_NS_CC;

MarketLayer::MarketLayer()
{
	lastCategoryindex = -1;
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
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("marketLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();
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
	closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
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

	Node* categoryBtnNode = csbnode->getChildByName("catanode");
	for (int i = 0; i < categoryBtnNode->getChildrenCount(); i++)
	{
		std::string btnstr = StringUtils::format("btn%d", i);
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)categoryBtnNode->getChildByName(btnstr);
		btn->setTag(i);
		btn->addTouchEventListener(CC_CALLBACK_2(MarketLayer::onCategory, this));
		vec_categoryBtn.push_back(btn);
	}

	if (!NewGuideLayer::checkifNewerGuide(66))
	{
		if (NewGuideLayer::checkifNewerGuide(67))
		{
			lastCategoryindex = 1;
		}
	}
	loadData();
	updateContent(lastCategoryindex);

	updateUI(0);
	this->schedule(schedule_selector(MarketLayer::updateUI), 1.0f);

	this->scheduleOnce(schedule_selector(MarketLayer::delayShowNewerGuide), 0.3f);

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

void MarketLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(66))
	{
		if (NewGuideLayer::checkifNewerGuide(67))
		{
			showNewerGuide(67);
		}
		else if (NewGuideLayer::checkifNewerGuide(68))
		{
			showNewerGuide(68);
		}
	}
}

void MarketLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 67)
	{
		for (unsigned int i = 0; i < map_cateRes[MKCATA_1].size(); i++)
		{
			if (map_cateRes[MKCATA_1][i].resid.compare("d001") == 0)
			{
				m_contentscroll->setTouchEnabled(false);
				Node* node = m_contentscroll->getChildByTag(i)->getChildByName("csbnode")->getChildByName("actionbtn");
				nodes.push_back(node);
				break;
			}
		}
	}
	else if (step == 68)
	{
		nodes.push_back(closebtn);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
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
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["maxlv"]);
			}
			break;
		case 1001://刷新
			//resetStockRes();
			{
				MarketRefreshLayer* layer = MarketRefreshLayer::create();
				this->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			
			break;
		case 1002://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}
	}
}

void MarketLayer::loadData()
{
	map_cateRes.clear();
	for (int v = 0; v <= m_buidingData->level.getValue(); v++)
	{
		int vsize = m_buidingData->vec_exdata.size();
		if (v < vsize)
		{
			for (unsigned int i = 0; i < m_buidingData->vec_exdata[v].size(); i++)
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(m_buidingData->vec_exdata[v][i], vec_tmp, "-");
				MK_RES mkres;
				mkres.resid = vec_tmp[0];
				mkres.maxcount = atoi(vec_tmp[1].c_str());
				mkres.stockcount = mkres.maxcount;

				if (mkres.resid.compare(0, 1, "p") == 0 || mkres.resid.compare(0, 1, "t") == 0 || mkres.resid.compare(0, 1, "v") == 0 || mkres.resid.compare(0, 1, "c") == 0 || mkres.resid.compare(0, 1, "i") == 0 || mkres.resid.compare("r013") == 0 || mkres.resid.compare("r014") == 0)
					map_cateRes[MKCATA_0].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "d") == 0)
					map_cateRes[MKCATA_1].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "s") == 0 || mkres.resid.compare(0, 1, "m") == 0)
					map_cateRes[MKCATA_2].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "b") == 0)
					map_cateRes[MKCATA_3].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "r") == 0 || mkres.resid.compare(0, 1, "q") == 0)
					map_cateRes[MKCATA_4].push_back(mkres);

				map_cateRes[MKCATA_ALL].push_back(mkres);
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

			std::map<int, std::vector<MK_RES>>::iterator it;

			for (it = map_cateRes.begin(); it != map_cateRes.end(); it++)
			{
				for (unsigned int m = 0; m < map_cateRes[it->first].size(); m++)
				{
					if (map_cateRes[it->first][m].resid.compare(resid) == 0)
					{
						map_cateRes[it->first][m].stockcount = stockcount;
					}
				}
			}
		}
	}
}

void MarketLayer::updateContent(int category)
{
	for (unsigned int i = 0; i < vec_categoryBtn.size(); i++)
	{
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)vec_categoryBtn[i]->getChildByName("text");

		if (category != i)
		{
			std::string textstr = StringUtils::format("marketcata%d_text_n", i);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			vec_categoryBtn[i]->setBright(true);
		}
		else
		{
			std::string textstr = StringUtils::format("marketcata%d_text_s", i);
			vec_categoryBtn[i]->setBright(false);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	m_contentscroll->removeAllChildrenWithCleanup(true);
	m_contentscroll->jumpToTop();

	int itemheight = 190;

	std::vector<MK_RES> vec_Res;

	if (map_cateRes.find(category) != map_cateRes.end())
	{
		vec_Res = map_cateRes[category];
	}

	int size = vec_Res.size();
	int innerheight = itemheight * size;
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(720, innerheight));

	for (int i = 0; i < size; i++)
	{
		Node* itemnode = MarketResNode::create(vec_Res[i].resid, vec_Res[i].stockcount);

		itemnode->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));

		itemnode->runAction(EaseSineIn::create(MoveBy::create(0.15f + i*0.07f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));
		//itemnode->setPosition(Vec2(m_contentscroll->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		m_contentscroll->addChild(itemnode, 0 , i);
	}
}

void MarketLayer::lvup()
{
	std::string str = StringUtils::format("%d%s", m_buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	loadData();
	updateContent(lastCategoryindex);
}

void MarketLayer::buyRes(int iterindex, int count)
{
	std::string resid = map_cateRes[lastCategoryindex][iterindex].resid;

	if (resid.compare("r013") == 0)
	{
		DynamicValueInt dvint;
		dvint.setValue(1000 * count);
		GlobalInstance::getInstance()->addMySoliverCount(dvint);
	}
	else if (resid.compare("r014") >= 0 && resid.compare("r018") <= 0)
	{
		int intid = atoi(resid.substr(1).c_str());
		std::string nresid = StringUtils::format("r%03d", intid - 13);
		MyRes::Add(nresid, 100 * count);
	}
	else
		MyRes::Add(resid, count);

	map_cateRes[lastCategoryindex][iterindex].stockcount -= count;

	if (lastCategoryindex != MKCATA_ALL)
	{
		for (unsigned int m = 0; m < map_cateRes[MKCATA_ALL].size(); m++)
		{
			if (map_cateRes[MKCATA_ALL][m].resid.compare(resid) == 0)
			{
				map_cateRes[MKCATA_ALL][m].stockcount -= count;
			}
		}
	}
	else
	{	
		std::map<int, std::vector<MK_RES>>::iterator it;

		for (it = map_cateRes.begin(); it != map_cateRes.end(); it++)
		{
			if (it->first != MKCATA_ALL)
			{
				for (unsigned int m = 0; m < map_cateRes[it->first].size(); m++)
				{
					if (map_cateRes[it->first][m].resid.compare(resid) == 0)
					{
						map_cateRes[it->first][m].stockcount -= count;
					}
				}
			}
		}
	}

	if (GlobalInstance::map_AllResources[resid].coinval > 0)
	{
		int saleval = GlobalInstance::map_AllResources[resid].coinval;
		DynamicValueInt dval;
		dval.setValue(count * saleval);
		GlobalInstance::getInstance()->costMyCoinCount(dval);
	}
	else
	{
		int saleval = GlobalInstance::map_AllResources[resid].silverval;
		DynamicValueInt dval;
		dval.setValue(count * saleval);
		GlobalInstance::getInstance()->costMySoliverCount(dval);
	}
	saveStockRes();
}

void MarketLayer::saveStockRes()
{
	std::string str;

	//std::map<int, std::vector<MK_RES>>::iterator it;

	//for (it = map_cateRes.begin(); it != map_cateRes.end(); it++)
	//{
		for (unsigned int m = 0; m < map_cateRes[MKCATA_ALL].size(); m++)
		{
			if (map_cateRes[MKCATA_ALL][m].stockcount < map_cateRes[MKCATA_ALL][m].maxcount)
			{
				std::string onestr = StringUtils::format("%s-%d;", map_cateRes[MKCATA_ALL][m].resid.c_str(), map_cateRes[MKCATA_ALL][m].stockcount);
				str.append(onestr);
			}
		}
	//}

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
	std::map<int, std::vector<MK_RES>>::iterator it;

	for (it = map_cateRes.begin(); it != map_cateRes.end(); it++)
	{
		for (unsigned int m = 0; m < map_cateRes[it->first].size(); m++)
		{
			if (map_cateRes[it->first][m].stockcount != map_cateRes[it->first][m].maxcount)
			{
				map_cateRes[it->first][m].stockcount = map_cateRes[it->first][m].maxcount;
			}
		}
	}

	std::vector<MK_RES> vec_Res;

	if (map_cateRes.find(lastCategoryindex) != map_cateRes.end())
	{
		vec_Res = map_cateRes[lastCategoryindex];
	}

	for (unsigned int i = 0; i < vec_Res.size(); i++)
	{
		MarketResNode* marketnode = (MarketResNode*)m_contentscroll->getChildByTag(i);
		marketnode->reset(vec_Res[i].maxcount);
	}
	DataSave::getInstance()->setMarketStock("");
	//updateContent();
}

void MarketLayer::onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
	}
}

void MarketLayer::onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (lastCategoryindex == node->getTag())
			return;

		lastCategoryindex = node->getTag();

		updateContent(node->getTag());
	}
}
