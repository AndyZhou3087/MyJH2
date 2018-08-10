#include "MessageNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "Const.h"
#include "MessageDescLayer.h"

MessageNode::MessageNode()
{
	lastStatus = -1;
}


MessageNode::~MessageNode()
{

}

MessageNode* MessageNode::create(int index)
{
	MessageNode *pRet = new(std::nothrow)MessageNode();
	if (pRet && pRet->init(index))
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

bool MessageNode::init(int index)
{
	MessageData msgdata = GlobalInstance::vec_messsages[index];

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("messageNode.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Widget* bg = (cocos2d::ui::Widget*)csbnode->getChildByName("resitem");
	bg->addTouchEventListener(CC_CALLBACK_2(MessageNode::onImgClick, this));
	bg->setSwallowTouches(false);

	icon = (cocos2d::ui::Widget*)csbnode->getChildByName("icon");
	if (msgdata.type == 0)
		icon->setVisible(false);
	else
	{
		if (msgdata.status != 0)
			icon->setOpacity(128);
	}
	
	redpoint = (cocos2d::ui::Widget*)csbnode->getChildByName("redpoint");
	if (msgdata.status != 0)
		redpoint->setVisible(false);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(msgdata.title);

	return true;
}

void MessageNode::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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

	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		MessageDescLayer* layer = MessageDescLayer::create(this->getTag());
		this->getParent()->getParent()->getParent()->getParent()->addChild(layer, 0, this->getTag());
	}
}


void MessageNode::updataUI()
{
	int status = GlobalInstance::vec_messsages[this->getTag()].status;
	if (lastStatus != status)
	{
		lastStatus = status;
		if (status == 1)
		{
			redpoint->setVisible(false);
			icon->setOpacity(128);
		}
	}
}