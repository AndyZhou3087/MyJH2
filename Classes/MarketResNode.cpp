#include "MarketResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "MarketLayer.h"
#include "MainScene.h"
#include "Const.h"
#include "SoundManager.h"
#include "SimpleResPopLayer.h"
#include "AnimationEffect.h"
#include "BuyCoinLayer.h"
#include "SmallStallLayer.h"

MarketResNode::MarketResNode()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	isEnable = true;
	buycount = 1;
}


MarketResNode::~MarketResNode()
{

}

MarketResNode* MarketResNode::create(std::string resid, int rescount, int type)
{
	MarketResNode *pRet = new(std::nothrow)MarketResNode();
	if (pRet && pRet->init(resid, rescount, type))
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

bool MarketResNode::init(std::string resid, int rescount, int type)
{
	m_resid = resid;
	m_type = type;
	totalrescount = rescount; 
	csbnode = CSLoader::createNode(ResourcePath::makePath("marketResNode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	cocos2d::ui::Widget* clickimg = (cocos2d::ui::Widget*)csbnode->getChildByName("clickimg");
	clickimg->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onBtnClick, this));
	clickimg->setTag(2000);
	clickimg->setSwallowTouches(false);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");

	std::string boxstr = "ui/resbox.png";
	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_RENS && t <= T_BOX)
	{
		int qu = atoi(resid.substr(1).c_str()) - 1;
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		CommonFuncs::playResBoxEffect(resbox, t, qu, 0);
	}
	resbox->loadTexture(ResourcePath::makePath(boxstr), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[resid].name);

	priceicon = (cocos2d::ui::ImageView*)csbnode->getChildByName("priceicon");
	priceicon->ignoreContentAdaptWithSize(true);

	if (GlobalInstance::map_AllResources[resid].coinval > 0)
		priceicon->loadTexture(ResourcePath::makePath("ui/main_coin.png"), cocos2d::ui::Widget::TextureResType::PLIST);

	if (resid.compare("r014") >= 0 && resid.compare("r018") <= 0)
	{
		int intid = atoi(resid.substr(1).c_str());
		resid = StringUtils::format("r%03d", intid - 13);
	}

	cocos2d::ui::ImageView* resimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", resid.c_str());
	resimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	//描述
	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(GlobalInstance::map_AllResources[resid].desc);

	totalpricelbl = (cocos2d::ui::Text*)csbnode->getChildByName("totalprice");

	totalpricelbl_1 = (cocos2d::ui::Text*)csbnode->getChildByName("totalprice_1");
	if (type == 0)
	{
		discount = 1.0f;
		totalpricelbl_1->setVisible(false);
	}
	else
	{
		discount = 0.6f;
	}


	rescountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("rescount");

	openlvtext = (cocos2d::ui::Text*)csbnode->getChildByName("openlvtext");
	openlvtext->setVisible(false);
	std::string openlvstr = StringUtils::format(ResourceLang::map_lang["marketopenlvtext"].c_str(), getResInMarketLv() + 1);
	openlvstr = CommonFuncs::replace_all(openlvstr, "\\n", "\n");
	openlvtext->setString(openlvstr);

	updateData();
	//按钮
	actbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actbtn->setTag(1000);
	actbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onBtnClick, this));
	actbtn->setSwallowTouches(false);

	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	cocos2d::ui::ImageView* actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath("buybtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//加号按钮
	addbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("addbtn");
	addbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onAddBtnClick, this));
	addbtn->setTag(1001);
	//减号按钮
	subbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("subbtn");
	subbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onSubBtnClick, this));
	subbtn->setTag(1002);

	timelbl = (cocos2d::ui::Text*)csbnode->getChildByName("timelbl");

	tagnode = csbnode->getChildByName("tag");
	tagnode->setVisible(false);

	inmarktLv = getResInMarketLv();

	this->schedule(schedule_selector(MarketResNode::updateTime), 1.0f);

	return true;
}

void MarketResNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	if (tag != 2000)
		CommonFuncs::BtnAction(pSender, type);

	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		clickflag = true;
		beginTouchPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));
	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{
		Vec2 movedPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));

		if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
			clickflag = false;
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		switch (tag)
		{
		case 1000://购买按钮
		{
			std::string showstr;

			int saleval = 0;
			DynamicValueInt myrich;
			bool iscoinsale = false;
			if (GlobalInstance::map_AllResources[m_resid].coinval > 0)
			{
				saleval = GlobalInstance::map_AllResources[m_resid].coinval;
				myrich.setValue(GlobalInstance::getInstance()->getMyCoinCount().getValue());
				iscoinsale = true;
			}
			else
			{
				saleval = GlobalInstance::map_AllResources[m_resid].silverval;
				myrich.setValue(GlobalInstance::getInstance()->getMySoliverCount().getValue());
			}

			if (myrich.getValue() < buycount * saleval * discount)
			{
				if (iscoinsale)
				{
					showstr = ResourceLang::map_lang["nomorecoin"];

					Layer* layer = BuyCoinLayer::create(buycount * saleval * discount - myrich.getValue());
					Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
				}
				else
					showstr = ResourceLang::map_lang["nomoresilver"];
			}
			else
			{
				if (!checkResIsFull())
				{
					if (totalrescount >= buycount)
					{
						bool isCanBuy = true;
						//if (m_resid.compare("v001") == 0)
						//{
						//	int vsionLv = MyRes::getMyResCount(m_resid, MYSTORAGE);
						//	if (vsionLv >= 5)
						//	{
						//		showstr = ResourceLang::map_lang["vsionmax"];
						//		isCanBuy = false;
						//	}
						//}
						if (isCanBuy)
						{
							SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUYRES);
							if (m_type == 0)
							{
								MarketLayer* parent = (MarketLayer*)this->getParent()->getParent()->getParent()->getParent();
								parent->buyRes(this->getTag(), buycount);
							}
							else
							{
								SmallStallLayer* parent = (SmallStallLayer*)this->getParent()->getParent()->getParent()->getParent();
								parent->buyRes(this->getTag(), buycount);

							}

							std::string str = StringUtils::format("%sx%d", GlobalInstance::map_AllResources[m_resid].name.c_str(), buycount);
							showstr = StringUtils::format(ResourceLang::map_lang["marketbuyok"].c_str(), str.c_str());

							totalrescount -= buycount;

							buycount = 1;
							updateData();
						}
					}
					else
					{
						showstr = ResourceLang::map_lang["outofstock"];
					}
				}
				else
				{
					showstr = StringUtils::format(ResourceLang::map_lang["storeisfull"].c_str(), GlobalInstance::map_AllResources[m_resid].name.c_str());
				}
			}
			MovingLabel::show(showstr);
			break;
		}
		case 2000:
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			std::string resstr = m_resid;
			if (m_resid.compare("r014") >= 0 && m_resid.compare("r018") <= 0)
			{
				int intid = atoi(m_resid.substr(1).c_str());
				resstr = StringUtils::format("r%03d", intid - 13);
			}
			SimpleResPopLayer* layer = SimpleResPopLayer::create(resstr);
			g_mainScene->addChild(layer);
			AnimationEffect::openAniEffect(layer);
			break;
		}
		default:
			break;
		}
	}
}

void MarketResNode::onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(MarketResNode::longTouchUpdate)))
			schedule(schedule_selector(MarketResNode::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		addCount();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void MarketResNode::onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(MarketResNode::longTouchUpdate)))
			schedule(schedule_selector(MarketResNode::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		cancelLongTouch();
		subCount();
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void MarketResNode::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		if (m_longTouchNode->getTag() % 1000 == 1)
			addCount();
		else if (m_longTouchNode->getTag() % 1000 == 2)
			subCount();
	}
}

void MarketResNode::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(MarketResNode::longTouchUpdate));
}

void MarketResNode::addCount()
{
	if (buycount >= totalrescount)
	{
		MovingLabel::show(ResourceLang::map_lang["outofstock"]);
	}
	else
	{
		buycount++;
		updateData();
	}
}

void MarketResNode::subCount()
{
	if (buycount > 1)
	{
		buycount--;
		updateData();
	}
}

void MarketResNode::reset(int maxcount)
{
	//buycount = 1;
	totalrescount = maxcount;
	updateData();
}

void MarketResNode::setEnable(bool val)
{
	isEnable = val;
	if (val)
	{
		for (int i = 0; i < csbnode->getChildrenCount(); i++)
		{
			CommonFuncs::removeGray(csbnode->getChildren().at(i));
		}
	}
	else
	{
		for (int i = 0; i < csbnode->getChildrenCount(); i++)
		{
			CommonFuncs::changeGray(csbnode->getChildren().at(i));
		}
	}
	rescountlbl->setVisible(val);
	actbtn->setVisible(val);
	addbtn->setVisible(val);
	subbtn->setVisible(val);
	openlvtext->setVisible(!val);
}

