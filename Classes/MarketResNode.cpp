#include "MarketResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "MarketLayer.h"
#include "Const.h"

MarketResNode::MarketResNode()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;

	buycount = 1;
}


MarketResNode::~MarketResNode()
{

}

MarketResNode* MarketResNode::create(std::string resid, int rescount)
{
	MarketResNode *pRet = new(std::nothrow)MarketResNode();
	if (pRet && pRet->init(resid, rescount))
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

bool MarketResNode::init(std::string resid, int rescount)
{
	m_resid = resid;
	totalrescount = rescount; 
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("marketResNode.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Widget* resbox = (cocos2d::ui::Widget*)csbnode->getChildByName("resbox");
	//resbox->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onImgClick, this));
	//resbox->setSwallowTouches(false);

	cocos2d::ui::ImageView* resimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", resid.c_str());
	resimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[resid].name);

	//描述
	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(GlobalInstance::map_AllResources[resid].desc);

	totalpricelbl = (cocos2d::ui::Text*)csbnode->getChildByName("totalprice");

	priceicon = (cocos2d::ui::ImageView*)csbnode->getChildByName("priceicon");
	priceicon->ignoreContentAdaptWithSize(true);

	if (GlobalInstance::map_AllResources[resid].coinval > 0)
		priceicon->loadTexture(ResourcePath::makePath("ui/main_coin.png"), cocos2d::ui::Widget::TextureResType::PLIST);

	rescountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("rescount");

	updateData();
	//按钮
	cocos2d::ui::Widget* actbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actbtn->setTag(1000);
	actbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onBtnClick, this));
	actbtn->setSwallowTouches(false);

	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	cocos2d::ui::ImageView* actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath("buybtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//加号按钮
	cocos2d::ui::Widget* addbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("addbtn");
	addbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onAddBtnClick, this));
	addbtn->setTag(1001);
	//减号按钮
	cocos2d::ui::Widget* subbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("subbtn");
	subbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onSubBtnClick, this));
	subbtn->setTag(1002);
	return true;
}

void MarketResNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);

	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();

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

			if (myrich.getValue() < buycount * saleval)
			{
				if (iscoinsale)
					showstr = ResourceLang::map_lang["nomorecoin"];
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
						if (m_resid.compare("v001") == 0)
						{
							int vsionLv = MyRes::getMyResCount(m_resid, MYSTORAGE);
							if (vsionLv >= 5)
							{
								showstr = ResourceLang::map_lang["vsionmax"];
								isCanBuy = false;
							}
						}
						if (isCanBuy)
						{
							MarketLayer* parent = (MarketLayer*)this->getParent()->getParent()->getParent()->getParent();
							parent->buyRes(this->getTag(), buycount);

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
			schedule(schedule_selector(MarketResNode::longTouchUpdate), 0.2f);
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
			schedule(schedule_selector(MarketResNode::longTouchUpdate), 0.2f);
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

	if (myrich.getValue() < buycount * saleval)
	{
		totalpricelbl->setColor(Color3B(255, 0, 0));
	}
	else
	{
		totalpricelbl->setColor(Color3B(121, 78, 46));
	}

	std::string salestr = StringUtils::format("%d", buycount * saleval);
	totalpricelbl->setString(salestr);

	std::string countstr = StringUtils::format("%d/%d", buycount, totalrescount);
	rescountlbl->setString(countstr);
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

void MarketResNode::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{

	}
}