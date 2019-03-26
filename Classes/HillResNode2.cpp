#include "HillResNode2.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "MainScene.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "HH_ResActionLayer.h"

int needfood2[] = { 2,3,4,10 };
Vec2 addbtnPos[] = { Vec2(95, 20), Vec2(-100, 65), Vec2(20, 20), Vec2(75, -20), Vec2(-145, -50) }; 
Vec2 subbtnPos[] = { Vec2(-120, 0), Vec2(125, 15), Vec2(-130, -35), Vec2(-128, -65), Vec2(165, -20) };

HillResNode2::HillResNode2()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
}

HillResNode2::~HillResNode2()
{

}

HillResNode2* HillResNode2::create(int rescreatorindex)
{
	HillResNode2 *pRet = new(std::nothrow)HillResNode2();
	if (pRet && pRet->init(rescreatorindex))
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

bool HillResNode2::init(int rescreatorindex)
{

	if (!Node::init())
		return false;

	m_resCreator = GlobalInstance::vec_resCreators[rescreatorindex];

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("homeResNode2.csb"));
	this->addChild(csbnode, 0, "csbnode");

	int langtype = GlobalInstance::getInstance()->getLang();
	//储备
	maxcaptext = (cocos2d::ui::ImageView*)csbnode->getChildByName("captext");
	maxcaptext->loadTexture(ResourcePath::makeTextImgPath("hh_cap_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	maxcap = (cocos2d::ui::Text*)csbnode->getChildByName("capval");

	//产量
	outputtext = (cocos2d::ui::ImageView*)csbnode->getChildByName("outputtext");
	outputtext->loadTexture(ResourcePath::makeTextImgPath("hh_output_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	output = (cocos2d::ui::Text*)csbnode->getChildByName("outputval");

	//按钮
	cocos2d::ui::Widget* actbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("reslvbtn");
	actbtn->setTag(1000);
	actbtn->addTouchEventListener(CC_CALLBACK_2(HillResNode2::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath("hh_reslvbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//加号按钮
	cocos2d::ui::Widget* addbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("addclick");
	addbtn->addTouchEventListener(CC_CALLBACK_2(HillResNode2::onAddBtnClick, this));
	addbtn->setTag(1001);
	addbtn->setPosition(addbtnPos[rescreatorindex]);

	//减号按钮
	cocos2d::ui::Widget* subbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("subclick");
	subbtn->addTouchEventListener(CC_CALLBACK_2(HillResNode2::onSubBtnClick, this));
	subbtn->setTag(1002);
	subbtn->setPosition(subbtnPos[rescreatorindex]);
	updateData(0);

	this->schedule(schedule_selector(HillResNode2::updateData), 0.2f);
	return true;
}

void HillResNode2::updateData(float dt)
{

	DynamicValueInt dtemp;

	dtemp.setValue(MyRes::getMyResCount(m_resCreator->getName()));

	std::string str = StringUtils::format("%d/%d", dtemp.getValue(), m_resCreator->getMaxCap(m_resCreator->getLv().getValue()).getValue());
	maxcap->setString(str);
	
	//if (dtemp.getValue() >= m_resCreator->getMaxCap(m_resCreator->getLv().getValue()).getValue())
	//	maxcap->setTextColor(Color4B(255, 0, 0, 255));
	//else
	//	maxcap->setTextColor(Color4B(255, 255, 255, 255));

	int outcount = 0;
	if (m_resCreator->getName().compare("r001") == 0)
		outcount = GlobalInstance::getInstance()->calcFoodMakeOut();
	else
	{
		//if (MyRes::getMyResCount("r001") <= 0)
		//	outcount = 0;
		//else
			outcount = m_resCreator->getFarmersCount().getValue();
	}

	if (outcount >= 0)
		output->setTextColor(Color4B(255, 255, 255, 255));
	else
		output->setTextColor(Color4B(255, 0, 0, 255));

	str = StringUtils::format("%d/%d", outcount, m_resCreator->getMaxFarmersCount().getValue());
	output->setString(str);
}

void HillResNode2::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{

	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();

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
		case 1000://升级按钮
		{
			if (m_resCreator->getLv().getValue() >= (RESCREATOREMAXLV-1))
			{
				MovingLabel::show(ResourceLang::map_lang["maxlv"]);
			}
			else
			{
				SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
				HH_ResActionLayer* layer = HH_ResActionLayer::create(m_resCreator, HH_RESCREATORLVUP);
				this->getParent()->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			break;
		}
		default:
			break;
		}
	}
}

void HillResNode2::onAddBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(HillResNode2::longTouchUpdate)))
			schedule(schedule_selector(HillResNode2::longTouchUpdate), 0.1f);
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

void HillResNode2::onSubBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(HillResNode2::longTouchUpdate)))
			schedule(schedule_selector(HillResNode2::longTouchUpdate), 0.1f);
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

void HillResNode2::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		if (m_longTouchNode->getTag() % 1000 == 1)
			addCount();
		else if (m_longTouchNode->getTag() % 1000 == 2)
			subCount();
	}
}

void HillResNode2::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(HillResNode2::longTouchUpdate));
}

void HillResNode2::addCount()
{
	if (m_resCreator->getFarmersCount().getValue() < m_resCreator->getMaxFarmersCount().getValue())
	{
		if (GlobalInstance::getInstance()->getTotalFarmers() - GlobalInstance::getInstance()->getWorkingFarmerCount() > 0)
		{
			int foodcount = GlobalInstance::getInstance()->calcFoodMakeOut();
			if (GlobalInstance::vec_resCreators.size() > 0)
			{
				for (unsigned int i = 1; i < GlobalInstance::vec_resCreators.size(); i++)
				{
					ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
					if (m_resCreator->getName().compare(rescreator->getName()) == 0)
					{
						foodcount -= needfood2[i - 1];
						break;
					}
				}
			}

			if (foodcount + MyRes::getMyResCount("r001") < 0)
			{
				std::string str = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), GlobalInstance::map_AllResources["r001"].name.c_str());
				MovingLabel::show(str);
			}
			else
			{
				DynamicValueInt dvalue;
				dvalue.setValue(m_resCreator->getFarmersCount().getValue() + 1);
				m_resCreator->setFarmersCount(dvalue);
				GlobalInstance::getInstance()->saveResCreatorData();
				updateData(0);
			}
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
}

void HillResNode2::subCount()
{
	if (m_resCreator->getFarmersCount().getValue() > 0)
	{
		DynamicValueInt dvalue;
		dvalue.setValue(m_resCreator->getFarmersCount().getValue() - 1);
		m_resCreator->setFarmersCount(dvalue);
		GlobalInstance::getInstance()->saveResCreatorData();
		updateData(0);
	}
}