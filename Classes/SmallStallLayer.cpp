#include "SmallStallLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "MyRes.h"
#include "MarketResNode.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MainScene.h"
#include "WaitingProgress.h"

USING_NS_CC;

SmallStallLayer::SmallStallLayer()
{
	lastCategoryindex = -1;
}

SmallStallLayer::~SmallStallLayer()
{
	
}


SmallStallLayer* SmallStallLayer::create()
{
	SmallStallLayer*pRet = new(std::nothrow)SmallStallLayer();
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
bool SmallStallLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_buidingData = Building::map_buildingDatas["5market"];
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("marketLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* bg = (cocos2d::ui::ImageView*)csbnode->getChildByName("bg");
	bg->setContentSize(Size(bg->getContentSize().width, 1200));
	langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->ignoreContentAdaptWithSize(true);
	titleimg->loadTexture(ResourcePath::makeTextImgPath("smallstalltitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//升级按钮
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setVisible(false);

	//刷新按钮
	cocos2d::ui::Widget* refreshbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("refreshbtn");
	refreshbtn->setVisible(false);

	Node* lvUIlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	lvUIlbl->setVisible(false);

	//关闭按钮
	closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(SmallStallLayer::onBtnClick, this));

	Node* buildinglvbox = csbnode->getChildByName("buildinglvbox");
	buildinglvbox->setVisible(false);

	//滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setContentSize(Size(m_contentscroll->getContentSize().width, 960));
	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	Node* timebar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("timebar");
	timebar->setVisible(false);

	Node* timelbl = (cocos2d::ui::Text*)csbnode->getChildByName("timelbl");
	timelbl->setVisible(false);

	Node* timebargb = (cocos2d::ui::Text*)csbnode->getChildByName("timebargb");
	timebargb->setVisible(false);

	Node* timeiconbox = (cocos2d::ui::Text*)csbnode->getChildByName("timeiconbox");
	timeiconbox->setVisible(false);

	cocos2d::ui::Widget* repairbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("repairbtn");
	repairbtn->setVisible(false);

	cocos2d::ui::Widget* repairtimelbl = (cocos2d::ui::Text*)repairbtn->getChildByName("time");
	repairtimelbl->setVisible(false);

	mycoinlbl = (cocos2d::ui::Text*)csbnode->getChildByName("coincountlbl");

	mysilverlbl = (cocos2d::ui::Text*)csbnode->getChildByName("silvercountlbl");

	Node* categoryBtnNode = csbnode->getChildByName("catanode");
	for (int i = 0; i < categoryBtnNode->getChildrenCount(); i++)
	{
		std::string btnstr = StringUtils::format("btn%d", i);
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)categoryBtnNode->getChildByName(btnstr);
		btn->setTag(i);
		btn->addTouchEventListener(CC_CALLBACK_2(SmallStallLayer::onCategory, this));
		vec_categoryBtn.push_back(btn);
	}

	WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["doingtext"]);
	this->addChild(waitbox, 1,"waitbox");

	this->scheduleOnce(schedule_selector(SmallStallLayer::delayShowUI), 0.5f);

	Label *desclbl = Label::createWithTTF(ResourceLang::map_lang["smallstalldesc"], FONT_NAME, 22);
	desclbl->setColor(Color3B(125, 80, 47));
	desclbl->setPosition(Vec2(360, 60));
	csbnode->addChild(desclbl);

	updateLabel();

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


void SmallStallLayer::delayShowUI(float dt)
{
	loadData();
	updateContent(lastCategoryindex);

	this->removeChildByName("waitbox");
}


void SmallStallLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000:
			break;
		case 1001://刷新
			
			break;
		case 1002://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1003:
			break;
		default:
			break;
		}
	}
}

