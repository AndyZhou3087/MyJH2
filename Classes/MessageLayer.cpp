#include "MessageLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MessageNode.h"
#include "HttpDataSwap.h"
#include "WaitingProgress.h"
#include "AnimationEffect.h"

USING_NS_CC;

MessageLayer::MessageLayer()
{

}

MessageLayer::~MessageLayer()
{
	
}


MessageLayer* MessageLayer::create()
{
	MessageLayer *pRet = new(std::nothrow)MessageLayer();
	if (pRet && pRet->init())
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

// on "init" you need to initialize your instance
bool MessageLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("messageLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("messagetitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	//一键已读按钮
	cocos2d::ui::Widget* onekeyreadbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("onekeyreadbtn");
	onekeyreadbtn->setTag(1000);
	onekeyreadbtn->addTouchEventListener(CC_CALLBACK_2(MessageLayer::onBtnClick, this));
	//一键已读文字
	cocos2d::ui::ImageView* onekeyreadbtntxt = (cocos2d::ui::ImageView*)onekeyreadbtn->getChildByName("text");
	onekeyreadbtntxt->loadTexture(ResourcePath::makeTextImgPath("onkeyread_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//清空已读按钮
	cocos2d::ui::Widget* cleanreadbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cleanreadbtn");
	cleanreadbtn->setTag(1001);
	cleanreadbtn->addTouchEventListener(CC_CALLBACK_2(MessageLayer::onBtnClick, this));

	//清空已读按钮文字
	cocos2d::ui::ImageView* cleanreadbtntxt = (cocos2d::ui::ImageView*)cleanreadbtn->getChildByName("text");
	cleanreadbtntxt->loadTexture(ResourcePath::makeTextImgPath("cleanread_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//全部领取按钮
	cocos2d::ui::Widget* allgetbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("allgetbtn");
	allgetbtn->setTag(1002);
	allgetbtn->addTouchEventListener(CC_CALLBACK_2(MessageLayer::onBtnClick, this));

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1003);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MessageLayer::onBtnClick, this));

	//消息滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	this->addChild(wp, 0, "waitingprogress");
	HttpDataSwap::init(this)->getMessageList();

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MessageLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://一键已读
		{
			bool hasunread = false;
			for (unsigned int i = 0; i < GlobalInstance::vec_messsages.size(); i++)
			{
				if (GlobalInstance::vec_messsages[i].status == 0)
				{
					GlobalInstance::vec_messsages[i].status = 1;
					updateStatus(i);
					hasunread = true;
				}
			}
			if (hasunread)
				HttpDataSwap::init(NULL)->updateMessageStatus("0", 1);
			break;
		}
		case 1001://清空已读
		{
			bool ishasread = false;
			std::vector<MessageData>::iterator it;
			for (it = GlobalInstance::vec_messsages.begin(); it != GlobalInstance::vec_messsages.end();)
			{
				if ((it->type == 0 && it->status == 1) || (it->type != 0 && it->status == 3))
				{
					it = GlobalInstance::vec_messsages.erase(it);
					ishasread = true;
				}
				else
				{
					it++;
				}
			}
			if (ishasread)
			{
				HttpDataSwap::init(NULL)->updateMessageStatus("0", 2);
				refreshScrollViewUi();
			}
		}
			break;
		case 1002://全部领取
		{
			bool hasawd = false;
			for (unsigned int i = 0; i < GlobalInstance::vec_messsages.size(); i++)
			{
				if (GlobalInstance::vec_messsages[i].type != 0 && GlobalInstance::vec_messsages[i].status < 2)
				{
					GlobalInstance::vec_messsages[i].status = 3;
					updateStatus(i);
					hasawd = true;
				}
			}
			if (hasawd)
			{
				HttpDataSwap::init(NULL)->updateMessageStatus("0", 3);
			}
		}
			break;
		case 1003://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}
	}
}

void MessageLayer::refreshScrollViewUi()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int size = GlobalInstance::vec_messsages.size();
	int itemheight = 140;
	int innerheight = itemheight * size;
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(650, innerheight));

	for (int i = 0; i < size; i++)
	{
		MessageNode* nodeitem = MessageNode::create(i);
		nodeitem->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
		nodeitem->runAction(EaseSineIn::create(MoveBy::create(0.10f + i*0.05f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));
		m_contentscroll->addChild(nodeitem, 0, i);

	}
}

void MessageLayer::onExit()
{
	Layer::onExit();
}

void MessageLayer::onFinish(int code)
{
	this->removeChildByName("waitingprogress");
	if (code == SUCCESS)
	{
		refreshScrollViewUi();
	}
}

void MessageLayer::updateStatus(int index)
{
	MessageNode* nodeitem = (MessageNode*)m_contentscroll->getChildByTag(index);
	nodeitem->updataUI();
}
