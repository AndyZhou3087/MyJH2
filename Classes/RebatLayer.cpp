#include "RebatLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "DynamicValue.h"
#include "ShopLayer.h"
#include "MyRes.h"
#include "WaitingProgress.h"
#include "SoundManager.h"
#include "RebateAwdNode.h"

USING_NS_CC;


int rebatprice[] = {0, 100019,100099, 100199, 100499, 100999, 101999, 104999, 1009999 };

std::string rebateawds = {"i002-5,r006-10000,z003-2;y003-2,b005-2,r006-20000;f003-1-4,g003-1-4,e003-1-4;q001-300,q002-300,b005-10;f004-1-4,i004-20,b005-20;g004-1-4,e004-1-4,u002-2;x036-1,b005-40,u002-4;b005-80,i004-50,u002-8"};

RebatLayer::RebatLayer()
{

}

RebatLayer::~RebatLayer()
{

}


RebatLayer* RebatLayer::create()
{
	RebatLayer *pRet = new(std::nothrow)RebatLayer();
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
bool RebatLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("rebatLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	title->loadTexture(ResourcePath::makeTextImgPath("rebattitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* totalbuytext = (cocos2d::ui::ImageView*)csbnode->getChildByName("totalbuytextimg");
	totalbuytext->loadTexture(ResourcePath::makeTextImgPath("rebattotalbuy_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* rewardtext = (cocos2d::ui::ImageView*)csbnode->getChildByName("rewardtextimg");
	rewardtext->loadTexture(ResourcePath::makeTextImgPath("rebatreward_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(RebatLayer::onBtnClick, this));
	closebtn->setTag(1000);

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("ScrollView");

	std::vector<std::string> vec_ads;
	CommonFuncs::split(rebateawds, vec_ads, ";");
	for (int i = 1; i <= 8; i++)
	{
		std::string name = StringUtils::format("price%dfnt", i);

		float scale = 0.9f;
		cocos2d::ui::TextBMFont* tbf = (cocos2d::ui::TextBMFont*)scrollview->getChildByName(name);
		std::string str = StringUtils::format("%d", rebatprice[i] % 100000);
		tbf->setScale(scale);
		tbf->setString(str);

		name = StringUtils::format("yuan%d", i);
		Node* yuan = scrollview->getChildByName(name);
		yuan->setPositionX(tbf->getPositionX() + tbf->getContentSize().width*scale);
	}

	int incount = 0;

	for (int i = 0; i < sizeof(rebatprice) / sizeof(rebatprice[0]); i++)
	{
		if (GlobalInstance::totalPayAmout >= rebatprice[i]%100000)
			incount = i;
	}

	for (int i = 0; i < 8; i++)
	{

		RebateAwdNode* nodeitem = RebateAwdNode::create(vec_ads[i]);
		nodeitem->setPosition(Vec2(400, 1100 - 145 * i));
		scrollview->addChild(nodeitem, 0, i);

		std::string barbgname = StringUtils::format("price%dbg", i);
		Node* barbg = scrollview->getChildByName(barbgname);
		Sprite *barsprite = Sprite::createWithSpriteFrameName("ui/rebatpricebar.png");
		barsprite->setContentSize(Size(barbg->getContentSize()));

		ProgressTimer* progressTimer = ProgressTimer::create(barsprite);

		progressTimer->setType(ProgressTimer::Type::BAR);

		progressTimer->setMidpoint(Vec2(0.5, 1));
		progressTimer->setBarChangeRate(Vec2(0, 1));
		if (i < incount)
		{
			progressTimer->setPercentage(100);
			
			nodeitem->setSelect();
		}
		else if (i == incount)
			progressTimer->setPercentage((GlobalInstance::totalPayAmout - rebatprice[incount] % 100000) * 100 / (rebatprice[incount + 1] - rebatprice[incount]));

		progressTimer->setPosition(barbg->getPosition());
		scrollview->addChild(progressTimer);
	}

	//WaitingProgress* wp = WaitingProgress::create(ResourceLang::map_lang["datawaitingtext"]);
	//this->addChild(wp, 0, "waitingprogress");
	//HttpDataSwap::init(this)->vipIsOn();

	//�����²���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}



void RebatLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag == 1000)
		{
			AnimationEffect::closeAniEffect((Layer*)this);
		}
	}
}

void RebatLayer::onFinish(int code)
{
	this->removeChildByName("waitingprogress");
	if (code == 0)
	{
		
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);
	}
}