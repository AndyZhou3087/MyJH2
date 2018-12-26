#include "BuyCoinLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "ShopLayer.h"

USING_NS_CC;

BuyCoinLayer::BuyCoinLayer()
{

}

BuyCoinLayer::~BuyCoinLayer()
{

}


BuyCoinLayer* BuyCoinLayer::create(int coincount)
{
	BuyCoinLayer *pRet = new(std::nothrow)BuyCoinLayer();
	if (pRet && pRet->init(coincount))
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
bool BuyCoinLayer::init(int coincount)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("buyCoinLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["nomorecoin"]);

	cocos2d::ui::Text* title_0 = (cocos2d::ui::Text*)csbnode->getChildByName("title_0");
	title_0->setString(ResourceLang::map_lang["buycoindesc"]);

	//按钮1
	cocos2d::ui::Widget* buybtn = (cocos2d::ui::Widget*)csbnode->getChildByName("buybtn");
	buybtn->addTouchEventListener(CC_CALLBACK_2(BuyCoinLayer::onBtnClick, this));

	//按钮1文字
	cocos2d::ui::ImageView* buybtntxt = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
	buybtntxt->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	DynamicValueInt needdint;
	needdint.setValue(coincount);

	int index = 0;
	for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
	{
		if (GlobalInstance::vec_shopdata[i].show && GlobalInstance::vec_shopdata[i].type == 0)
		{
			index = i;
			if (GlobalInstance::vec_shopdata[i].count >= needdint.getValue())
				break;
		}
	}

	buybtn->setTag(index);
	cocos2d::ui::Text* price = (cocos2d::ui::Text*)csbnode->getChildByName("price");
	std::string str = StringUtils::format(ResourceLang::map_lang["shoppricetext"].c_str(), GlobalInstance::vec_shopdata[index].price);
	price->setString(str);

	DynamicValueInt shopdint;
	shopdint.setValue(GlobalInstance::vec_shopdata[index].count);

	cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");
	str = StringUtils::format("x%d", shopdint.getValue());
	countlbl->setString(str);


	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect(this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void BuyCoinLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		ShopLayer::beginPay(node->getTag());
	}
}