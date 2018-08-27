#include "HillResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "TaskNode.h"
#include "TaskDescLayer.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"

TaskNode::TaskNode()
{

}


TaskNode::~TaskNode()
{

}

TaskNode* TaskNode::create(TaskData* data, int type)
{
	TaskNode *pRet = new(std::nothrow)TaskNode();
	if (pRet && pRet->init(data, type))
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

bool TaskNode::init(TaskData* data, int type)
{
	m_type = type;
	m_Data = data;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainNode.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* resitem = (cocos2d::ui::ImageView*)csbnode->getChildByName("resitem");
	resitem->addTouchEventListener(CC_CALLBACK_2(TaskNode::onImgClick, this));
	resitem->setSwallowTouches(false);

	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(data->name);

	//提示红点
	redpoint = (cocos2d::ui::ImageView*)csbnode->getChildByName("redpoint");

	//
	finish = (cocos2d::ui::Widget*)csbnode->getChildByName("finish");
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)finish->getChildByName("Image");
	text->loadTexture(ResourcePath::makeTextImgPath("taskfinish", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	updateData(0);

	this->schedule(schedule_selector(TaskNode::updateData), 1.0f);
	return true;
}

void TaskNode::updateData(float dt)
{
	if (m_Data->isfinish == QUEST_FINISH)
	{
		finish->setVisible(true);
	}
	else if (m_Data->isfinish == QUEST_GET)
	{
		finish->setVisible(true);
		redpoint->setVisible(false);
	}
	else
	{
		finish->setVisible(false);
		redpoint->setVisible(false);
		int id;
		if (m_type == 0)
		{
			id = GlobalInstance::myCurMainData.id;
		}
		else
		{
			id = GlobalInstance::myCurBranchData.id;
		}
		if (m_Data->id == id)
		{
			redpoint->setVisible(true);
		}
	}

}

void TaskNode::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
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
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Layer* layer = TaskDescLayer::create(m_Data, m_type);
		TaskLayer* m_layer = (TaskLayer*)this->getParent()->getParent()->getParent();
		if (m_layer!=NULL)
		{
			m_layer->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
	}
}