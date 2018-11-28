#include "MatchRankLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"
#include "WaitingProgress.h"

USING_NS_CC;

MatchRankLayer::MatchRankLayer()
{

}

MatchRankLayer::~MatchRankLayer()
{

}


MatchRankLayer* MatchRankLayer::create()
{
	MatchRankLayer *pRet = new(std::nothrow)MatchRankLayer();
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
bool MatchRankLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 160));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchRankLayer.csb"));
	this->addChild(csbnode);

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	title->loadTexture(ResourcePath::makeTextImgPath("ranktitle_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	text = (cocos2d::ui::Text*)csbnode->getChildByName("text");
	text->setVisible(false);
	text->setString(ResourceLang::map_lang["doingtext"]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MatchRankLayer::onBtnClick, this));

	scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");

	WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	Director::getInstance()->getRunningScene()->addChild(waitbox, 100, "waitbox");

	HttpDataSwap::init(this)->getMatchRankHeros();


	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	//listener->onTouchEnded = [=](Touch *touch, Event *event)
	//{
	//	AnimationEffect::closeAniEffect((Layer*)this);
	//};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void MatchRankLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void MatchRankLayer::loadMyRankHeros()
{
	scrollView->removeAllChildrenWithCleanup(true);
	int size = GlobalInstance::myRankInfo.vec_rankData.size();
	int itemheight = 170;
	int innerheight = size * itemheight;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));

	for (int i = 0; i < size; i++)
	{
		MatchRankNode* node = MatchRankNode::create(GlobalInstance::myRankInfo.vec_rankData[i]);
		scrollView->addChild(node);
		node->setPosition(Vec2(324, innerheight - i * itemheight - itemheight / 2));
	}
}

void MatchRankLayer::onFinish(int code)
{
	Director::getInstance()->getRunningScene()->removeChildByName("waitbox");
	if (code == 0)
	{
		this->removeChildByName("waitbox");
		text->setVisible(true);

		MyRankData data;
		mynode = MatchRankNode::create(data, 1);
		this->addChild(mynode);
		mynode->setPosition(Vec2(352, 120));

		loadMyRankHeros();

		text->setVisible(false);
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	}
}