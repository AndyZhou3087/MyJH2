#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "AchieveNode.h"
#include "Const.h"
#include "DataSave.h"
#include "TaskLayer.h"
#include "Building.h"
#include "MainScene.h"
#include "InnRoomLayer.h"
#include "SmithyLayer.h"
#include "StoreHouseLayer.h"
#include "Quest.h"
#include "ResCreator.h"
#include "AnimationEffect.h"

AchieveNode::AchieveNode()
{

}


AchieveNode::~AchieveNode()
{

}

AchieveNode* AchieveNode::create(AchieveData* data)
{
	AchieveNode *pRet = new(std::nothrow)AchieveNode();
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

bool AchieveNode::init(AchieveData* data)
{
	m_Data = data;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("achieveNode.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* resitem = (cocos2d::ui::ImageView*)csbnode->getChildByName("resitem");
	resitem->setSwallowTouches(false);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["dailyrlabel"]);

	//名字
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	std::string str = StringUtils::format("dailytype_%d", data->type);
	std::string s;
	if (data->type == ACHIEVE_FIGHT || data->type == ACHIEVE_GOODS)
	{
		s = StringUtils::format(ResourceLang::map_lang[str].c_str(), GlobalInstance::map_AllResources[data->achid].name.c_str(), data->count);
	}
	else
	{
		s = StringUtils::format(ResourceLang::map_lang[str].c_str(), data->count);
	}
	name->setString(s);

	finish = (cocos2d::ui::Widget*)csbnode->getChildByName("finish");
	getbtn = (cocos2d::ui::Button*)csbnode->getChildByName("getbtn");
	getbtn->addTouchEventListener(CC_CALLBACK_2(AchieveNode::onbtnClick, this));
	getbtn->setTag(1000);
	getbtn->setSwallowTouches(false);

	taskprobar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("taskprobar");
	taskprotext = (cocos2d::ui::Text*)csbnode->getChildByName("taskprotext");

	//奖励的物品
	for (unsigned int i = 0; i < data->rewards.size(); i++)
	{
		std::string resid = data->rewards[i][0];
		int count = atoi(data->rewards[i][1].c_str());
		int qu = atoi(data->rewards[i][2].c_str());
		s = StringUtils::format("reward_%d", i);
		cocos2d::ui::ImageView* reward = (cocos2d::ui::ImageView*)csbnode->getChildByName(s);
		reward->setVisible(true);
		s = StringUtils::format("ui/%s.png", resid.c_str());
		if (qu == 3)
		{
			s = StringUtils::format("ui/%s-2.png", resid.c_str());
		}
		else if (qu == 4)
		{
			s = StringUtils::format("ui/%s-3.png", resid.c_str());
		}
		reward->loadTexture(s, cocos2d::ui::Widget::TextureResType::PLIST);
		if (i == 0)
		{
			std::string boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			res->loadTexture(s, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		s = StringUtils::format("text_%d", i); 
		cocos2d::ui::Text* text = (cocos2d::ui::Text*)csbnode->getChildByName(s);
		text->setVisible(true);
		s = StringUtils::format("%d", count);
		text->setString(s);
	}

	updateData(0);

	this->schedule(schedule_selector(AchieveNode::updateData), 1.0f);
	return true;
}

void AchieveNode::updateData(float dt)
{
	if (m_Data->state == DAILY_UNFINISHED)
	{
		finish->setVisible(false);
		getbtn->setVisible(false);
	}
	else if (m_Data->state == DAILY_FINISHED)
	{
		finish->setVisible(false);
		getbtn->setVisible(true);
	}
	else
	{
		finish->setVisible(true);
		getbtn->setVisible(false);
	}

	int c = Quest::map_achieveTypeCount[m_Data->type];
	if (c >= m_Data->count)
	{
		c = m_Data->count;
	}
	float h = c * 100 / m_Data->count;
	taskprobar->setPercent(h);
	std::string str = StringUtils::format("%d/%d", c, m_Data->count);
	taskprotext->setString(str);
}

void AchieveNode::onbtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);

	Node* clicknode = (Node*)pSender;
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

		Node* node = (Node*)pSender;
		int tag = node->getTag();

		//物品
		for (unsigned int i = 0; i < m_Data->rewards.size(); i++)
		{
			std::vector<std::string> one_res = m_Data->rewards[i];
			std::string resid = one_res[0];
			int count = atoi(one_res[1].c_str());
			int qu = 0;
			int stc = 0;
			if (one_res.size()>2 && one_res[2].length()>0)
			{
				qu = atoi(one_res[2].c_str());
				stc = GlobalInstance::getInstance()->generateStoneCount(qu);
			}

			for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
			{
				ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
				if (rescreator->getName().compare(resid) == 0)
				{
					int maxcount = rescreator->getMaxCap(rescreator->getLv().getValue()).getValue();
					if (MyRes::getMyResCount(resid) + count>maxcount)
					{
						MovingLabel::show(ResourceLang::map_lang["nomoreres"]);
						return;
					}
				}
			}
			MyRes::Add(resid, count, MYSTORAGE, qu, stc);
		}

		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)pSender;
		btn->setTouchEnabled(false);

		m_Data->state = DAILY_RECEIVE;
		GlobalInstance::getInstance()->saveMyAchieveData();
	}
}