void MarketResNode::updateData()
{	
	int saleval = 0;
	DynamicValueInt myrich;
	if (GlobalInstance::map_AllResources[m_resid].coinval > 0)
	{
		saleval = GlobalInstance::map_AllResources[m_resid].coinval;
		myrich.setValue(GlobalInstance::getInstance()->getMyCoinCount().getValue());
	}
	else
	{
		saleval = GlobalInstance::map_AllResources[m_resid].silverval;
		myrich.setValue(GlobalInstance::getInstance()->getMySoliverCount().getValue());
	}

	int needrich = buycount * saleval * discount;
	if (myrich.getValue() < needrich)
	{
		totalpricelbl->setColor(Color3B(255, 0, 0));
		totalpricelbl_1->setColor(Color3B(255, 0, 0));
	}
	else
	{
		totalpricelbl->setColor(Color3B(121, 78, 46));
		totalpricelbl_1->setColor(Color3B(0, 128, 0));
	}

	std::string salestr = StringUtils::format("%d", buycount * saleval);
	totalpricelbl->setString(salestr);

	totalpricelbl_1->setPositionX(totalpricelbl->getPositionX() + totalpricelbl->getContentSize().width + 10);

	salestr = StringUtils::format("%d", needrich);
	totalpricelbl_1->setString(salestr);

	std::string countstr = StringUtils::format("%d/%d", buycount, totalrescount);
	rescountlbl->setString(countstr);


	if (m_type == 1)
	{
		if (totalpricelbl->getChildByName("mline") != NULL)
		{
			totalpricelbl->removeChildByName("mline");
		}

		DrawNode* underlineNode = DrawNode::create();
		totalpricelbl->addChild(underlineNode, 1, "mline");
		underlineNode->setLineWidth(2.0f);
		underlineNode->drawLine(Vec2(0, totalpricelbl->getContentSize().height/2), Vec2(totalpricelbl->getContentSize().width, totalpricelbl->getContentSize().height/2), Color4F(Color3B(255, 0, 0)));
	}
}

bool MarketResNode::checkResIsFull()
{
	for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
	{
		ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
		if (rescreator->getName().compare(m_resid) == 0)
		{
			int maxcount = rescreator->getMaxCap(rescreator->getLv().getValue()).getValue();
			int rcount = MyRes::getMyResCount(m_resid);
			if (rcount + buycount > maxcount)
				return true;
			else
				return false;
		}
	}
	return false;
}

int MarketResNode::getResInMarketLv()
{
	Building* buildingdata = Building::map_buildingDatas["5market"];
	for (int v = 0; v < buildingdata->maxlevel.getValue(); v++)
	{
		int vsize = buildingdata->vec_exdata.size();

		for (unsigned int i = 0; i < buildingdata->vec_exdata[v].size(); i++)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(buildingdata->vec_exdata[v][i], vec_tmp, "-");

			if (m_resid.compare(vec_tmp[0]) == 0)
			{
				return v;
			}
		}
	}
	return 0;
}

void MarketResNode::updateTime(float dt)
{
	Building* buildingdata = Building::map_buildingDatas["5market"];

	int brokenlesslv = 0;
	if (m_type == 0)
	{
		if (buildingdata->level.getValue() >= 8 && GlobalInstance::map_buildingrepairdata["5market"].state > 0)
			brokenlesslv = 8;
	}

	if (inmarktLv > buildingdata->level.getValue() - brokenlesslv)
	{
		if (GlobalInstance::map_timeMartData.find(m_resid) != GlobalInstance::map_timeMartData.end())
		{
			timelbl->setVisible(true);
			tagnode->setVisible(true);
			int zerotime = GlobalInstance::servertime + 8 * 60 * 60;
			int lefttime = 86400 - zerotime % TWENTYFOURHOURSTOSEC;
			std::string timestr = StringUtils::format("%02d:%02d:%02d", lefttime / 3600, lefttime % 3600 / 60, lefttime % 3600 % 60);
			timelbl->setString(timestr);
			if (!isEnable)
				setEnable(true);
		}
		else
		{
			timelbl->setVisible(false);
			tagnode->setVisible(false);
			if (isEnable)
				setEnable(false);
		}
	}
	else
	{
		timelbl->setVisible(false);
		tagnode->setVisible(false);
		if (!isEnable)
			setEnable(true);
	}

}