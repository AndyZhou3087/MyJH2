#include "HillResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"

HillResNode::HillResNode()
{

}


HillResNode::~HillResNode()
{

}

HillResNode* HillResNode::create(ResCreator* data)
{
	HillResNode *pRet = new(std::nothrow)HillResNode();
	if (pRet && pRet->init(data))
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

bool HillResNode::init(ResCreator* data)
{

	m_Data = data;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("hillResNode.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Widget* resbox = (cocos2d::ui::Widget*)csbnode->getChildByName("resbox");
	resbox->addTouchEventListener(CC_CALLBACK_2(HillResNode::onImgClick, this));

	cocos2d::ui::ImageView* resimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", data->getName().c_str());
	resimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[data->getName()].name);

	//储备
	maxcaptext = (cocos2d::ui::Text*)csbnode->getChildByName("maxcaptext");
	maxcaptext->setString(ResourceLang::map_lang["maxcaptext"]);

	maxcap = (cocos2d::ui::Text*)csbnode->getChildByName("maxcap");

	//产量
	outputtext = (cocos2d::ui::Text*)csbnode->getChildByName("outputtext");
	outputtext->setString(ResourceLang::map_lang["outputtext"]);

	output = (cocos2d::ui::Text*)csbnode->getChildByName("output");

	//工人人数
	farmercounttext = (cocos2d::ui::Text*)csbnode->getChildByName("farmercounttext");
	farmercounttext->setString(ResourceLang::map_lang["farmercounttext"]);

	farmercount = (cocos2d::ui::Text*)csbnode->getChildByName("farmercount");

	//按钮
	cocos2d::ui::Widget* actbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actbtn->setTag(0);
	actbtn->addTouchEventListener(CC_CALLBACK_2(HillResNode::onBtnClick, this));

	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	cocos2d::ui::ImageView* actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath("resitemlv_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//加号按钮
	cocos2d::ui::Widget* addbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("addbtn");
	addbtn->addTouchEventListener(CC_CALLBACK_2(HillResNode::onBtnClick, this));
	addbtn->setTag(1);

	//减号按钮
	cocos2d::ui::Widget* subbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("subbtn");
	subbtn->addTouchEventListener(CC_CALLBACK_2(HillResNode::onBtnClick, this));
	subbtn->setTag(2);

	updateData(0);

	this->schedule(schedule_selector(HillResNode::updateData), 1.0f);
	return true;
}

void HillResNode::updateData(float dt)
{
	std::string str;
	ResBase* res = MyRes::getMyResource(m_Data->getName());
	DynamicValueInt dtemp;
	if (res != NULL)
	{
		dtemp.setValue(res->getCount().getValue());
	}
	str = StringUtils::format("%d/%d", dtemp.getValue(), m_Data->getMaxCap(m_Data->getLv().getValue()).getValue());
	maxcap->setString(str);
	
	if (dtemp.getValue() >= m_Data->getMaxCap(m_Data->getLv().getValue()).getValue())
		maxcap->setTextColor(Color4B(255, 0, 0, 255));
	else
		maxcap->setTextColor(Color4B(255, 255, 255, 255));

	str = StringUtils::format("%d", m_Data->getFarmersCount().getValue());
	output->setString(str);

	str = StringUtils::format("%d/%d", m_Data->getFarmersCount().getValue(), m_Data->getMaxFarmersCount().getValue());
	farmercount->setString(str);

}


void HillResNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* btnnode = (Node*)pSender;
	int tag = btnnode->getTag();
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 0://升级按钮
		{
			DynamicValueInt dlv;
			dlv.setValue(m_Data->getLv().getValue() + 1);
			m_Data->setLv(dlv);
			GlobalInstance::getInstance()->saveResCreatorData();
			updateData(0);
			break;
		}
		case 1://增加工人
		{
			if (m_Data->getFarmersCount().getValue() < m_Data->getMaxFarmersCount().getValue())
			{
				if (GlobalInstance::getInstance()->getTotalFarmers() - GlobalInstance::getInstance()->getWorkingFarmerCount() > 0)
				{
					DynamicValueInt dvalue;
					dvalue.setValue(m_Data->getFarmersCount().getValue() + 1);
					m_Data->setFarmersCount(dvalue);
					GlobalInstance::getInstance()->saveResCreatorData();
					updateData(0);
				}
				else
				{
					MovingLabel::show(ResourceLang::map_lang["farmerlack"]);
				}
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["farmermaxhint"]);
			}
			break;
		}
		case 2:
			if (m_Data->getFarmersCount().getValue() > 0)
			{
				DynamicValueInt dvalue;
				dvalue.setValue(m_Data->getFarmersCount().getValue() - 1);
				m_Data->setFarmersCount(dvalue);
				GlobalInstance::getInstance()->saveResCreatorData();
				updateData(0);
			}
			break;
		default:
			break;
		}
	}
}

void HillResNode::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{

	}
}