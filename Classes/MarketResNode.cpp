#include "MarketResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "MarketLayer.h"
MarketResNode::MarketResNode()
{
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

	totalsilverlbl = (cocos2d::ui::Text*)csbnode->getChildByName("totalsilver");

	rescountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("rescount");

	updateData();
	//按钮
	cocos2d::ui::Widget* actbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actbtn->setTag(1000);
	actbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onBtnClick, this));

	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	cocos2d::ui::ImageView* actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath("buybtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//加号按钮
	cocos2d::ui::Widget* addbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("addbtn");
	addbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onBtnClick, this));
	addbtn->setTag(1001);

	//减号按钮
	cocos2d::ui::Widget* subbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("subbtn");
	subbtn->addTouchEventListener(CC_CALLBACK_2(MarketResNode::onBtnClick, this));
	subbtn->setTag(1002);

	return true;
}

void MarketResNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* btnnode = (Node*)pSender;
	int tag = btnnode->getTag();
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 1000://购买按钮
		{
			std::string showstr;
			if (GlobalInstance::getInstance()->getMySoliverCount().getValue() < buycount * GlobalInstance::map_AllResources[m_resid].saleval)
			{
				showstr = ResourceLang::map_lang["nomoresilver"];
			}
			else
			{
				if (!checkResIsFull())
				{
					MarketLayer* parent = (MarketLayer*)this->getParent()->getParent()->getParent()->getParent();
					parent->buyRes(this->getTag(), buycount);

					std::string str = StringUtils::format("%sx%d", GlobalInstance::map_AllResources[m_resid].name.c_str(), buycount);
					showstr = StringUtils::format(ResourceLang::map_lang["marketbuyok"].c_str(), str.c_str());

					totalrescount -= buycount;

					if (totalrescount <= 0)
						buycount = 0;
					else
						buycount = 1;
					updateData();
				}
				else
				{
					showstr = StringUtils::format(ResourceLang::map_lang["storeisfull"].c_str(), GlobalInstance::map_AllResources[m_resid].name.c_str());
				}
			}
			MovingLabel::show(showstr);
			break;
		}
		case 1001://增加按钮
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
			break;
		}
		case 1002://减号按钮
			if (buycount > 1)
			{
				buycount--;
				updateData();
			}
			break;
		default:
			break;
		}
	}
}

void MarketResNode::reset(int maxcount)
{
	buycount = 1;
	totalrescount = maxcount;
	updateData();
}

void MarketResNode::updateData()
{
	std::string salestr = StringUtils::format("%d", buycount * GlobalInstance::map_AllResources[m_resid].saleval);
	totalsilverlbl->setString(salestr);

	if (GlobalInstance::getInstance()->getMySoliverCount().getValue() < buycount * GlobalInstance::map_AllResources[m_resid].saleval)
	{
		totalsilverlbl->setColor(Color3B(255, 0, 0));
	}
	else
	{
		totalsilverlbl->setColor(Color3B(121, 78, 46));
	}

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