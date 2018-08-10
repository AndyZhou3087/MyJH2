#include "MessageDescLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "HttpDataSwap.h"
#include "MessageLayer.h"

MessageDescLayer::MessageDescLayer()
{

}


MessageDescLayer::~MessageDescLayer()
{

}

MessageDescLayer* MessageDescLayer::create(int index)
{
	MessageDescLayer *pRet = new(std::nothrow)MessageDescLayer();
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

bool MessageDescLayer::init(int index)
{
	MessageData data = GlobalInstance::vec_messsages[index];

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("messageDescLayer.csb"));
	this->addChild(m_csbnode);

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)m_csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Widget* awdnode = (cocos2d::ui::Widget*)m_csbnode->getChildByName("awdnode");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	std::string actiontextstr;
	int btntag = 0;

	int textscrollheight = 0;

	if (data.type == 0)
	{
		actiontextstr = "closebtn_text";
		btntag = 1000;
		textscrollheight = 500;
		awdnode->setVisible(false);
	}
	else
	{
		actiontextstr = "msgallget_text";
		btntag = 1001;
		textscrollheight = 350;
	}
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(MessageDescLayer::onBtnClick, this));
	actionbtn->setTag(btntag);
	//按钮1文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath(actiontextstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntxt->ignoreContentAdaptWithSize(true);

	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)m_csbnode->getChildByName("title");
	title->setString(data.title);

	cocos2d::ui::ScrollView* contentscroll = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("contentscroll");

	contentscroll->setContentSize(Size(contentscroll->getContentSize().width, textscrollheight));
	contentscroll->setBounceEnabled(true);

	int contentwidth = 500;
	Label* contentlbl = Label::createWithTTF(ResourceLang::map_lang["testtext"]/*data.content*/, FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(255, 255, 255));
	contentlbl->setHorizontalAlignment(TextHAlignment::LEFT);
	contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setMaxLineWidth(contentwidth);
	contentscroll->addChild(contentlbl);
	int innerheight = contentlbl->getStringNumLines() * 25;//contentlbl->getHeight();
	int contentheight = contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	contentscroll->setInnerContainerSize(Size(contentscroll->getContentSize().width, innerheight));
	contentlbl->setPosition(Vec2((contentscroll->getContentSize().width - contentwidth)/2, innerheight));

	if (data.status == 0)
	{
		HttpDataSwap::init(NULL)->updateMessageStatus(data.id, 1);
	}

	this->scheduleOnce(schedule_selector(MessageDescLayer::delay), 0.5f);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		this->removeFromParentAndCleanup(true);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void MessageDescLayer::delay(float dt)
{
	int tag = this->getTag();
	GlobalInstance::vec_messsages[tag].status = 1;
	MessageLayer* mlayer = (MessageLayer*)this->getParent();
	mlayer->updateStatus(tag);
}

void MessageDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 1000:
			this->removeFromParentAndCleanup(true);
			break;
		case 1001:

			break;

		default:
			break;
		}
	}
}