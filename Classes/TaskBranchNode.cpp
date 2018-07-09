#include "HillResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "TaskBranchNode.h"
#include "TaskBranchDescLayer.h"

TaskBranchNode::TaskBranchNode()
{

}


TaskBranchNode::~TaskBranchNode()
{

}

TaskBranchNode* TaskBranchNode::create(TaskBranchData* data, TaskLayer* layer)
{
	TaskBranchNode *pRet = new(std::nothrow)TaskBranchNode();
	if (pRet && pRet->init(data, layer))
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

bool TaskBranchNode::init(TaskBranchData* data, TaskLayer* layer)
{
	m_layer = layer;
	m_Data = data;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainNode.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* resitem = (cocos2d::ui::ImageView*)csbnode->getChildByName("resitem");
	resitem->addTouchEventListener(CC_CALLBACK_2(TaskBranchNode::onImgClick, this));
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

	this->schedule(schedule_selector(TaskBranchNode::updateData), 1.0f);
	return true;
}

void TaskBranchNode::updateData(float dt)
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
		if (m_Data->id == GlobalInstance::myCurBranchData.id)
		{
			redpoint->setVisible(true);
		}
	}

}

void TaskBranchNode::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = TaskBranchDescLayer::create(m_Data);
		if (m_layer != NULL)
		{
			m_layer->addChild(node);
		}
	}
}