void SmallStallLayer::loadData()
{
	map_cateRes.clear();
	int showlv = m_buidingData->level.getValue();
	for (int v = 0; v <= showlv; v++)
	{
		int vsize = m_buidingData->vec_exdata.size();
		if (v < vsize)
		{
			for (unsigned int i = 0; i < m_buidingData->vec_exdata[v].size(); i++)
			{
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(m_buidingData->vec_exdata[v][i], vec_tmp, "-");
				SS_RES mkres;
				mkres.resid = vec_tmp[0];
				mkres.maxcount = atoi(vec_tmp[1].c_str());
				mkres.stockcount = mkres.maxcount;

				if (mkres.resid.compare(0, 1, "p") == 0 || mkres.resid.compare(0, 1, "t") == 0 || mkres.resid.compare(0, 1, "v") == 0 || mkres.resid.compare(0, 1, "c") == 0 || mkres.resid.compare(0, 1, "i") == 0 || mkres.resid.compare("r013") == 0 || mkres.resid.compare("r014") == 0 || mkres.resid.compare("z001") == 0)
					map_cateRes[SSCATA_0].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "d") == 0)
					map_cateRes[SSCATA_1].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "s") == 0 || mkres.resid.compare(0, 1, "m") == 0)
					map_cateRes[SSCATA_2].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "b") == 0)
					map_cateRes[SSCATA_3].push_back(mkres);
				else if (mkres.resid.compare(0, 1, "r") == 0 || mkres.resid.compare(0, 1, "q") == 0)
					map_cateRes[SSCATA_4].push_back(mkres);

				map_cateRes[SSCATA_ALL].push_back(mkres);
			}
		}
	}

	std::string stockstr = DataSave::getInstance()->getSmallStallStock();
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

			std::map<int, std::vector<SS_RES>>::iterator it;

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

void SmallStallLayer::updateContent(int category)
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

	std::vector<SS_RES> vec_Res;

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
		MarketResNode* itemnode = MarketResNode::create(vec_Res[i].resid, vec_Res[i].stockcount, 1);

		if (i < 8)
		{
			itemnode->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));

			itemnode->runAction(EaseSineIn::create(MoveBy::create(0.15f + i * 0.07f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));
		}
		else
			itemnode->setPosition(Vec2(m_contentscroll->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		m_contentscroll->addChild(itemnode, 0 , i);
	}
}

void SmallStallLayer::buyRes(int iterindex, int count)
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

	if (lastCategoryindex != SSCATA_ALL)
	{
		for (unsigned int m = 0; m < map_cateRes[SSCATA_ALL].size(); m++)
		{
			if (map_cateRes[SSCATA_ALL][m].resid.compare(resid) == 0)
			{
				map_cateRes[SSCATA_ALL][m].stockcount -= count;
			}
		}
	}
	else
	{	
		std::map<int, std::vector<SS_RES>>::iterator it;

		for (it = map_cateRes.begin(); it != map_cateRes.end(); it++)
		{
			if (it->first != SSCATA_ALL)
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
		dval.setValue((int)(count * saleval*0.6f));
		GlobalInstance::getInstance()->costMyCoinCount(dval);
		GlobalInstance::getInstance()->setPosCostCoin(COSTCOIN_MARKETBUYRES, dval.getValue());
	}
	else
	{
		int saleval = GlobalInstance::map_AllResources[resid].silverval;
		DynamicValueInt dval;
		dval.setValue((int)(count * saleval*0.6f));
		GlobalInstance::getInstance()->costMySoliverCount(dval);
	}


	saveStockRes();

	updateLabel();
}

void SmallStallLayer::saveStockRes()
{
	std::string str;

	//std::map<int, std::vector<MK_RES>>::iterator it;

	//for (it = map_cateRes.begin(); it != map_cateRes.end(); it++)
	//{
		for (unsigned int m = 0; m < map_cateRes[SSCATA_ALL].size(); m++)
		{
			if (map_cateRes[SSCATA_ALL][m].stockcount < map_cateRes[SSCATA_ALL][m].maxcount)
			{
				std::string onestr = StringUtils::format("%s-%d;", map_cateRes[SSCATA_ALL][m].resid.c_str(), map_cateRes[SSCATA_ALL][m].stockcount);
				str.append(onestr);
			}
		}
	//}

	if (str.length() > 0)
	{
		DataSave::getInstance()->setSmallStallStock(str.substr(0, str.length() - 1));
	}
}

void SmallStallLayer::updateLabel()
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	mysilverlbl->setString(str);

	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	mycoinlbl->setString(str);
}

void SmallStallLayer::onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
	}
}

void SmallStallLayer::onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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
