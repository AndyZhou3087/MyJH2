#include "TaskMainDescLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "TaskMainNode.h"

TaskMainDescLayer::TaskMainDescLayer()
{

}


TaskMainDescLayer::~TaskMainDescLayer()
{

}

TaskMainDescLayer* TaskMainDescLayer::create(TaskMainData* data)
{
	TaskMainDescLayer *pRet = new(std::nothrow)TaskMainDescLayer();
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

bool TaskMainDescLayer::init(TaskMainData* data)
{

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainDescLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)m_csbnode->getChildByName("name");
	name->setString(data->name);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)m_csbnode->getChildByName("desc");
	desc->setString(data->desc);

	//npc头像
	/*cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("icon");
	std::string str = StringUtils::format("ui/%s.png", data->npcid);
	icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	//npc名字
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)m_csbnode->getChildByName("npcname");
	npcname->setString(data->);*/

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->setTag(0);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskMainDescLayer::onBtnClick, this));

	cocos2d::ui::Widget* accbtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("accbtn");
	accbtn->setTag(1);
	accbtn->addTouchEventListener(CC_CALLBACK_2(TaskMainDescLayer::onBtnClick, this));

	cocos2d::ui::Widget* getbtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("getbtn");
	getbtn->setTag(2);
	getbtn->addTouchEventListener(CC_CALLBACK_2(TaskMainDescLayer::onBtnClick, this));

	if (data->isfinish == 1)
	{
		getbtn->setVisible(true);
		accbtn->setVisible(false);
	}
	else
	{
		getbtn->setVisible(false);
		accbtn->setVisible(true);
	}

	if (data->id != GlobalInstance::myCurTaskMain->id && data->isfinish == 0)
	{
		getbtn->setVisible(false);
		accbtn->setVisible(false);
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void TaskMainDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 0:
			this->removeFromParentAndCleanup(true);
			break;
		case 1: //接受任务
			break;
		case 2: //完成后领取奖励
			break;
		default:
			break;
		}
	}
}